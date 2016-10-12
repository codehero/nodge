#include <iostream>
#include <cassert>
#include "BasicGraph.hh"

/* BasicGraph::Ref */

static const MemberSpec Ref_member_specs[] = {
	MemberSpec(0, Type::M_STRING, "node_id", NULL),
};

const MemberSpec& BasicGraph::Ref::node_id_SPEC = Ref_member_specs[0];

void BasicGraph::Ref::Get(const MemberSpec& id, const unsigned*& x) const
	throw(parameter_error)
{
	switch(id.id){
		case 0:
			x = &node_id;
			break;
		default: Data::Get(id, x);
	}
}

const Data::Spec BasicGraph::Ref::s_spec("Ref", Ref_member_specs,
	sizeof(Ref_member_specs)/sizeof(MemberSpec),
	&BasicGraph::Ref::CreateInstance);


/* BasicGraph::lc_iterator */

BasicGraph::lc_iterator::lc_iterator(BasicGraph::NodeList::iterator i, bool start)
	: _nlink(i)
		,_state(start ? lc_iterator::ST_START : lc_iterator::ST_END)
		,_filter(NULL)
{
}

BasicGraph::lc_iterator::~lc_iterator() {
}

const LinkSpec* BasicGraph::Spec(const const_link_iterator& li) const {
	assert(li.owner == this);
	if(!li.i)
		return NULL;

	const auto& a = *reinterpret_cast<const lc_iterator*>(li.i);
	switch(a._state){
		case lc_iterator::ST_CONSTANT:
			return *a._constant_lspec;

		case lc_iterator::ST_MANY:
			return &a._cur_many->lspec;

		default:
			return NULL;
	}
}

/* BasicGraph::n_iterator */

const NodeSpec* BasicGraph::Spec(const const_node_iterator& i) const {
	assert(i.owner == this);
	if(!i.i)
		return NULL;

	auto& ni = (*reinterpret_cast<NodeList::const_iterator*>(i.i));
	return &ni->Spec();
}


/* BasicGraph */

BasicGraph::BasicGraph(const Model& model)
	: _model(model), _first_many(_nodes.end())
{
}

BasicGraph::~BasicGraph() {
	/* FIXME: Delete data in Generic Nodes. */
}


Graph::const_node_iterator BasicGraph::begin(void) const{
	const_node_iterator ret;
	ret.owner = this;
	auto g = const_cast<BasicGraph*>(this);
	auto i = new NodeList::iterator;
	*i = g->_nodes.begin();
	ret.i = i;
	return ret;
}

Graph::node_iterator BasicGraph::begin(void){
	node_iterator ret;
	ret.owner = this;
	auto i = new NodeList::iterator;
	*i = _nodes.begin();
	ret.i = i;
	return ret;
}

Graph::const_node_iterator BasicGraph::end(void) const{
	const_node_iterator ret;
	ret.owner = this;
	auto g = const_cast<BasicGraph*>(this);
	auto i = new NodeList::iterator;
	*i = g->_nodes.end();
	ret.i = i;
	return ret;
}

Graph::const_node_iterator BasicGraph::at(const const_link_iterator& lit, bool parent)
	const 
{
	assert(lit.owner == this);
	if(!lit.i)
		throw parameter_error("Bad link iterator", 0x1);

	const auto& lc = *reinterpret_cast<const lc_iterator*>(lit.i);
	const_node_iterator ni;
	set_iterator(ni, get_node(lc, parent));
	return ni;
}

Graph::node_iterator BasicGraph::at(const const_link_iterator& lit, bool parent)
{
	const_node_iterator ni(
		at(static_cast<const const_link_iterator&>(lit), parent));
	
	node_iterator ret;

	ret.owner = this;
	ret.i = ni.i;

	ni.owner = NULL;
	ni.i = NULL;

	return ret;
}

Graph::const_link_iterator BasicGraph::neighbor_begin(const const_node_iterator& i)
	const 
{
	assert(i.owner == this);
	if(!i.i)
		throw parameter_error("Bad link iterator", 0x1);

	const_link_iterator ret;

	auto& nli = *reinterpret_cast<const NodeList::iterator*>(i.i);
	set_iterator(ret, nli, true);
	return ret;
}

Graph::link_iterator BasicGraph::neighbor_begin(const node_iterator& node){
	const_link_iterator cli(
		neighbor_begin(static_cast<const const_node_iterator&>(node)));
	
	link_iterator ret;

	ret.owner = this;
	ret.i = cli.i;

	cli.owner = NULL;
	cli.i = NULL;

	return ret;
}

Graph::const_link_iterator BasicGraph::neighbor_end(const const_node_iterator& i) const
{
	assert(i.owner == this);
	auto& nli = *reinterpret_cast<const NodeList::iterator*>(i.i);
	const_link_iterator li;
	set_iterator(li, nli, false);
	return li;
}

Graph::const_link_iterator BasicGraph::neighbor_begin(
	const const_node_iterator& i, const LinkSpec& lspec) const
{
	assert(i.owner == this);
	if(!i.i)
		throw parameter_error("Bad link iterator", 0x1);

	const_link_iterator ret;

	auto& nli = *reinterpret_cast<const NodeList::iterator*>(i.i);

	ret.owner = this;
	ret.i = new lc_iterator(nli, true);
	auto& lc = *reinterpret_cast<lc_iterator*>(ret.i);
	lc._filter = &lspec;
	++ret;

	return ret;
}

Graph::link_iterator BasicGraph::neighbor_begin(
	const node_iterator& i, const LinkSpec& lspec)
{
	assert(i.owner == this);
	if(!i.i)
		throw parameter_error("Bad link iterator", 0x1);

	link_iterator ret;

	auto& nli = *reinterpret_cast<const NodeList::iterator*>(i.i);

	ret.owner = this;
	ret.i = new lc_iterator(nli, true);
	auto& lc = *reinterpret_cast<lc_iterator*>(ret.i);
	lc._filter = &lspec;
	++ret;

	return ret;
}

void BasicGraph::advance(const_node_iterator& i) const {
	assert(i.owner == this);
	assert(i.i);

	auto& ni = (*reinterpret_cast<NodeList::iterator*>(i.i));
	if(_nodes.end() == ni)
		throw parameter_error("Node iterator at end", 0x1);
	++ni;
}

void BasicGraph::advance(const_link_iterator& l) const {
	/* Sanity check. */
	assert(l.owner == this);
	if(!l.i)
		return;

	auto& lc = *reinterpret_cast<lc_iterator*>(l.i);
	const NodeSpec& nspec = lc._nlink->Spec();

	switch(lc._state){
		/* Initialization state. */
		case lc_iterator::ST_START:
			lc._cur_index = 0;
			lc._lspec_next = 0;
			lc._constant_lspec = _model.constant_begin(nspec);
			lc._state = lc_iterator::ST_CONSTANT;

			while(lc._constant_lspec != _model.constant_end(nspec)){
				if(!lc._filter || Spec(l) == lc._filter){
					/* Update iteration limit. */
					lc._lspec_next += (*lc._constant_lspec)->ConstantNeighborsMax();

					if(false){
		case lc_iterator::ST_CONSTANT:
						++lc._cur_index;
					}

					/* Iterate until we find defined neighbor. */
					while(lc._cur_index < lc._lspec_next){
						unsigned offset = lc._nlink->_constant_offset + lc._cur_index;
						if(_constant_neighbors[offset].node_idx != NO_CONSTANT_NEIGHBOR)
							return;
						++lc._cur_index;
					}
				}

				/* go onto next lspec */
				++lc._constant_lspec;
			}

			lc._state = lc_iterator::ST_MANY;
			lc._cur_many = lc._nlink->many_begin;

			if(false){
		case lc_iterator::ST_MANY:
				++lc._cur_many;
			}

			if(lc._filter){
				while(lc._cur_many != many_end(lc._nlink) && Spec(l) != lc._filter)
					++lc._cur_many;
			}

			if(lc._cur_many != many_end(lc._nlink))
				return;

			lc._state = lc_iterator::ST_END;
			return;

		default:
			throw std::runtime_error("Cannot advance iterator at end!");
	}
}

bool BasicGraph::compare(const const_node_iterator& a1, const const_node_iterator& a2)
	const 
{
	assert(a1.owner == this);
	assert(a2.owner == this);

	if(!a1.i)
		return a2.i ? false : true;

	auto& n1 = (*reinterpret_cast<const NodeList::iterator*>(a1.i));
	auto& n2 = (*reinterpret_cast<const NodeList::iterator*>(a2.i));

	return n1 == n2;
}


bool BasicGraph::compare(const const_link_iterator& l1, const const_link_iterator& l2)
	const 
{
	assert(l1.owner == this);
	assert(l2.owner == this);

	/* Handle NULL iterators */
	if(!l1.i)
		return l2.i ? false : true;

	lc_iterator& a = *reinterpret_cast<lc_iterator*>(l1.i);
	lc_iterator& b = *reinterpret_cast<lc_iterator*>(l2.i);
	if((a._state != b._state) || (a._nlink != b._nlink))
		return false;

	switch(a._state){
		case lc_iterator::ST_CONSTANT:
			return (a._cur_index == b._cur_index);

		case lc_iterator::ST_MANY:
			return (a._cur_many == b._cur_many);

		default:
			return true;
	}
}

Graph::const_node_iterator BasicGraph::Clone(const const_node_iterator& i) const{
	assert(i.owner == this);
	if(!i.i)
		throw parameter_error("Bad iterator", 0x1);

	const NodeList::iterator* pnli =
		reinterpret_cast<const NodeList::iterator*>(i.i);
	const_node_iterator ni;
	set_iterator(ni, *pnli);
	return ni;
}

Graph::node_iterator BasicGraph::Clone(const const_node_iterator& i) {
	auto g = const_cast<const BasicGraph*>(this);
	const_node_iterator ni = g->Clone(i);
	node_iterator ret;

	ret.owner = this;
	ret.i = ni.i;

	ni.owner = NULL;
	ni.i = NULL;
	return ret;
}

Graph::const_link_iterator BasicGraph::Clone(const const_link_iterator& i) const{
	assert(i.owner == this);
	if(!i.i)
		throw parameter_error("Bad iterator", 0x1);

	const lc_iterator& lci = *reinterpret_cast<lc_iterator*>(i.i);
	const_link_iterator li;
	set_iterator(li, lci);
	return li;
}

Graph::link_iterator BasicGraph::Clone(const const_link_iterator& i) {
	const_link_iterator ni = Clone(i);
	link_iterator ret;

	ret.owner = this;
	ret.i = ni.i;

	ni.owner = NULL;
	ni.i = NULL;
	return ret;
}

const BasicGraph::GenericNode*
	BasicGraph::Get(const const_node_iterator& i) const 
{
	assert(i.owner == this);
	if(!i.i)
		return NULL;

	const NodeList::iterator& nli =
		*reinterpret_cast<const NodeList::iterator*>(i.i);
	if(nli == _nodes.end())
		return NULL;
	return &(*nli);
}

BasicGraph::GenericNode* BasicGraph::Get(const const_node_iterator& i) {
	assert(i.owner == this);
	if(!i.i)
		return NULL;

	const NodeList::iterator& nli =
		*reinterpret_cast<const NodeList::iterator*>(i.i);
	if(nli == _nodes.end())
		return NULL;

	return &const_cast<GenericNode&>(*nli);
}


const BasicGraph::GenericNode*
	BasicGraph::Get(const const_link_iterator& i, bool parent) const 
{
	assert(i.owner == this);
	if(!i.i)
		return NULL;
	const lc_iterator& lc = *reinterpret_cast<const lc_iterator*>(i.i);

	NodeList::iterator nli = get_node(lc, parent);
	if(nli == _nodes.end())
		return NULL;
	return &(*nli);
}

BasicGraph::GenericNode* BasicGraph::Get(const const_link_iterator& i, bool parent)
{
	assert(i.owner == this);
	if(!i.i)
		return NULL;
	const lc_iterator& lc = *reinterpret_cast<const lc_iterator*>(i.i);

	NodeList::iterator nli = get_node(lc, parent);
	if(nli == _nodes.end())
		return NULL;
	return &const_cast<GenericNode&>(*nli);
}

const Data::Spec& BasicGraph::RefSpec(void) const {
	return Ref::s_spec;
}

Data* BasicGraph::NodeRef(const const_node_iterator& i) const{
	assert(i.owner == this);
	if(!i.i)
		return NULL;
	const NodeList::iterator& nli =
		*reinterpret_cast<const NodeList::iterator*>(i.i);

	return new Ref(get_index(*nli));
}

Graph::const_node_iterator BasicGraph::GetNodeFromID(const member_iterator& ref) const{
	unsigned idx;
	ref.Get(idx);
	if(idx >= _map.size())
		throw parameter_error("Bad index", 0x1);
	const_node_iterator ni;
	set_iterator(ni, _map[idx]);
	return ni;
}


#if 0
unsigned BasicGraph::Count(
	const LinkSpec* lspec, const Graph::node_iterator* parent,
	const Graph::node_iterator* child) const 
{
	return 0;
}
#endif

Graph::node_iterator BasicGraph::AddNode(const NodeSpec& nspec, const Data& data)
{
	/* First reserve capacity for the Constant links of the Node. */
	/* Initialize the Node's neighbors to NO_CONSTANT_NEIGHBOR. */
	unsigned required = _model.ConstantNeighborsMax(nspec);
	unsigned offset = _constant_neighbors.size();

	struct ConstantRef cinit;
	cinit.node_idx = NO_CONSTANT_NEIGHBOR;
	cinit.link_data = NULL;
	_constant_neighbors.reserve(required + offset);
	_constant_neighbors.insert(
		_constant_neighbors.end(), required, cinit);

	/* FIXME Look for holes in id space (from removed nodes). */
	unsigned id = _nodes.size();

	/* Add Node to our list. Initialize reference fields in n.  */
	NodeList::iterator i =
		_nodes.insert(_nodes.begin(), GenericNode(nspec, data, id));
	

	/* FIXME insert into proper id spot (when removal implemented) */
	_map.push_back(i);

	/* Initialize other fields in new Node. */
	GenericNode& n = *i;
	n._constant_offset = offset;
	n.many_begin = _many_neighbors.end();

	Graph::node_iterator ni;
	set_iterator(ni, i);
	return ni;
}

void BasicGraph::RemoveNode(node_iterator& nit) {
	/* FIXME do some kind of removal... */
	throw std::runtime_error("Unimplemented!");
}

void BasicGraph::Insert(const link_iterator& oli, const node_iterator& child){
#if 0
	if(!lspec.ordered_by_insert)
		throw parameter_error("Linkspec must be ordered!", 0x2);
#endif

	/* Sanity check. */
	assert(oli.owner == this);
	assert(oli.i);

	assert(child.owner == this);
	assert(child.i);

	auto& lc = *reinterpret_cast<lc_iterator*>(oli.i);
	if(lc_iterator::ST_MANY != lc._state)
		throw parameter_error("Bad link iterator", 0x1);

	/* FIXME Implement parent insert */
	/* FIXME Implement child insert */

	throw std::runtime_error("Unimplemented!");
}

bool BasicGraph::AddLink(const LinkSpec& lspec,
	const Graph::const_node_iterator& parent, const Graph::const_node_iterator& child)
{
	assert(parent.owner == this);
	assert(child.owner == this);
	if(!parent.i)
		return false;
	if(!child.i)
		return false;

	const NodeList::iterator& pnode =
		*reinterpret_cast<const NodeList::iterator*>(parent.i);

	const NodeList::iterator& cnode =
		*reinterpret_cast<const NodeList::iterator*>(child.i);

	/* Check the specs. */
	if(!lspec.IsEndpointType(pnode->Spec(), true) ||
		!lspec.IsEndpointType(cnode->Spec(), false))
		throw spec_mismatch(lspec, pnode->Spec(), cnode->Spec());
	
	/* First check if addition abides by link count restrictions. */
	if(lspec.num_parents != LinkSpec::MANY){
		unsigned child_count = 0;
		unsigned parent_count = parent_child_count(cnode, child_count, &lspec);

		if(parent_count >= lspec.num_parents)
			return false;
	}

	/* Now check children. */
	if(lspec.num_children != LinkSpec::MANY){
		unsigned child_count = 0;
		parent_child_count(pnode, child_count, &lspec);

		if(child_count >= lspec.num_children)
			return false;
	}

	/* Calculate node indices. */
	unsigned pid = get_index(*pnode);
	unsigned cid = get_index(*cnode) | CHILD_FLAG;

	/* Now add the child->parent link reference. */
	if(lspec.num_parents != LinkSpec::MANY){
		unsigned offset =
			cnode->_constant_offset + SumNeighbors(_model, cnode->Spec(), lspec);
		unsigned end = lspec.num_parents + offset;
		add_constant_link(offset, end, pid);
	}
	else{
		add_many_link(lspec, cnode, pid);
	}

	/* Now add the parent->child link reference. */
	if(lspec.num_children != LinkSpec::MANY){
		unsigned offset =
			pnode->_constant_offset + SumNeighbors(_model, pnode->Spec(), lspec);
		offset += lspec.num_parents;
		unsigned end = lspec.num_children + offset;
		add_constant_link(offset, end, cid);
	}
	else{
		add_many_link(lspec, pnode, cid);
	}

	return true;
}

bool BasicGraph::RemoveLink(Graph::const_link_iterator& lit) {
	/* FIXME Implement RemoveLink */
	throw std::runtime_error("Unimplemented!");
}

void BasicGraph::Free(const_link_iterator& i) const {
	assert(i.owner == this);
	if(!i.i)
		return;

	const NodeList::iterator* pnli =
		reinterpret_cast<const NodeList::iterator*>(i.i);
	delete pnli;
	i.i = NULL;
}

void BasicGraph::Free(const_node_iterator& n) const {
	assert(n.owner == this);
	if(n.i){
		const NodeList::iterator* pnli =
			reinterpret_cast<const NodeList::iterator*>(n.i);
		delete pnli;
		n.i = NULL;
	}
}

const BasicGraph::NodeList::iterator
	BasicGraph::get_node(const lc_iterator& lc, bool parent) const
{
	switch(lc._state){
		/*  */
		case lc_iterator::ST_CONSTANT:{
			unsigned offset = lc._nlink->_constant_offset + lc._cur_index;
			unsigned val = _constant_neighbors[offset].node_idx;
			if(parent)
				return (val & CHILD_FLAG) ? lc._nlink : _map[val & ID_MASK];
			else
				return (val & CHILD_FLAG) ? _map[val & ID_MASK] : lc._nlink;
		}

		/*  */
		case lc_iterator::ST_MANY:{
			if(parent){
				return (lc._cur_many->idx & CHILD_FLAG) ?
					lc._nlink : _map[lc._cur_many->idx & ID_MASK];
			}
			else{
				return (lc._cur_many->idx & CHILD_FLAG) ?
					_map[lc._cur_many->idx & ID_MASK] : lc._nlink;
			}
		}

		default:
			throw std::out_of_range("lc_iterator Child() out of range!");
	}
}

/** @brief Count number of parents of particular link type that a node has. */
unsigned BasicGraph::parent_child_count(
	BasicGraph::NodeList::const_iterator node,
	unsigned& child_count, const LinkSpec* lspec) const 
{
	/* Convenience local. */
	const NodeSpec& ns = node->Spec();

	/* Initialize return values. */
	unsigned parent_count = 0;
	child_count = 0;

	/* First count constant neighbors.
	 * If no defined lspec to search over, search all neighors.
	 * If lspec defined, define search limits (offset, end). */
	if(!lspec || (lspec->relationship != LinkSpec::MANY_TO_MANY)){
		/* Define initial offset. */
		unsigned offset = node->_constant_offset;
		if(lspec)
			offset += SumNeighbors(_model, ns, *lspec);

		/* Define end. */
		unsigned end = lspec ?
			lspec->ConstantNeighborsMax() : _model.ConstantNeighborsMax(ns);
		end += offset;

		/* Count parents/children. */
		for(; offset < end; ++offset){
			const unsigned& val = _constant_neighbors[offset].node_idx;
			if(val != NO_CONSTANT_NEIGHBOR){
				if(val & CHILD_FLAG)
					++child_count;
				else
					++parent_count;
			}
		}
	}

	for(ManyList::iterator it = node->many_begin; it != many_end(node); ++it){
		const unsigned& val = it->idx;
		if(!lspec || (it->lspec == *lspec)){
			if(val & CHILD_FLAG)
				++child_count;
			else
				++parent_count;
		}
	}

	return parent_count;
}

unsigned
	BasicGraph::add_constant_link(unsigned offset, unsigned end, unsigned node_id)
{
	while(offset < end){
		if(_constant_neighbors[offset].node_idx == NO_CONSTANT_NEIGHBOR){
			_constant_neighbors[offset].node_idx = node_id;
			break;
		}
		++offset;
	}
	return offset;
}

void BasicGraph::add_many_link(const LinkSpec& ls,
	BasicGraph::NodeList::iterator i, unsigned nid)
{
	/* Will insert this. */
	ManyRef mr = {nid, ls, NULL};

	/* Check for zero size */
	if(i->many_begin == _many_neighbors.end()){
		/* Splice i from its current pos in _nodes to the first many point.
		 * Update _first_many to be i. */
		_nodes.splice(_first_many, _nodes, i);
		_first_many = i;

		/* First insertion will take place at beginning of list. */
		i->many_begin = _many_neighbors.insert(_many_neighbors.begin(), mr);
	}
	else{
		_many_neighbors.insert(many_end(i), mr);
	}
}

void BasicGraph::set_iterator(const_node_iterator& n, NodeList::iterator nli) const
{
	if(n.owner){
		assert(n.owner == this);

		/* Modify current iterator. */
		auto& ni = (*reinterpret_cast<NodeList::const_iterator*>(n.i));
		ni = nli;
	}
	else{
		/* Instantiate new iterator. */
		n.owner = this;
		NodeList::iterator* pnli = new NodeList::iterator;
		*pnli = nli;
		n.i = pnli;
	}
}

void BasicGraph::set_iterator(const_link_iterator& l, NodeList::iterator nli, bool start)
	const
{
	if(l.owner){
		assert(l.owner == this);
		/* TODO improve efficiency. */
		lc_iterator* x = reinterpret_cast<lc_iterator*>(l.i);
		delete x;
		l.i = new lc_iterator(nli, start);
	}
	else{
		l.owner = this;
		l.i = new lc_iterator(nli, start);
	}
	if(start)
		advance(l);
}

void BasicGraph::set_iterator(const_link_iterator& l, const lc_iterator& lci) const{
	if(l.owner){
		assert(l.owner == this);
		/* TODO improve efficiency. */
		lc_iterator* x = reinterpret_cast<lc_iterator*>(l.i);
		delete x;
		l.i = new lc_iterator(lci);
	}
	else{
		l.owner = this;
		l.i = new lc_iterator(lci);
	}
}
