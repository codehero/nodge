#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <nodge/Graph.hh>
#include "Algorithms.hh"

/* Node Data references. */

namespace {
	const MetaSchema::model_spec* the_model = NULL;

	struct it_entry{
		Graph::node_iterator i;
		const Data* d;
	};

	/* Sorting Criteria */
	struct Sorter_it {
		bool operator()(const it_entry* data1, const it_entry* data2){
			const MemberSpec& sort_mem =
				MetaSchema::data_specs[MetaSchema::DATA_DATA_SPEC].member_specs[MetaSchema::data_spec::MEM_ID];
			const unsigned *id1, *id2;
			data1->d->Get(sort_mem, id1);
			data2->d->Get(sort_mem, id2);
			return *id1 < *id2;
		}
	};

	struct Sorter {
		bool operator()(const Data* data1, const Data* data2){
			const MemberSpec& sort_mem =
				MetaSchema::data_specs[MetaSchema::DATA_DATA_SPEC].member_specs[MetaSchema::data_spec::MEM_ID];
			const unsigned *id1, *id2;
			data1->Get(sort_mem, id1);
			data2->Get(sort_mem, id2);
			return *id1 < *id2;
		}
	};

	/** @brief List of generated node links. */
	std::vector<Model::NodeLinks> s_node_links;

	/* Output */

	std::ostringstream model_hh;
	std::ostringstream model_cpp;

	std::ostringstream model_node_spec_cpp;
	std::ostringstream model_link_node_spec_cpp;
	std::ostringstream model_link_spec_cpp;

	std::ostringstream data_spec_hh;
	std::ostringstream data_spec_cpp;

	std::ostringstream enum_hh;
	std::ostringstream enum_cpp;

	const MetaSchema::type_spec& determine_type(const Graph& g, const Graph::node_iterator& mi, const MetaSchema::enum_spec*& enum_data){
		/*  */
		Graph::link_iterator li;
		g.neighbor_begin(mi, li);
		Graph::node_iterator ki;
		g.at(li, false, ki);
		if(MetaSchema::model.ModelID(*ki.Spec()) == MetaSchema::ENUM_SPEC_NODE){

			enum_data = ki.Data<const MetaSchema::enum_spec>();

			Graph::link_iterator qi, end;
			g.neighbor_begin(ki, qi);
			g.neighbor_end(ki, end);

			while(qi != end){
				if(*qi.Spec() == MetaSchema::link_specs[MetaSchema::LNK_ENUM_TYPE]){
					g.at(qi, false, ki);
					break;
				}
				g.advance(qi);
			}
		}

		if(MetaSchema::model.ModelID(*ki.Spec()) != MetaSchema::TYPE_SPEC_NODE)
			throw std::runtime_error("bad type");

		return ki.Data<const MetaSchema::type_spec>();
	}

	void gen_enum(const Graph& g, const Graph::node_iterator& ni){
		/* FIXME */
		const auto* enum_data =
			static_cast<const MetaSchema::enum_spec*>(&g.Get(ni)->Get());
		model_cpp << "static EnumValue " << enum_data->cpp_name << "_vals[] = {" << std::endl;
		unsigned count = 0;

		Graph::link_iterator li, end;
		g.neighbor_begin(ni, li);
		g.neighbor_end(ni, end);
		while(li != end){
			if(*li.Spec() == MetaSchema::link_specs[MetaSchema::LNK_ENUM_ENUM_VALUE]){
				Graph::node_iterator mi;
				g.at(li, false, mi);

				const MetaSchema::enum_value_spec&
					value_data = static_cast<const MetaSchema::enum_value_spec&>(g.Get(mi)->Get());
				if(count)
					model_cpp << ",";
				model_cpp << "{\"" << value_data.cpp_name << "\", \""
					<< value_data.value << "\"}" << std::endl;
				++count;
			}
			g.advance(li);
		}
		model_cpp << "};" << std::endl;
			
		model_cpp << "static const Enum " << enum_data->cpp_name << "_spec = {" << std::endl;
		model_cpp << "\"" << enum_data->cpp_name << "\""
			<< ", " << the_model->name  << "::type_list[" << the_model->name << "::M_UNSIGNED_INT], " << enum_data->cpp_name << "_vals"
			<< ", " << count << "};"
			<< std::endl;
	}

	void gen_data(const Graph& g, const Graph::node_iterator& ni){
		const auto data = ni.Data<const MetaSchema::data_spec&>();

		/* Output header data. */
		string ancestor_name = "Data";
		data_spec_hh << "class " << data.name << " : public " << ancestor_name << " {" << std::endl;
		data_spec_hh << "public:" << std::endl;

		data_spec_hh << "static const unsigned ID = " << data.name << "_DATA_SPEC;" << std::endl;

		/* Output impl data. */
		string qualified_name = the_model->name + "::" + data.name;

		/* ctor */
		data_spec_cpp << qualified_name << "::" << data.name << "() throw(){}" << std::endl;

		/* dtor */
		data_spec_cpp << qualified_name << "::~" << data.name << "() throw(){}" << std::endl;

		/* Clone function. */
		data_spec_cpp << qualified_name << "* " << qualified_name << "::Clone() const{" << std::endl;
		data_spec_cpp << "\t" << data.name << "* retval = new " << data.name << ";" << std::endl;
		data_spec_cpp << "\t*retval = *this;" << std::endl;
		data_spec_cpp << "\treturn retval;" << std::endl;
		data_spec_cpp << "}" << std::endl;
		data_spec_cpp << std::endl;

		/* Print out the memberspec stuff. */
		{
			data_spec_cpp << "static const MemberSpec " << data.name << "_mems[] = {" << std::endl;
			Graph::link_iterator li, end;
			g.neighbor_begin(ni, li);
			g.neighbor_end(ni, end);

			/* Where will we print memberspec enums. */
			std::ostringstream mem_enum_ostr;
			mem_enum_ostr << "enum {" << std::endl;

			std::vector<const MetaSchema::mem_spec*> mems;
			std::vector<const MetaSchema::type_spec*> types;

			while(li != end){
				if(*li.Spec() == MetaSchema::link_specs[MetaSchema::LNK_DATA_MEM]){
					Graph::node_iterator mi;
					g.at(li, false, mi);
					const MetaSchema::mem_spec& mem =
						static_cast<const MetaSchema::mem_spec&>(g.Get(mi)->Get());

					const MetaSchema::enum_spec* edata = NULL;
					const MetaSchema::type_spec& mem_t = determine_type(g, mi, edata);

					data_spec_hh << mem_t.cpp_type << " " << mem.name << ";" << std::endl;
					data_spec_cpp << "MemberSpec(" << qualified_name << "::" << mem.name << "_MEM_ID,"
						<< "Type::" << Type::type_list[mem_t.enum_name].enum_name
						<< ", \"" << mem.name << "\", ";

					if(!edata)
						data_spec_cpp << "NULL)," << std::endl;
					else{
						data_spec_cpp << '&' << edata->cpp_name << "_spec)," << std::endl;
					}

					mem_enum_ostr << "\t" << mem.name << "_MEM_ID," << std::endl;

					mems.push_back(&mem);
					types.push_back(&mem_t);
				}
				g.advance(li);
			}
			data_spec_cpp << "};" << std::endl << std::endl;
			mem_enum_ostr << "};" << std::endl;
			data_spec_hh << mem_enum_ostr.str();

			for(unsigned i = 0; i < Type::NUM_TYPES; ++i){
				bool found = false;
				for(unsigned k = 0; k < types.size(); ++k){
					if(types[k]->enum_name == i){
						if(!found){
							data_spec_cpp << "void " << qualified_name
								<< "::Get(const MemberSpec& ms, const "
								<< types[k]->cpp_type
								<< "*& x) const throw(parameter_error) {" << std::endl
								<< "switch(ms.id){" << std::endl;
							found = true;
						}
						data_spec_cpp << "case " << mems[k]->name << "_MEM_ID:" << std::endl;
						data_spec_cpp << "x = &" << mems[k]->name << "; break;" << std::endl;

					}
				}
				if(found){
					data_spec_cpp << "default: Data::Get(ms, x);" << std::endl;
					data_spec_cpp << "}" << std::endl;
					data_spec_cpp << "}" << std::endl;

					data_spec_hh << "void Get(const MemberSpec& id, const "
						<< Type::type_list[i].cpp_type << "*& x) const throw(parameter_error);"
						<< std::endl;
				}
			}
		}

		/* Common hh stuff. */
		data_spec_hh << data.name << "(void) throw();" << std::endl;
		data_spec_hh << '~' << data.name << "() throw();" << std::endl;
		data_spec_hh << data.name << "* Clone(void) const;" << std::endl;

		/* hh.GetSpec */
		data_spec_hh << "const Spec& GetSpec(void) const throw(){" << std::endl;
		data_spec_hh << "return data_specs[" << data.name << "_DATA_SPEC];" << std::endl;
		data_spec_hh << "}" << std::endl;

		/* hh.CreateInstance */
		data_spec_hh << "static Data* CreateInstance(void){" << std::endl;
		data_spec_hh << "return new " << data.name << ";" << std::endl;
		data_spec_hh << "}" << std::endl;

		data_spec_hh << "};" << std::endl << std::endl;
	}

	void gen_node(const Graph& g, const Graph::node_iterator& ni){
		/* Output NodeSpec Table. */
		const MetaSchema::node_spec& node =
			static_cast<const MetaSchema::node_spec&>(g.Get(ni)->Get());

		Graph::node_iterator di;
		const string& n = node.name;
		const Graph::Node* dn =
			g.Get(ni, MetaSchema::link_specs[MetaSchema::LNK_NODE_DATA], di);
		if(!dn)
			throw std::runtime_error("Missing data spec!");
		const string& data_name = static_cast<const MetaSchema::data_spec&>(dn->Get()).name;
		model_node_spec_cpp << "NodeSpec(" << the_model->name << "::model, \""
			<< n << "_node\", " << the_model->name << "::data_specs["
			<< the_model->name << "::" << data_name << "::ID])," << std::endl;
	}

	void gen_node_link(const Graph& g, const Graph::node_iterator& ni){

		/* Graph search should create table indexed by node_spec and ordered by node_spec.id;
		 * The task here is to create a table indexed by the node_spec
		 * such that:
		 *
		 * -Column 1 = count of link_spec->lnk_child_node->node_spec
		 * -Column 2 = count of link_spec->lnk_parent_node->node_spec
		 * -Column 3 = count of ((link_spec->lnk_parent_node->node_spec
		 *  	where link_spec.num_children == MANY) ||
		 *    (link_spec->lnk_child_node->node_spec
		 *  	where link_spec.num_parents == MANY))
		 *
		 * -Column 4 = count of ((link_spec->lnk_parent_node->node_spec
		 *  	where link_spec.num_children != MANY) &&
		 *    (link_spec->lnk_child_node->node_spec
		 *  	where link_spec.num_parents != MANY))
		 *
		 *
		 * Summary Values:
		 * -Column 0 (offset) should be determined after traversal;
		 *  counting will increase linearly
		 *  by a later function (sum of 1,2,3,4) of all previous rows
		 *
		 * */

		std::ostringstream ostr_parents;
		std::ostringstream ostr_children;
		std::ostringstream ostr_many;
		std::ostringstream ostr_constant;

		Model::NodeLinks tmp = {0, 0, 0, 0, 0, 0};

		static unsigned s_offset_total = 0;
		tmp.offset = s_offset_total;

		Graph::link_iterator qi, end;
		g.neighbor_begin(ni, qi);
		g.neighbor_end(ni, end);

		while(qi != end){
			if(!(*qi.Spec() == MetaSchema::link_specs[MetaSchema::LNK_LINK_PARENT_NODE]) &&
				!(*qi.Spec() == MetaSchema::link_specs[MetaSchema::LNK_LINK_CHILD_NODE]))
			{
				g.advance(qi);
				continue;
			}

			Graph::node_iterator mi;
			g.at(qi, true, mi);
			const MetaSchema::link_spec&
				link = static_cast<const MetaSchema::link_spec&>(g.Get(mi)->Get());

			if(*qi.Spec() == MetaSchema::link_specs[MetaSchema::LNK_LINK_CHILD_NODE]){
				++tmp.parent_count;
				ostr_parents << "&" << the_model->name << "::link_specs["
					<< the_model->name << "::" << link.name << "]," << std::endl;
				if(link.num_parents){
					++tmp.constant_count;
					tmp.max_const_neighors += link.num_parents;

					ostr_constant << "&" << the_model->name << "::link_specs["
						<< the_model->name << "::" << link.name << "]," << std::endl;
				}
				else{
					++tmp.many_count;
					ostr_many << "&" << the_model->name << "::link_specs["
						<< the_model->name << "::" << link.name << "]," << std::endl;
				}
			}
			else {
				++tmp.child_count;

				ostr_children << "&" << the_model->name << "::link_specs["
					<< the_model->name << "::" << link.name << "]," << std::endl;
				if(link.num_children){
					++tmp.constant_count;
					tmp.max_const_neighors += link.num_children;

					ostr_constant << "&" << the_model->name << "::link_specs["
						<< the_model->name << "::" << link.name << "]," << std::endl;
				}
				else {
					++tmp.many_count;
					ostr_many << "&" << the_model->name << "::link_specs["
						<< the_model->name << "::" << link.name << "]," << std::endl;
				}
			}

			g.advance(qi);
		}
		s_offset_total += tmp.parent_count + tmp.child_count
			+ tmp.many_count + tmp.constant_count;

		s_node_links.push_back(tmp);

		model_cpp << "/* */" << std::endl;
		model_cpp << ostr_parents.str() << std::endl;
		model_cpp << ostr_children.str() << std::endl;
		model_cpp << ostr_many.str() << std::endl;
		model_cpp << ostr_constant.str() << std::endl;
	}

	void gen_link(const Graph& g, const Graph::node_iterator& ni){
		static unsigned s_arr_counter = 0;
		const MetaSchema::link_spec& lspec = 
			static_cast<const MetaSchema::link_spec&>((*ni)->Get());

		model_link_spec_cpp << "LinkSpec("+the_model->name << "::model"
			<< ",\"" << lspec.name << "\""
			<< "," << lspec.num_parents
			<< "," << lspec.num_children
			<< "," << lspec.num_parent_types
			<< "," << lspec.num_children_types
			<< ",link_node_specs + " << s_arr_counter
			<< ")," << std::endl;

		Graph::link_iterator qi, end;
		g.neighbor_begin(ni, qi);
		g.neighbor_end(ni, end);

		while(qi != end){
			if(*qi.Spec() == MetaSchema::link_specs[MetaSchema::LNK_LINK_PARENT_NODE]){
				Graph::node_iterator ki;
				g.at(qi, false, ki);
				const MetaSchema::node_spec& ns = static_cast<const MetaSchema::node_spec&>((*ki)->Get());
				model_link_node_spec_cpp
					<< the_model->name << "::node_specs + "<< the_model->name << "::" << ns.name << "," << std::endl;
				++s_arr_counter;
			}
			g.advance(qi);
		}

		g.neighbor_begin(ni, qi);
		g.neighbor_end(ni, end);

		while(qi != end){
			if(*qi.Spec() == MetaSchema::link_specs[MetaSchema::LNK_LINK_CHILD_NODE]){
				Graph::node_iterator ki;
				g.at(qi, false, ki);
				const MetaSchema::node_spec& ns = static_cast<const MetaSchema::node_spec&>((*ki)->Get());

				model_link_node_spec_cpp
					<< the_model->name << "::node_specs + "<< the_model->name << "::" << ns.name << "," << std::endl;
				++s_arr_counter;
			}
			g.advance(qi);
		}

		model_link_node_spec_cpp << std::endl;
	}
}

void MetaSchema::Generate(const Graph& g, const Graph::node_iterator& model_node){


	/* Init global data. */
	the_model = static_cast<const model_spec*>(&(g.Get(model_node)->Get()));

	/* Header output. */
	model_hh << "#ifndef __" << the_model->name << "_MODEL_HH__" << std::endl;
	model_hh << "#define __" << the_model->name << "_MODEL_HH__" << std::endl;
	model_hh << "#include <nodge/Model.hh>" << std::endl;
	model_hh << "namespace " << the_model->name << " { " << std::endl;

	/* Arrays of Data, Node, Link Specs. */
	std::vector<const data_spec*> data_specs;
	std::vector<it_entry*> node_specs;
	std::vector<it_entry*> link_specs;

	model_node_spec_cpp << "const NodeSpec " << the_model->name
		<< "::node_specs[NODE_SPEC_COUNT] = {" << std::endl;

	/* We will encounter each node only once. */
	Graph::node_iterator ni, end;
	g.begin(ni);
	g.end(end);
	while(ni != end){
		const Graph::Node& node = *g.Get(ni);
		switch(model.ModelID(node.Spec())){
				
			case ENUM_SPEC_NODE:
				gen_enum(g, ni);
				break;

			case DATA_SPEC_NODE:
				data_specs.push_back(static_cast<const data_spec*>(&node.Get()));
				gen_data(g, ni);
				break;

			case NODE_SPEC_NODE:
				{
					it_entry* x = new it_entry;
					g.Clone(ni, x->i);
					x->d = &node.Get();
					node_specs.push_back(x);
				}
				break;

			case LINK_SPEC_NODE:
				{
					it_entry* x = new it_entry;
					g.Clone(ni, x->i);
					x->d = &node.Get();
					link_specs.push_back(x);
				}
				break;

			default:
				break;
		}
		g.advance(ni);
	}

	/* Sort arrays. */
	Sorter s;
	Sorter_it sit;
	std::sort(data_specs.begin(), data_specs.end(), s);
	std::sort(node_specs.begin(), node_specs.end(), sit);
	std::sort(link_specs.begin(), link_specs.end(), sit);

	model_link_spec_cpp
		<< "const LinkSpec " << the_model->name << "::link_specs[] = {" << std::endl;
	model_link_node_spec_cpp
		<< "const NodeSpec* link_node_specs[] = {" << std::endl;

	for(unsigned i = 0; i < link_specs.size(); ++i){
		gen_link(g, link_specs[i]->i);
	}

	for(unsigned i = 0; i < node_specs.size(); ++i){
		gen_node(g, node_specs[i]->i);
	}

	model_link_spec_cpp << "};";
	model_link_node_spec_cpp << "};";
	model_node_spec_cpp << "};";

	/* Output defined enums. */
	model_hh << enum_hh.str();
	model_cpp << enum_cpp.str();

	/* Output Enum Lists. */
	unsigned i;
	model_hh << "\tenum {" << std::endl;
	for(i = 0; i < data_specs.size(); ++i){
		model_hh << "\t\t" << data_specs[i]->name << "_DATA_SPEC," << std::endl;
	}
	model_hh << "\t\tDATA_SPEC_COUNT" << std::endl;
	model_hh << "\t};" << std::endl;
	model_hh << "extern const Data::Spec data_specs[DATA_SPEC_COUNT];" << std::endl;

	model_hh << "\tenum {" << std::endl;
	for(i = 0; i < node_specs.size(); ++i){
		model_hh << "\t\t" << static_cast<const MetaSchema::node_spec*>(node_specs[i]->d)->name << "," << std::endl;
	}
	model_hh << "\t\tNODE_SPEC_COUNT" << std::endl;
	model_hh << "\t};" << std::endl;
	model_hh << "extern const NodeSpec node_specs[NODE_SPEC_COUNT];" << std::endl;

	model_hh << "\tenum {" << std::endl;
	for(i = 0; i < link_specs.size(); ++i){
		model_hh << "\t\t" << static_cast<const MetaSchema::link_spec*>(link_specs[i]->d)->name << "," << std::endl;
	}
	model_hh << "\t\tLINK_SPEC_COUNT" << std::endl;
	model_hh << "\t};" << std::endl;
	model_hh << "extern const LinkSpec link_specs[LINK_SPEC_COUNT];" << std::endl;

	model_hh << "extern Model model;" << std::endl;

	/* Output data spec header and implementation. */
	model_hh << data_spec_hh.str();
	model_cpp << data_spec_cpp.str();

	/* Output Data::Spec Table. */
	model_cpp << "const Data::Spec " << the_model->name
		<< "::data_specs[DATA_SPEC_COUNT] = {" << std::endl;
	for(i = 0; i < data_specs.size(); ++i){
		const string& d = data_specs[i]->name;
		model_cpp << "Data::Spec(\"" << d << "\", " << d << "_mems, "
			<< "sizeof(" << d << "_mems) / sizeof(MemberSpec) " << ", &" << d << "::CreateInstance)," << std::endl;
	}
	model_cpp << "};" << std::endl;

	model_cpp << model_node_spec_cpp.str() << std::endl;
	model_cpp << model_link_node_spec_cpp.str() << std::endl;
	model_cpp << model_link_spec_cpp.str() << std::endl;

	model_cpp << "static const LinkSpec* link_index_arr[] = {" << std::endl;
	for(unsigned i = 0; i < node_specs.size(); ++i){
		gen_node_link(g, node_specs[i]->i);
	}
	model_cpp << "};" << std::endl;

	model_cpp << "static Model::NodeLinks node_index_arr[] = {" << std::endl;
	for(unsigned i = 0; i < s_node_links.size(); ++i){
		const Model::NodeLinks& nl = s_node_links[i];
		model_cpp << "{" << nl.offset
			<< "," << nl.parent_count
			<< "," << nl.child_count
			<< "," << nl.many_count
			<< "," << nl.constant_count
			<< "," << nl.max_const_neighors << "}," << std::endl;
	}
	model_cpp << "};" << std::endl;


	model_cpp << "Model " << the_model->name << "::model(string(\"" << the_model->name << "\")," << std::endl
		<< the_model->name << "::node_specs," << std::endl
		<< the_model->name << "::NODE_SPEC_COUNT," << std::endl
		<< the_model->name << "::link_specs," << std::endl
		<< the_model->name << "::LINK_SPEC_COUNT," << std::endl
		<< the_model->name << "::data_specs," << std::endl
		<< the_model->name << "::DATA_SPEC_COUNT," << std::endl
		<< "NULL, 0," << std::endl
		<< "link_index_arr, sizeof(link_index_arr) / sizeof(const LinkSpec*)," << std::endl
		<< "node_index_arr, sizeof(node_index_arr) / sizeof(Model::NodeLinks)" << std::endl
		<< ");" << std::endl;

	/* End header file. */
	model_hh << "}" << std::endl;
	model_hh << "#endif" << std::endl;

	std::cout << model_hh.str();
	std::cout << model_cpp.str();
}
