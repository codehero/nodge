#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <iostream>
#include <sstream>
#include <map>

#include <nodge/BasicGraph.hh>
#include <benejson/pull.hh>

#include "Algorithms.hh"
#include "ModelWriter.hh"
#include "posix.hh"

using BNJ::PullParser;

struct ParseState {
	uint8_t buffer[512];
	uint32_t stack[64];
	FD_Reader *reader;
	PullParser *parser;
	ParseState *parent;
	std::vector<ParseState*> children;
	unsigned current_state;
	bool started;
};

typedef std::map<string, Graph::node_iterator> Env;
typedef std::map<string, ParseState*> ProcessOrdering;

typedef std::list<ParseState*> Queue;

enum {
	HDR_IMPORT
	,HDR_PROPERTIES
	,HDR_MAKE_TYPE
	,HDR_MAKE_ENUM
	,HDR_MAKE_MEMBER_SPEC
	,HDR_MAKE_DATA_SPEC
	,HDR_MAKE_NODE_SPEC
	,HDR_MAKE_LINK
	,COUNT_HDR_TYPES
};

static const char* s_header_orders[COUNT_HDR_TYPES] = {
	"import"
	,"properties"
	,"make_type"
	,"make_enum"
	,"make_member_spec"
	,"make_data_spec"
	,"make_node_spec"
	,"make_link_spec"
};

#if 0
enum {
	PROP_KV_NAME
	,COUNT_KV_PROPS
};

static const char* s_prop_kv[COUNT_KV_PROPS] = {
	"name"
};
#endif

enum {
	MEMSPEC_KV_KIND
	,MEMSPEC_KV_NAME
	,COUNT_KV_MEMSPEC
};

static const char* s_prop_memspec[COUNT_KV_MEMSPEC] = {
	"kind"
	,"name"
};


enum {
	MK_DS_ADD_MEM_SPEC
	,MK_DS_LINK_MEM_SPEC
	,COUNT_MK_DS
};

static const char* s_mk_ds[COUNT_MK_DS] = {
	"add_member_spec"
	,"link_member_spec"
};


BasicGraph s_model_graph(MetaSchema::model);
BasicGraph s_import_graph(MetaSchema::model);

MetaSchema::Builder* s_sb = NULL;
Env s_environment;
ProcessOrdering s_ordering;


ParseState* BeginFile(int fd, Graph::node_iterator parent){
	ParseState *ps = new ParseState;
	ps->reader = new FD_Reader(fd);
	ps->parser = new PullParser(64, ps->stack);
	ps->parser->Begin(ps->buffer, 512, ps->reader);
	ps->parent = NULL;
	ps->current_state = HDR_IMPORT;


	/* Top level object is a list. */
	ps->parser->Pull();
	BNJ::VerifyList(*(ps->parser));

	ps->parser->Pull();
	BNJ::VerifyList(*(ps->parser));

	/* Expect start of import list. */
	ps->parser->Pull();
	char tmp[128];
	ps->parser->ChunkRead8(tmp, 128);
	if(strcmp("import", tmp))
		throw PullParser::invalid_value("Expected import!", *ps->parser);

	/* Process the import list. */
	while(PullParser::ST_ASCEND_LIST != ps->parser->Pull()){
		ps->parser->Pull();
		ps->parser->ChunkRead8(tmp, 128);

		/* See if this one has already been read. */
		ProcessOrdering::const_iterator i = s_ordering.find(tmp);
		if(s_ordering.end() == i){
			/* This is a filename containing another schema. */
			int fd = open(tmp, O_RDONLY | O_CLOEXEC);
			if(-1 == fd)
				throw PullParser::invalid_value("Bad import file!", *ps->parser);
			s_ordering[tmp] = NULL;

			MetaSchema::data_spec ds;
			ds.name = tmp;
			Graph::node_iterator child = s_import_graph.AddNode(
				MetaSchema::node_specs[MetaSchema::DATA_SPEC_NODE], ds);
			s_import_graph.AddLink(
					MetaSchema::link_specs[MetaSchema::LNK_DATA_ANCESTOR], child, parent);

			ParseState* pc = BeginFile(fd, child);

			/* Check for circularity... */
			ProcessOrdering::const_iterator i = s_ordering.find(tmp);
			s_ordering[tmp] = pc;
		}

	}

	ps->parser->Pull();
	BNJ::VerifyList(*(ps->parser));

	ps->parser->Pull();
	ps->parser->ChunkRead8(tmp, 128);
	if(strcmp("properties", tmp))
		throw PullParser::invalid_value("Expected import!", *ps->parser);

	ps->parser->Pull();
	BNJ::VerifyMap(*(ps->parser));
	ps->parser->Up();

	if(PullParser::ST_ASCEND_LIST != ps->parser->Pull())
		throw PullParser::invalid_value("Expected property end!", *ps->parser);

	ps->current_state = HDR_MAKE_TYPE;

	return ps;
}

void make_type(ParseState& ps){
	BNJ::VerifyList(*(ps.parser));
	/* FIXME */
	ps.parser->Up();

	ps.current_state = HDR_MAKE_ENUM;
}

void make_enum(ParseState& ps){
	BNJ::VerifyMap(*ps.parser);
	while(PullParser::ST_ASCEND_MAP != ps.parser->Pull()){
		char enum_node_name[64];
		BNJ::GetKey(enum_node_name, 64, *ps.parser);

		BNJ::VerifyList(*ps.parser);

		ps.parser->Pull();
		char cpp_name[64];
		ps.parser->ChunkRead8(cpp_name, 64);

		ps.parser->Pull();
		char type_node_name[64];
		ps.parser->ChunkRead8(type_node_name, 64);

		Env::const_iterator type_node = s_environment.find(type_node_name);
		if(s_environment.end() == type_node)
			throw PullParser::invalid_value("Invalid type id!", *ps.parser);

		Graph::node_iterator enum_i = s_sb->make_enum(cpp_name, type_node->second);
		s_environment[enum_node_name] = enum_i;

		ps.parser->Pull();
		BNJ::VerifyList(*(ps.parser));
		while(PullParser::ST_ASCEND_LIST != ps.parser->Pull()){
			BNJ::VerifyList(*(ps.parser));

			ps.parser->Pull();
			char enum_str[64];
			ps.parser->ChunkRead8(enum_str, 64);

			ps.parser->Pull();
			char enum_val[64];
			ps.parser->ChunkRead8(enum_val, 64);

			if(PullParser::ST_ASCEND_LIST != ps.parser->Pull())
				throw PullParser::invalid_value("Overlong enum value list length!", *ps.parser);
			s_sb->add_enum_value(enum_str, enum_val);
		}

		if(PullParser::ST_ASCEND_LIST != ps.parser->Pull())
			throw PullParser::invalid_value("Expected end of enum!", *ps.parser);
	}

	ps.current_state = HDR_MAKE_MEMBER_SPEC;
}

Graph::node_iterator make_member_spec_help(PullParser& parser){
		BNJ::VerifyMap(parser);
		Env::const_iterator kind;
		char cpp_name[64];
		while(PullParser::ST_ASCEND_MAP != parser.Pull(s_prop_memspec, COUNT_KV_MEMSPEC)){

			switch(parser.GetValue().key_enum){
				case MEMSPEC_KV_KIND:
					{
						char tmp[64];
						parser.ChunkRead8(tmp, 64);
						kind = s_environment.find(tmp);
						if(s_environment.end() == kind){
							throw PullParser::invalid_value("Invalid kind id!", parser);
						}
					}
					break;

				case MEMSPEC_KV_NAME:
					parser.ChunkRead8(cpp_name, 64);
					break;

				default:
					throw PullParser::invalid_value("Invalid Memspec key!", parser);
			}
		}
		return s_sb->make_member_spec(cpp_name, kind->second);
}

void make_member_spec(ParseState& ps){
	BNJ::VerifyMap(*ps.parser);

	while(PullParser::ST_ASCEND_MAP != ps.parser->Pull()){
		char member_node_name[64];
		BNJ::GetKey(member_node_name, 64, *ps.parser);
		Graph::node_iterator r = make_member_spec_help(*ps.parser);
		s_environment[member_node_name] = r;
	}

	ps.current_state = HDR_MAKE_DATA_SPEC;
}

void make_data_spec(ParseState& ps){
	BNJ::VerifyMap(*ps.parser);

	while(PullParser::ST_ASCEND_MAP != ps.parser->Pull()){
		char data_node_name[64];
		BNJ::GetKey(data_node_name, 64, *ps.parser);

		Graph::node_iterator data_node = s_sb->make_data_spec(data_node_name);
		s_environment[data_node_name] = data_node;

		BNJ::VerifyList(*ps.parser);
		while(PullParser::ST_ASCEND_LIST != ps.parser->Pull()){
			BNJ::VerifyList(*ps.parser);

			ps.parser->Pull();
			char cmd[64];
			ps.parser->ChunkRead8(cmd, 64);

			/* Lookup command. */
			unsigned c;
			for(c = 0; c < COUNT_MK_DS; ++c){
				if(!strcmp(cmd, s_mk_ds[c]))
					break;
			}

			ps.parser->Pull();
			switch(c){
				case MK_DS_ADD_MEM_SPEC:
					make_member_spec_help(*ps.parser);
					break;

				case MK_DS_LINK_MEM_SPEC:
					{
						char tmp[64];
						ps.parser->ChunkRead8(tmp, 64);
						Env::const_iterator mem = s_environment.find(tmp);
						if(s_environment.end() == mem)
							throw PullParser::invalid_value("Invalid memberspec id!", *ps.parser);
						s_sb->link_member_spec(mem->second);
					}
					break;

				default:
					throw PullParser::invalid_value("Bad data spec command", *ps.parser);
			}
			if(ps.parser->Pull() != PullParser::ST_ASCEND_LIST)
				throw PullParser::invalid_value("Overlong command!", *ps.parser);
		}
	}

	ps.current_state = HDR_MAKE_NODE_SPEC;
}

void make_node_spec(ParseState& ps){
	BNJ::VerifyMap(*ps.parser);

	while(PullParser::ST_ASCEND_MAP != ps.parser->Pull()){
		char node_node_name[64];
		BNJ::GetKey(node_node_name, 64, *ps.parser);

		char tmp[64];
		ps.parser->ChunkRead8(tmp, 64);

		Env::const_iterator data_spec = s_environment.find(tmp);
		if(s_environment.end() == data_spec)
			throw PullParser::invalid_value("Invalid memberspec id!", *ps.parser);
		s_environment[node_node_name] =
			s_sb->make_node_spec(node_node_name, data_spec->second);
	}

	ps.current_state = HDR_MAKE_LINK;
}

void make_link_spec(ParseState& ps){

	while(PullParser::ST_ASCEND_MAP != ps.parser->Pull()){
		BNJ::VerifyList(*ps.parser);

		char link_node_name[64];
		BNJ::GetKey(link_node_name, 64, *ps.parser);

		unsigned num_parents;
		unsigned num_children;
		Env::const_iterator parent_node;
		Env::const_iterator child_node;

		ps.parser->Pull();
		BNJ::Get(num_parents, *ps.parser);

		ps.parser->Pull();
		BNJ::Get(num_children, *ps.parser);

		char tmp[64];
		ps.parser->Pull();
		ps.parser->ChunkRead8(tmp, 64);
		parent_node = s_environment.find(tmp);
		if(s_environment.end() == parent_node)
			throw PullParser::invalid_value("Invalid parent node!", *ps.parser);

		ps.parser->Pull();
		ps.parser->ChunkRead8(tmp, 64);
		child_node = s_environment.find(tmp);
		if(s_environment.end() == child_node)
			throw PullParser::invalid_value("Invalid child node!", *ps.parser);

		ps.parser->Pull();
		bool ordered_by_insert;
		BNJ::Get(ordered_by_insert, *ps.parser);

		Graph::node_iterator link_node = s_sb->make_link(
				num_parents, num_children, link_node_name,
				parent_node->second, child_node->second, ordered_by_insert);

		ps.parser->Pull();
		BNJ::VerifyList(*ps.parser);
		while(PullParser::ST_ASCEND_LIST != ps.parser->Pull()){
			BNJ::VerifyList(*ps.parser);

			ps.parser->Pull();
			ps.parser->ChunkRead8(tmp, 64);
			Env::iterator node = s_environment.find(tmp);
			if(s_environment.end() == node)
				throw PullParser::invalid_value("Invalid additional link node!", *ps.parser);

			ps.parser->Pull();
			bool is_parent;
			BNJ::Get(is_parent, *ps.parser);

			s_sb->add_to_link(node->second, is_parent);

			if(PullParser::ST_ASCEND_LIST != ps.parser->Pull())
				throw PullParser::invalid_value("Overlong add'l node entry!", *ps.parser);
		}
		if(PullParser::ST_ASCEND_LIST != ps.parser->Pull())
			throw PullParser::invalid_value("Overlong link entry!", *ps.parser);
	}

	ps.current_state = COUNT_HDR_TYPES;
}


int main(int argc, const char* argv[]){
	try{
		if(argc < 2)
			throw std::runtime_error("Need model name!");

		/* Initialize with common types. */
		MetaSchema::Builder sb(s_model_graph, argv[1], 1);
		s_sb = &sb;
		for(unsigned i = 0; i < Type::NUM_TYPES; ++i){
			MetaSchema::type_spec tmp;
			tmp.cpp_type  = MetaSchema::type_list[i].cpp_type;
			tmp.enum_name = MetaSchema::type_list[i].enum_name;

			/* Add Data to the graph. */
			Graph::node_iterator n = sb.make_type(tmp.enum_name, tmp.cpp_type);
			s_environment[tmp.enum_name] = n;
		}

		Queue queue;

		/* Read file from stdin. */
		MetaSchema::data_spec ds;
		ds.name = argv[1];
		Graph::node_iterator top_node = s_import_graph.AddNode(
			MetaSchema::node_specs[MetaSchema::DATA_SPEC_NODE], ds);

		ParseState* top = BeginFile(0, top_node);
		s_ordering[ds.name] = top;

		/* Perform topological sort on the import graph. */

		queue.push_back(top);


		for(unsigned i = HDR_MAKE_TYPE; i < COUNT_HDR_TYPES; ++i){
			for(Queue::iterator k = queue.begin(); k != queue.end(); ++k){
				ParseState* ps = *k;
				ps->parser->Pull();
				BNJ::VerifyList(*ps->parser);

				ps->parser->Pull();
				char tmp[64];
				ps->parser->ChunkRead8(tmp, 64);
				if(strcmp(tmp, s_header_orders[i]))
					throw PullParser::invalid_value("Invalid header", *ps->parser);

				ps->parser->Pull();
				switch(i){
					case HDR_MAKE_TYPE:
						make_type(*ps);
						break;

					case HDR_MAKE_ENUM:
						make_enum(*ps);
						break;

					case HDR_MAKE_MEMBER_SPEC:
						make_member_spec(*ps);
						break;

					case HDR_MAKE_DATA_SPEC:
						make_data_spec(*ps);
						break;

					case HDR_MAKE_NODE_SPEC:
						make_node_spec(*ps);
						break;

					case HDR_MAKE_LINK:
						make_link_spec(*ps);
						break;

					default:
						break;

				}

				if(PullParser::ST_ASCEND_LIST != ps->parser->Pull())
					throw PullParser::invalid_value("Expected section end!", *ps->parser);
			}
		}


		/* Output phase. */
		Graph::node_iterator ni(s_model_graph.begin());
		Graph::const_node_iterator end(s_model_graph.end());

		Graph::node_iterator model_node;
		while(end != ni){
			if(MetaSchema::model.ModelID(*ni.Spec()) == MetaSchema::MODEL_SPEC_NODE){
				model_node = s_model_graph.Clone(ni);
			}
			++ni;
		}

		MetaSchema::ModelWriter mw(s_model_graph, model_node);

		mw.WriteHeader(std::cout);
		mw.WriteCpp(std::cout);
	}
	catch(const Graph::spec_mismatch& e){
		std::cerr << e.what() << std::endl;
		std::cerr << e.lspec.name << std::endl;
		std::cerr << e.parent.name << std::endl;
		std::cerr << e.child.name << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
