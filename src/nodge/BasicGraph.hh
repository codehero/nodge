#ifndef __EXAMPLE_GRAPH_HH__
#define __EXAMPLE_GRAPH_HH__

#include <vector>
#include <list>
#include "Model.hh"
#include "Graph.hh"

class BasicGraph : public Graph{
	protected:
		class GenericNode;
	public:
		BasicGraph(const Model& model);
		~BasicGraph();

		/* Overrides. */
		const LinkSpec* Spec(const const_link_iterator& i) const;
		const NodeSpec* Spec(const const_node_iterator& i) const;

		const_node_iterator begin(void) const;
		node_iterator begin(void);
		const_node_iterator end(void) const;

		const_node_iterator at(const const_link_iterator& lit, bool parent) const;
		node_iterator at(const const_link_iterator& lit, bool parent);

		const_link_iterator neighbor_begin(const const_node_iterator& node) const;
		link_iterator neighbor_begin(const node_iterator& node);

		const_link_iterator neighbor_end(const const_node_iterator& node) const;

		const_link_iterator neighbor_begin(const const_node_iterator& i, const LinkSpec& lspec) const;
		link_iterator neighbor_begin(const node_iterator& i, const LinkSpec& lspec);

		void advance(const_node_iterator& i) const;
		void advance(const_link_iterator& i) const;
		bool compare(const const_node_iterator& a, const const_node_iterator& b) const;
		bool compare(const const_link_iterator& a, const const_link_iterator& b) const;

		const_node_iterator Clone(const const_node_iterator& i) const;
		node_iterator Clone(const const_node_iterator& i);

		const_link_iterator Clone(const const_link_iterator& i) const;
		link_iterator Clone(const const_link_iterator& i);

		void Free(const_link_iterator& i) const;
		void Free(const_node_iterator& i) const;

		const GenericNode* Get(const const_node_iterator& i) const;
		GenericNode* Get(const const_node_iterator& i);

		const GenericNode* Get(const const_link_iterator& i, bool parent) const;
		GenericNode* Get(const const_link_iterator& i, bool parent);

		const Data::Spec& RefSpec(void) const;
		Data* NodeRef(const const_node_iterator& node) const;
		const_node_iterator GetNodeFromID(const member_iterator& ref) const;

#if 0
		unsigned Count(const LinkSpec* lspec, const node_iterator* parent,
			const node_iterator* child) const;
#endif

		node_iterator AddNode(const NodeSpec& nspec, const Data& data);
		void RemoveNode(node_iterator& nit);
		void Insert(const link_iterator& oli, const node_iterator& child);
		bool AddLink(const LinkSpec& lspec, const const_node_iterator& parent,
			const const_node_iterator& child);
		bool RemoveLink(const_link_iterator& lit);

	protected:

		static const unsigned NO_CONSTANT_NEIGHBOR = 0xFFFFFFFF;
		static const unsigned MAX_NODE_ID = 0x7FFFFFFE;
		static const unsigned CHILD_FLAG = 0x80000000;
		static const unsigned ID_MASK = 0x7FFFFFFF;

		/** @brief  */
		class Ref : public Data {
			public:
				Ref(void);
				Ref(unsigned nid);
				~Ref();

				/* Overrides. */
				void Get(const MemberSpec& id, const unsigned*& x) const
					throw(parameter_error);

				const Spec& GetSpec(void) const noexcept;

				Ref* Clone(void) const;

				static Data* CreateInstance(void){
					return new Ref;
				}

				/** @brief  */
				unsigned node_id;

				static const MemberSpec& node_id_SPEC;
				static const Spec s_spec;
		};

		/** @brief  */
		struct ManyRef{
			/** @brief Identifies where in BasicGraph::_map[] endpoint node located. */
			unsigned idx;

			/** @brief  */
			const LinkSpec& lspec;

			/** @brief If lspec supports link data, it is defined here. */
			Data* link_data;
		};

		struct ConstantRef {
			unsigned node_idx;
			Data* link_data;
		};

		struct OrderedRefHead {
			std::list<ConstantRef> links;
			const LinkSpec* spec;
		};

		/** @brief  */
		typedef std::list<ManyRef> ManyList;

		/** @brief  */
		class GenericNode : public Node {
			public:
				/** @brief 
				 *  @param nspec
				 *  @param data */
				GenericNode(const NodeSpec& nspec, const Data& data, unsigned id);

				/** @brief  */
				~GenericNode(void);

				/* Overrides. */

				const Data& Get(void) const;
				Data& Get(void);
				const NodeSpec& Spec(void) const;

			protected:

				/** @brief  */
				const NodeSpec& _nspec;

				/** @brief  */
				Data* _data;

				/** @brief  */
				const unsigned _id;

				/** @brief Index at BasicGraph::_constant_neighbors[] where neighbor
				 * references begin.*/
				unsigned _constant_offset;


				/** @brief Points to beginning of many parent list. */
				mutable ManyList::iterator many_begin;

				friend class BasicGraph;
				friend class lc_iterator;
		};

		/** @brief  */
		typedef std::list<GenericNode> NodeList;

		/** @brief iterator helper class. */
		class lc_iterator{
			public:
				/** @brief ctor
				 *  @param parent Parent container graph
				 *  @param i Node over whose links to iterate
				 *  @param start whether this is a begin() or end() iterator*/
				lc_iterator(NodeList::iterator i, bool start);

				/** @brief  */
				~lc_iterator();

				/** @brief Fixed endpoint of the link. */
				const NodeList::iterator _nlink;

				/** @brief  */
				unsigned _cur_index;

				/** @brief  */
				ManyList::iterator _cur_many;

				/** @brief  */
				unsigned _state;

				/** @brief  */
				Model::lspec_iterator _constant_lspec;

				/** @brief  */
				unsigned _lspec_next;

				/** @brief If specified, */
				const LinkSpec* _filter;

			protected:
				enum {
					ST_START
					,ST_CONSTANT
					,ST_MANY
					,ST_END
				};

				friend class BasicGraph;
		};

		/** @brief 
		 *  @param lc
		 *  @param parent
		 *  @return */
		const NodeList::iterator
			get_node(const lc_iterator& lc, bool parent) const;

		ManyList::const_iterator many_end(const NodeList::const_iterator& i) const;

		ManyList::iterator many_end(const NodeList::iterator& i);

		/** @brief  */
		unsigned get_index(const GenericNode& node) const throw(std::out_of_range);

		/** @brief Count parents of particular link type that a node has. */
		unsigned parent_child_count(NodeList::const_iterator i,
			unsigned& child_count, const LinkSpec* lspec = NULL) const;

		/** @brief 
		 *  @param val
		 *  @return */
		static bool is_child_link(unsigned val);

		/** @brief
		 *  @param offset 
		 *  @param end
		 *  @param node_id 
		 *  @return */
		unsigned add_constant_link(unsigned offset, unsigned end, unsigned node_id);

		/** @brief
		 *  @param ls
		 *  @param i
		 *  @param nid */
		void add_many_link(const LinkSpec& ls, NodeList::iterator i, unsigned nid);

		void set_iterator(const_node_iterator& n, NodeList::iterator nli) const;

		void set_iterator(const_link_iterator& l, NodeList::iterator nli, bool start) const;

		void set_iterator(const_link_iterator& l, const lc_iterator& lci) const;

		/** @brief Link/Node model. */
		const Model& _model;

		/** @brief Master list of nodes in the system.
		 *  List is divided into two sections:
		 *  1) nodes with no many links
		 *  2) nodes with at least 1 many link */
		NodeList _nodes;

		/** @brief Indicates location of division in _nodes
		 *  if at begin(), all nodes contain at > 1 many_link
		 *  if at end(), all nodes contain 0 many_links */
		NodeList::iterator _first_many;

		/** @brief Master list of all CONSTANT neighbors.
		 * 	_constant_offset in GenericNode indexes into this array. */
		std::vector<ConstantRef> _constant_neighbors;

		/** @brief  */
		std::vector<NodeList::iterator> _map;

		/** @brief List of all nodes that are MANY neighbors. */
		ManyList _many_neighbors;
};

/* BasicGraph::Ref */

inline BasicGraph::Ref::Ref(void){
}

inline BasicGraph::Ref::Ref(unsigned nid) : node_id(nid)
{
}

inline BasicGraph::Ref::~Ref() {
}

inline const Data::Spec& BasicGraph::Ref::GetSpec(void) const noexcept{
	return s_spec;
}

inline BasicGraph::Ref* BasicGraph::Ref::Clone(void) const{
	return new Ref(node_id);
}


/* BasicGraph */

inline BasicGraph::ManyList::const_iterator
	BasicGraph::many_end(const NodeList::const_iterator& i) const
{
	if(i->many_begin == _many_neighbors.end())
		return _many_neighbors.end();

	NodeList::const_iterator k = i;
	++k;
	return (k == _nodes.end()) ?
		_many_neighbors.end() : k->many_begin;
}

inline BasicGraph::ManyList::iterator
	BasicGraph::many_end(const NodeList::iterator& i)
{
	if(i->many_begin == _many_neighbors.end())
		return _many_neighbors.end();

	NodeList::const_iterator k = i;
	++k;
	return (k == _nodes.end()) ?
		_many_neighbors.end() : k->many_begin;
}


inline unsigned BasicGraph::get_index(const BasicGraph::GenericNode& node) const
	throw(std::out_of_range)
{
	return node._id;
}

inline bool BasicGraph::is_child_link(unsigned val) {
	return val & CHILD_FLAG;
}

/* BasicGraph::GenericNode */

inline BasicGraph::GenericNode::GenericNode(const NodeSpec& nspec,
	const Data& data, unsigned id)
	: _nspec(nspec), _data(data.Clone()), _id(id)
{
}

inline BasicGraph::GenericNode::~GenericNode(void) {
}

inline const Data& BasicGraph::GenericNode::Get(void) const {
	return *_data;
}

inline Data& BasicGraph::GenericNode::Get(void) {
	return *_data;
}

inline const NodeSpec& BasicGraph::GenericNode::Spec(void) const {
	return _nspec;
}

#endif
