#include <nodge/BasicGraph.hh>

#include "Algorithms.hh"

/* TypeNodes */

/* Builder */

MetaSchema::Builder::Builder(Graph& model_graph, const string& name,
	unsigned id_val)
	: _model_graph(model_graph),
	_data_id_count(0), _node_id_count(0), _mem_id_count(0), _link_count(0)
{
	model_spec tmp;
	tmp.name = name;
	tmp.id = id_val;

	_model_node = _model_graph.AddNode(node_specs[MODEL_SPEC_NODE], tmp);
}

MetaSchema::Builder::~Builder(){
}

Graph::node_iterator MetaSchema::Builder::make_type(
	const string& enum_name, const string& cpp_type)
{
	type_spec tmp;
	tmp.cpp_type  = cpp_type;
	tmp.enum_name = enum_name;

	/* Add Data to the graph. */
	Graph::node_iterator t = _model_graph.AddNode(
		MetaSchema::node_specs[MetaSchema::TYPE_SPEC_NODE], tmp);
	_model_graph.AddLink(link_specs[LNK_MODEL_ORDER], _model_node, t);

	_type_nodes.push_back(t);;

	return t;
}

Graph::node_iterator MetaSchema::Builder::make_enum(const string& name,
	const Graph::node_iterator& type_ref)
{
	_cur_enum_order = 0;

	/* Initialize enum data. */
	enum_spec tmp;
	tmp.cpp_name = name;

	/* Create new enum node and link it to the type.. */
	_cur_enum_spec = _model_graph.AddNode(node_specs[ENUM_SPEC_NODE], tmp);

	/* Cross graph link; update the table. */
	_model_graph.AddLink(link_specs[LNK_ENUM_TYPE], _cur_enum_spec, type_ref);

	_model_graph.AddLink(link_specs[LNK_OWNER_MODEL], _cur_enum_spec, _model_node);

	return _cur_enum_spec;
}

Graph::node_iterator
	MetaSchema::Builder::add_enum_value(const string& name, const string& value)
{
	/* Check for valid enum_spec */
	if(!_cur_enum_spec.i)
		throw std::runtime_error("Attempted to add enum value without enum!");

	enum_value_spec tmp;
	tmp.cpp_name = name;
	tmp.value = value;
	tmp.order = _cur_enum_order++;

	Graph::node_iterator ev = _model_graph.AddNode(
		node_specs[ENUM_VALUE_SPEC_NODE], tmp);
	_model_graph.AddLink(link_specs[LNK_ENUM_ENUM_VALUE], _cur_enum_spec, ev);

	return ev;
}

Graph::node_iterator MetaSchema::Builder::make_member_spec(const string& name,
	const Graph::node_iterator& type)
{
	if(!type.i)
		throw std::runtime_error("Attempted to add member_spec without proper type!");
	/* Set the members of the member. */
	mem_spec mem_tmp;
	mem_tmp.id = _mem_id_count++;
	mem_tmp.name = name;

	/* Create the node for the member. */
	Graph::node_iterator mem_node = _model_graph.AddNode(
		node_specs[MEM_SPEC_NODE], mem_tmp);

	/* Link member to its kind. */
	_model_graph.AddLink(link_specs[LNK_MEM_KIND], mem_node, type);

	/* Link to its owning model. */
	//_model_graph.AddLink(link_specs[LNK_OWNER_MODEL], mem_node, _model_node);
	return mem_node;
}

Graph::node_iterator MetaSchema::Builder::make_data_spec(const string& name,
	const Graph::node_iterator* ancestor)
{
	/* Ready the state for the next member specs. */
	data_spec tmp;
	tmp.name = name;
	tmp.id = _data_id_count++;

	/* FIXME add ancestor support. */

	_cur_data_spec = _model_graph.AddNode(node_specs[DATA_SPEC_NODE], tmp);
	_model_graph.AddLink(link_specs[LNK_OWNER_MODEL], _cur_data_spec, _model_node);
	_model_graph.AddLink(link_specs[LNK_MODEL_ORDER], _model_node, _cur_data_spec);

	if(ancestor)
		_model_graph.AddLink(link_specs[LNK_DATA_ANCESTOR], _cur_data_spec, *ancestor);

	return _cur_data_spec;
}

Graph::node_iterator MetaSchema::Builder::make_node_spec(
	const std::string& name, const Graph::node_iterator& data_spec_ref)
{
	node_spec node_tmp;
	node_tmp.id = _node_id_count++;
	node_tmp.name = name;
	node_tmp.num_parent_links = 0;
	node_tmp.num_child_links = 0;

	Graph::node_iterator n = _model_graph.AddNode(
		node_specs[NODE_SPEC_NODE], node_tmp);
	_model_graph.AddLink(link_specs[LNK_OWNER_MODEL], n, _model_node);
	_model_graph.AddLink(link_specs[LNK_NODE_DATA], n, data_spec_ref);
	_model_graph.AddLink(link_specs[LNK_MODEL_ORDER], _model_node, n);

	return n;
}


Graph::node_iterator MetaSchema::Builder::add_member_spec(const string& name,
	const Graph::node_iterator& type)
{
	/* Check for valid data spec */
	if(!_cur_data_spec.i)
		throw std::runtime_error("Attempted to add member_spec without data_spec!");

	if(!type.i)
		throw std::runtime_error("Attempted to add member_spec without proper type!");
	/* Set the members of the member. */
	mem_spec mem_tmp;
	mem_tmp.id = _mem_id_count++;
	mem_tmp.name = name;

	/* Create the node for the member. */
	Graph::node_iterator mem_node = _model_graph.AddNode(
		node_specs[MEM_SPEC_NODE], mem_tmp);

	/* link the member to the current data spec. */
	_model_graph.AddLink(link_specs[LNK_DATA_MEM], _cur_data_spec, mem_node);

	/* Link member to its kind. */
	_model_graph.AddLink(link_specs[LNK_MEM_KIND], mem_node, type);

	/* Link to its owning model. */
	//_model_graph.AddLink(link_specs[LNK_OWNER_MODEL], mem_node, _model_node);
	return mem_node;
}

void MetaSchema::Builder::link_member_spec(
	const Graph::node_iterator& mem_node)
{
	/* Check for valid data spec */
	if(!_cur_data_spec.i)
		throw std::runtime_error("Attempted to add member_spec without data_spec!");

	/* link the member to the current data spec. */
	_model_graph.AddLink(link_specs[LNK_DATA_MEM], _cur_data_spec, mem_node);

	/* Link to its owning model. */
	//_model_graph.AddLink(link_specs[LNK_OWNER_MODEL], mem_node, _model_node);
}

Graph::node_iterator
	MetaSchema::Builder::make_link(unsigned num_parents, unsigned num_children,
	const string& name, const Graph::node_iterator& parent,
	const Graph::node_iterator& child, bool ordered_by_insert)
{
	/* set the data members of the link. */
	link_spec link_tmp;
	link_tmp.id = _link_count++;
	link_tmp.num_parents = num_parents;
	link_tmp.num_children = num_children;
	link_tmp.name = name;
	link_tmp.num_parent_types = 1;
	link_tmp.num_children_types = 1;
	link_tmp.ordered_by_insert = ordered_by_insert;

	/* Create the actual link node (where the parameters will be stored). */
	_cur_link = _model_graph.AddNode(
		node_specs[LINK_SPEC_NODE], link_tmp);

	/* Link in the parent and child nodes. */
	_model_graph.AddLink(link_specs[LNK_LINK_PARENT_NODE], _cur_link, parent);
	_model_graph.AddLink(link_specs[LNK_LINK_CHILD_NODE], _cur_link, child);

	/* Put the Link under the auspices of the Schema. */
	_model_graph.AddLink(link_specs[LNK_MODEL_LINK], _model_node, _cur_link);
	_model_graph.AddLink(link_specs[LNK_MODEL_ORDER], _model_node, _cur_link);

	return _cur_link;
}

void MetaSchema::Builder::add_to_link(
	Graph::node_iterator& node, bool parent_type)
{
	/* Check for valid data spec */
	if(!_cur_link.i)
		throw std::runtime_error("Attempted to add to nonexistent link!");

	if(parent_type){
		/* Add link and increment parent type count. */
		_model_graph.AddLink(link_specs[LNK_LINK_PARENT_NODE], _cur_link, node);
		++_cur_link.Data<link_spec&>().num_parent_types;
	}
	else{
		/* Add link and increment child type count. */
		_model_graph.AddLink(link_specs[LNK_LINK_CHILD_NODE], _cur_link, node);
		++_cur_link.Data<link_spec&>().num_children_types;
	}
}

Graph::node_iterator MetaSchema::Builder::create_type_enum(
	const std::string& enum_name)
{

	Graph::node_iterator unsig;
	for(unsigned i = 0; i < _type_nodes.size(); ++i){
		const Graph::node_iterator& ni = _type_nodes[i];

		auto type = ni.Data<const type_spec>();

		if("M_UNSIGNED_INT" == type.enum_name){
			unsig = ni;
			break;
		}
	}

	Graph::node_iterator type_enum = make_enum(enum_name, unsig);

	for(unsigned i = 0; i < _type_nodes.size(); ++i){
		const Graph::node_iterator& ni = _type_nodes[i];
		auto type = ni.Data<const type_spec>();

		add_enum_value(type.enum_name, std::to_string(i));
	}

	return type_enum;
}
