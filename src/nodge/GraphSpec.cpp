#include "GraphSpec.hh"

NodeSpec::NodeSpec(const Model& p_model, const string& p_name,
	const Data::Spec& p_dspec)
	: model(p_model), name(p_name), dspec(p_dspec)
{
}

bool LinkSpec::IsEndpointType(const NodeSpec& ns, bool parent) const noexcept{
	unsigned idx = parent ? 0 : num_parent_types;
	const unsigned end =
		parent ? num_parent_types : num_parent_types + num_children_types;

	while(idx != end){
		if(&ns == endpoint_types[idx])
			return true;
		++idx;
	}
	return false;
}
