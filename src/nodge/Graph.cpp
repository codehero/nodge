#include "Graph.hh"

Graph::Node* Graph::Get(const const_node_iterator& i) {
	const Graph& g = *this;
	return const_cast<Graph::Node*>(g.Get(i));
}

Graph::Node* Graph::Get(const const_link_iterator& i, bool parent) {
	const Graph& g = *this;
	return const_cast<Graph::Node*>(g.Get(i, parent));
}


const Graph::Node* Graph::Get(const const_node_iterator& i, const LinkSpec& lspec, const_node_iterator& target) const
{
	Graph::const_link_iterator qi(neighbor_begin(i));
	auto end(neighbor_end(i));

	while(qi != end){
		if(*qi.Spec() == lspec){
			target = at(qi, false);
			return *target;
		}
		advance(qi);
	}

	return NULL;
}
