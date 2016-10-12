#include <string.h>
#include <iostream>
#include <sstream>

#include <nodge/BasicGraph.hh>
#include "Algorithms.hh"
#include "ModelWriter.hh"

void Populate(Graph& g){
	MetaSchema::Builder sb(g, "MetaSchema", 1);

	Graph::node_iterator types[Type::NUM_TYPES];
	for(unsigned i = 0; i < Type::NUM_TYPES; ++i){
		MetaSchema::type_spec tmp;
		tmp.cpp_type  = MetaSchema::type_list[i].cpp_type;
		tmp.enum_name = i;

		/* Add Data to the graph. */
		types[i] = g.AddNode(MetaSchema::node_specs[MetaSchema::TYPE_SPEC_NODE], tmp);
	}

	/* Common member specs. */
	Graph::node_iterator cpp_name_mem =
		sb.make_member_spec("cpp_name", types[Type::M_STRING]);
	Graph::node_iterator id_mem =
		sb.make_member_spec("id", types[Type::M_UNSIGNED_INT]);
	Graph::node_iterator name_mem =
		sb.make_member_spec("name", types[Type::M_STRING]);

	/* type_spec */
	Graph::node_iterator type_class = sb.make_data_spec("type_spec");
	sb.add_member_spec("cpp_type", types[Type::M_STRING]);
	Graph::node_iterator type_enum =
		sb.make_enum("MemberType", types[Type::M_UNSIGNED_INT]);
	sb.add_enum_value("M_BOOL", "0");
	sb.add_enum_value("M_UNSIGNED_INT", "1");
	sb.add_enum_value("M_INT", "2");
	sb.add_enum_value("M_UNSIGNED_LONG", "3");
	sb.add_enum_value("M_LONG", "4");
	sb.add_enum_value("M_STRING", "5");
	sb.add_enum_value("M_FLOAT", "6");
	sb.add_enum_value("M_DOUBLE", "7");
	sb.add_enum_value("M_VARIANT", "8");
	sb.add_member_spec("enum_name", type_enum);


	/* enum_value_spec */
	Graph::node_iterator enum_value_class = sb.make_data_spec("enum_value_spec");
	sb.link_member_spec(cpp_name_mem);
	sb.add_member_spec("value", types[Type::M_UNSIGNED_INT]);

	/* enum_spec */
	Graph::node_iterator enum_class = sb.make_data_spec("enum_spec");
	sb.link_member_spec(cpp_name_mem);

	/* mem_spec */
	Graph::node_iterator mem_class = sb.make_data_spec("mem_spec");
	sb.link_member_spec(id_mem);
	sb.link_member_spec(name_mem);

	/* data_spec */
	Graph::node_iterator data_class = sb.make_data_spec("data_spec");
	sb.link_member_spec(id_mem);
	sb.link_member_spec(name_mem);

	/* node_spec */
	Graph::node_iterator node_class = sb.make_data_spec("node_spec");
	sb.link_member_spec(id_mem);
	sb.add_member_spec("num_parent_links", types[Type::M_UNSIGNED_INT]);
	sb.add_member_spec("num_child_links", types[Type::M_UNSIGNED_INT]);
	sb.link_member_spec(name_mem);

	/* link_spec */
	Graph::node_iterator link_class = sb.make_data_spec("link_spec");
	sb.link_member_spec(id_mem);
	sb.add_member_spec("num_parents", types[Type::M_UNSIGNED_INT]);
	sb.add_member_spec("num_children", types[Type::M_UNSIGNED_INT]);
	sb.add_member_spec("num_parent_types", types[Type::M_UNSIGNED_INT]);
	sb.add_member_spec("num_children_types", types[Type::M_UNSIGNED_INT]);
	sb.link_member_spec(name_mem);
	sb.add_member_spec("ordered_by_insert", types[Type::M_BOOL]);

	/* model_spec */
	Graph::node_iterator model_class = sb.make_data_spec("model_spec");
	sb.link_member_spec(id_mem);
	sb.link_member_spec(name_mem);

	/* Now create nodes. */
	Graph::node_iterator type_node =
		sb.make_node_spec("TYPE_SPEC_NODE", type_class);
	Graph::node_iterator enum_value_node =
		sb.make_node_spec("ENUM_VALUE_SPEC_NODE", enum_value_class);
	Graph::node_iterator enum_node =
		sb.make_node_spec("ENUM_SPEC_NODE", enum_class);
	Graph::node_iterator mem_node = sb.make_node_spec("MEM_SPEC_NODE", mem_class);
	Graph::node_iterator data_node =
		sb.make_node_spec("DATA_SPEC_NODE", data_class);
	Graph::node_iterator node_node =
		sb.make_node_spec("NODE_SPEC_NODE", node_class);
	Graph::node_iterator link_node =
		sb.make_node_spec("LINK_SPEC_NODE", link_class);
	Graph::node_iterator model_node =
		sb.make_node_spec("MODEL_SPEC_NODE", model_class);

	/* Now create links. */

	/* data member links */
	sb.make_link(LinkSpec::MANY, 1, "LNK_MEM_KIND", mem_node, type_node);
	sb.add_to_link(enum_node, false);

	/* enum links */
	sb.make_link(1, LinkSpec::MANY, "LNK_ENUM_ENUM_VALUE", enum_node,
		enum_value_node, true);
	sb.make_link(LinkSpec::MANY, 1, "LNK_ENUM_TYPE", enum_node, type_node);

	/* dataspec links. */
	sb.make_link(LinkSpec::MANY, LinkSpec::MANY,
		"LNK_DATA_MEM", data_node, mem_node, true);
	sb.make_link(LinkSpec::MANY, 1, "LNK_DATA_ANCESTOR", data_node, data_node);

	/* nodespec links */
	sb.make_link(LinkSpec::MANY, 1, "LNK_NODE_DATA", node_node, data_node);

	/* linkspec links */
	sb.make_link(LinkSpec::MANY, LinkSpec::MANY, "LNK_LINK_PARENT_NODE",
		link_node, node_node);
	sb.make_link(LinkSpec::MANY, LinkSpec::MANY, "LNK_LINK_CHILD_NODE",
		link_node, node_node);
	sb.make_link(1, LinkSpec::MANY, "LNK_MODEL_LINK", model_node, link_node);

	/* To Be Used in a model allowing multiple models in the same graph. */

	/* owner model link for disambiguation */
	sb.make_link(LinkSpec::MANY, 1, "LNK_OWNER_MODEL", node_node, model_node);
	sb.add_to_link(data_node, true);
	sb.add_to_link(mem_node, true);
	sb.add_to_link(enum_node, true);

	sb.make_link(1, LinkSpec::MANY, "LNK_MODEL_ORDER", model_node, type_node, true);
	sb.add_to_link(data_node, false);
	sb.add_to_link(node_node, false);
	sb.add_to_link(link_node, false);

}

int main(int argc, const char* argv[]){
	try{
		BasicGraph model_graph(MetaSchema::model);

		Populate(model_graph);

		Graph::node_iterator ni = model_graph.begin();
		Graph::const_node_iterator end = model_graph.end();

		unsigned count = 0;

		Graph::node_iterator model_node;
		while(end != ni){
			if(MetaSchema::model.ModelID(*ni.Spec()) == MetaSchema::MODEL_SPEC_NODE){
				model_node = model_graph.Clone(ni);
			}

			Graph::link_iterator k = model_graph.neighbor_begin(ni);
			Graph::const_link_iterator iend = model_graph.neighbor_end(ni);
			while(!(iend == k))
				++k;
			++ni;
			++count;
		}

		MetaSchema::ModelWriter mw(model_graph, model_node);

		mw.WriteHeader(std::cerr);
		mw.WriteCpp(std::cerr);

		/*
		MetaSchema::Generate(model_graph, model_node);
		*/
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
