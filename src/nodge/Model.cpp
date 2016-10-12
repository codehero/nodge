#include "Model.hh"

Model::ModelList Model::s_models;

static const Data::Spec common_data_specs[1] = {
	Data::Spec("empty_data_spec", NULL, 0, NULL)
};

static const NodeSpec common_node_specs[1] = {
	NodeSpec(Model::s_common,
			"wildcard_node",
			common_data_specs[0]
			)
};
const NodeSpec& Model::s_wildcard_node = common_node_specs[0];

const Model Model::s_common("Common",
		common_node_specs, 1,
		NULL, 0,
		common_data_specs, 1,
		NULL, 0,
		NULL, 0,
		NULL, 0
		);

Model::Model(const std::string& id_str,
	const NodeSpecList& nodes, unsigned node_count,
	const LinkSpecList& links, unsigned link_count,
	const DataSpecList& specs, unsigned spec_count,
	const NodeSpec* const * external_nodes, unsigned external_node_count,
	const LinkSpec* const * link_index, unsigned link_index_count,
	const NodeLinks* const node_index, unsigned node_index_count)
	: _id_str(id_str),
		_nodespecs(nodes), _nodespec_count(node_count),
		_linkspecs(links), _linkspec_count(link_count),
		_dataspecs(specs), _dataspec_count(spec_count),
		_external_nodes(external_nodes), _external_node_count(external_node_count),
		_link_index(link_index), _link_index_count(link_index_count),
		_node_index(node_index), _node_index_count(node_index_count),
		_total_nodes(node_count + external_node_count)
{
	s_models.push_back(this);

	/* First determine how many:
	 * -external Models referenced.
	 * -unique pairs of Models. */
}

Model::~Model(){
}

bool Model::References(const Model& other) const throw(){
	/* FIXME */
	return true;
}

const Model* Model::GetModel(const string& name) throw(){
	for(ModelList::const_iterator i = s_models.begin(); i != s_models.end(); ++i){
		if(name == (*i)->_id_str)
			return (*i);
	}
	return NULL;
}

const Data::Spec* Model::GetDataSpec(const string& name) const throw(){
	for(unsigned i = 0; i < _dataspec_count; ++i){
		if(_dataspecs[i].name == name)
			return _dataspecs + i;
	}
	return NULL;
}

const LinkSpec* Model::GetLinkSpec(const string& name) const throw(){
	for(unsigned i = 0; i < _linkspec_count; ++i){
		if(_linkspecs[i].name == name)
			return _linkspecs + i;
	}
	return NULL;
}

const NodeSpec* Model::GetNodeSpec(const string& name) const throw(){
	for(unsigned i = 0; i < _nodespec_count; ++i){
		if(_nodespecs[i].name == name)
			return _nodespecs + i;
	}
	return NULL;
}

const LinkSpec* const* Model::model_begin(const Model* m1, const Model* m2)
	const throw()
{
	/* FIXME */
	return NULL;
}

const LinkSpec* const* Model::model_end(const Model* m1, const Model* m2) const
	throw()
{
	/* FIXME */
	return NULL;
}

unsigned SumNeighbors(const Model& m, const NodeSpec& nspec, const LinkSpec& ls)
	throw()
{
	unsigned count = 0;
	for(Model::lspec_iterator i = m.constant_begin(nspec); !(ls == **i); ++i){
		count += (*i)->num_parents;
		count += (*i)->num_children;
	}
	return count;
}

int Model::get_offset(const NodeSpec& nspec) const throw(){
	int ret = ModelID(nspec);
	if(ret < 0){
		for(unsigned i = 0; i < _external_node_count; ++i){
			if(nspec == *_external_nodes[i]){
				ret = _nodespec_count + i;
				break;
			}
		}
	}
	return ret;
}
