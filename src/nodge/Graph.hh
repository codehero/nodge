#ifndef __GRAPH_HH__
#define __GRAPH_HH__

#include <cassert>
#include "GraphSpec.hh"

/** @brief  */
class Graph {
	public:

		/* Error classes. */

		/** @brief Thrown when attempt to add link between these two types of nodes. */
		class spec_mismatch : public logic_error{
			public:
				/** @brief 
				 *  @param lspec
				 *  @param p
				 *  @param c */
				spec_mismatch(const LinkSpec& lspec, const NodeSpec& p, 
					const NodeSpec& c);
				~spec_mismatch() ;

				/** @brief  */
				const LinkSpec& lspec;

				/** @brief  */
				const NodeSpec& parent;

				/** @brief  */
				const NodeSpec& child;
		};


		/* Component classes. */

		/** @brief  */
		class Node {
			public:
				/** @brief dtor */
				virtual ~Node(void) ;

				/** @brief  */
				virtual const Data& Get(void) const  = 0;

				/** @brief  */
				virtual Data& Get(void)  = 0;

				/** @brief Retrieve NodeSpec of node. */
				virtual const NodeSpec& Spec(void) const  = 0;
		};

		/** @brief  */
		template<bool is_const_iterator = true>
			struct _tm_link_iterator {

				typedef typename std::conditional<is_const_iterator, const Graph*, Graph*>::type GraphPtr;
				typedef typename std::conditional<is_const_iterator, const Node, Node>::type NodeType;
				_tm_link_iterator(void) : owner(NULL), i(NULL)
				{
				}

				~_tm_link_iterator(){
					if(owner && i)
						owner->Free(*this);
				}

				/** @brief Assignment operator. */
				_tm_link_iterator& operator =(_tm_link_iterator li){
					/* Checl for self assignment */
					if(this == &li)
						return *this;

					/* Free existing iterator */
					if(owner && i)
						owner->Free(*this);

					/* Destructive copy. */
					owner = li.owner;
					i = li.i;
					li.i = NULL;
					li.owner = NULL;

					return *this;
				}

				/** @brief Equality operator. */
				bool operator ==(const _tm_link_iterator& a) const{
					if(owner != a.owner)
						return false;
					return owner ? owner->compare(*this, a) : true;
				}

				/** @brief Inequality operator. */
				bool operator !=(const _tm_link_iterator& a) const{
					if(owner == a.owner)
						return owner ? !(owner->compare(*this, a)) : false;
					return true;
				}

				/** @brief Increment operator */
				_tm_link_iterator& operator++(void){
					if(owner && i)
						owner->advance(*this);
					return *this;
				}

				/** @brief Identify LinkSpec. */
				const LinkSpec* Spec(void) const{
					if(owner && i)
						return owner->Spec(*this);
					return NULL;
				}

				operator const _tm_link_iterator<true>& () const{
					return reinterpret_cast<const _tm_link_iterator<true>&>(*this);
				}

				operator _tm_link_iterator<true>& () {
					return reinterpret_cast<_tm_link_iterator<true>&>(*this);
				}

				/** @brief Owning graph. */
				GraphPtr owner;

				/** @brief Implementation specific index/iterator data. */
				void* i;
			};

		typedef _tm_link_iterator<false> link_iterator;
		typedef _tm_link_iterator<true> const_link_iterator;


		template<bool is_const_iterator = true>
			struct _tm_node_iterator {

				typedef typename std::conditional<is_const_iterator, const Graph*, Graph*>::type GraphPtr;
				typedef typename std::conditional<is_const_iterator, const Node, Node>::type NodeType;
				_tm_node_iterator(void) : owner(NULL), i(NULL)
				{
				}

				_tm_node_iterator(const _tm_node_iterator& src) :
					owner(NULL), i(NULL)
				{
					if(src.owner)
						*this = src.owner->Clone(src);
				}

				_tm_node_iterator(_tm_node_iterator&& src) :
					owner(src.owner), i(src.i)
				{
					src.owner = NULL;
					src.i = NULL;
				}

				~_tm_node_iterator(){
					if(owner && i)
						owner->Free(*this);
				}

				template<typename T>
					const T& Data(void) const{
						return static_cast<const T&>(owner->Get(*this)->Get());
					}

				/** @brief  */
				_tm_node_iterator& operator =(_tm_node_iterator src){
					/* Free existing iterator */
					if(owner && i)
						owner->Free(*this);

					/* Swap. */
					owner = src.owner;
					i = src.i;
					src.owner = NULL;
					src.i = NULL;

					return *this;
				}

				/** @brief  */
				bool operator ==(const _tm_node_iterator& a) const{
					if(owner != a.owner)
						return false;
					return owner ? owner->compare(*this, a) : true;
				}

				/** @brief  */
				bool operator !=(const _tm_node_iterator& a) const{
					if(owner == a.owner)
						return owner ? !(owner->compare(*this, a)) : false;
					return true;
				}

				/** @brief  */
				_tm_node_iterator& operator++(void){
					if(owner && i)
						owner->advance(*this);
					return *this;
				}

				/** @brief Identify NodeSpec. */
				const NodeSpec* Spec(void) const{
					if(owner && i)
						return owner->Spec(*this);
					return NULL;
				}

				/** @brief 
				 *  @return NULL if at end; Handle to Node otherwise. */
				NodeType* operator*(void) const{
					return (owner && i) ? owner->Get(*this) : NULL;
				}

				operator const _tm_node_iterator<true>& () const{
					return reinterpret_cast<const _tm_node_iterator<true>&>(*this);
				}

				operator _tm_node_iterator<true>& () {
					return reinterpret_cast<_tm_node_iterator<true>&>(*this);
				}

				/** @brief Owning graph. */
				GraphPtr owner;

				/** @brief Implementation specific index/iterator data. */
				void* i;
			};

		typedef _tm_node_iterator<false> node_iterator;
		typedef _tm_node_iterator<true> const_node_iterator;

		/** @brief  */
		virtual ~Graph();


		/* Iterator properties. */

		/** @brief Identify link type.
		 *  @return */
		virtual const LinkSpec* Spec(const const_link_iterator& i) const  = 0;

		/** @brief Identify node spec.
		 *  @return NULL if invalid iterator; Spec otherwise*/
		virtual const NodeSpec* Spec(const const_node_iterator& i) const  = 0;


		/* Iterator initializers. */

		/** @brief Begin node_iterator over all nodes.
		 *  @return */
		virtual const_node_iterator begin(void) const = 0;
		virtual node_iterator begin(void) = 0;

		/** @brief At end of graph. 
		 *  @return */
		virtual const_node_iterator end(void) const = 0;

		/** @brief Retrieve iterator to node pointed by a link_iterator
		 *  @param i
		 *  @param parent Whether to retrieve parent in link (child if false)
		 *  @return */
		virtual const_node_iterator at(const const_link_iterator& i, bool parent) const = 0;
		virtual node_iterator at(const const_link_iterator& i, bool parent) = 0;

		/** @brief Iterator over all links connected to node.
		 *  @param node
		 *  @return */
		virtual const_link_iterator neighbor_begin(const const_node_iterator& i) const = 0;
		virtual link_iterator neighbor_begin(const node_iterator& i) = 0;

		/** @brief 
		 *  @param i
		 *  @return */
		virtual const_link_iterator neighbor_end(const const_node_iterator& i) const = 0;

		/** @brief Iterator over all links connected to node.
		 *  @param node
		 *  @return */
		virtual const_link_iterator neighbor_begin(const const_node_iterator& i, const LinkSpec& lspec) const = 0;
		virtual link_iterator neighbor_begin(const node_iterator& i, const LinkSpec& lspec) = 0;


		/* Iterator iteration functions. */

		/** @brief Advance iterator to next node. */
		virtual void advance(const_node_iterator& i) const  = 0;

		/** @brief Advance link iterator to next link. */
		virtual void advance(const_link_iterator& i) const  = 0;


		/** @brief Iterator comparison. */
		virtual bool compare(const const_node_iterator& a, const const_node_iterator& b) const
			 = 0;

		/** @brief Iterator comparison. */
		virtual bool compare(const const_link_iterator& a, const const_link_iterator& b) const
			 = 0;


		/* Iterator copying; Copying/Freeing is logically const. */

		/** @brief Create a deepy copy of an iterator.
		 *  @param src 
		 *  @param target 
		 *  @return */
		virtual const_node_iterator Clone(const const_node_iterator& src) const = 0;
		virtual node_iterator Clone(const const_node_iterator& src) = 0;

		/** @brief Create a copy of the link iterator
		 *  @param i
		 *  @return */
		virtual const_link_iterator Clone(const const_link_iterator& i) const = 0;
		virtual link_iterator Clone(const const_link_iterator& i) = 0;

		/** @brief  */
		virtual void Free(const_link_iterator& i) const  = 0;

		/** @brief  */
		virtual void Free(const_node_iterator& i) const  = 0;


		/* Data Access. */

		/** @brief Const Node Access
		 *  @param i Iterator pointing to node
		 *  @return Const Node reference
		 *  @throw NULL if i points to the end. */
		virtual const Node* Get(const const_node_iterator& i) const = 0;

		/** @brief Non-Const Node Access
		 *  @param i Iterator pointing to node
		 *  @return Node reference
		 *  @throw NULL if i points to the end. */
		virtual Node* Get(const const_node_iterator& i);

		/** @brief Const Node Access
		 *  @param i Iterator pointing to node
		 *  @param parent whether to access parent (true) or child (false) of link
		 *  @return Const Node reference
		 *  @throw std::out_of_range if i points to the end. */
		virtual const Node* Get(const const_link_iterator& i, bool parent) const  = 0;

		/** @brief Non-Const Node Access
		 *  @param i Iterator pointing to node
		 *  @param parent whether to access parent (true) or child (false) of link
		 *  @return Node reference
		 *  @throw std::out_of_range if i points to the end. */
		virtual Node* Get(const const_link_iterator& i, bool parent);




		/** @brief Return Reference Data Spec.
		 *  @return Const reference to Data spec. */
		virtual const Data::Spec& RefSpec(void) const  = 0;

		/** @brief Return Data Reference.
		 *  @return Newly created Data value. User manages returned pointer.
		 *  TODO: allocator parameter. */
		virtual Data* NodeRef(const const_node_iterator& node) const = 0;

		/** @brief Retrieve const reference to node based on the Data Reference.
		 *  @param id_ref Pointer to data member containing the node id.
		 *  @return NULL if ref does not match any data. */
		virtual const_node_iterator GetNodeFromID(const member_iterator& id_ref) const = 0;




#if 0
		/** @brief Check for the existence of a link.
		 *  @param lspec
		 *  @param parent
		 *  @param child
		 *  @return */
		virtual unsigned Count(const LinkSpec* lspec, const node_iterator* parent,
			const node_iterator* child) const  = 0;
#endif

		/** @brief  */
		virtual const Node* Get(const const_node_iterator& i, const LinkSpec& lspec, const_node_iterator& target) const;

		/* Graph modification. */

		/** @brief Add a node to the graph.
		 *  @param nspec
		 *  @param data Data to add. Graph class allocates memory for the copy.
		 *  @return */
		virtual node_iterator AddNode(const NodeSpec& nspec, const Data& data) = 0;

		/** @brief Remove node from the graph. Removes link references as well.
		 *  @param nit */
		virtual void RemoveNode(node_iterator& nit)  = 0;

		/** @brief Insert a link before the iterator
		 * only applicable for ordered MANY links
		 *  @param oli a link iterator with an ordered spec.
		 *  @param child child node; link is assumed to be 1 to MANY
		 *  @return */
		virtual void Insert(const link_iterator& oli, const node_iterator& child) = 0;

		/** @brief Add link to the graph. Ordered link specs are appended to the end of their list.
		 *  @param lspec
		 *  @param parent parent node; should not equal child
		 *  @param child childe node; should not equal parent
		 *  @return */
		virtual bool AddLink(const LinkSpec& lspec, const const_node_iterator& parent,
			const const_node_iterator& child) = 0;

		/** @brief Remove Link pointed to by link iterator
		 * 	@param lit Erase link lit. lit is updated to an invalid iterator.
		 *  @return Whether remove was successful.*/
		virtual bool RemoveLink(const_link_iterator& lit)  = 0;
};

/* Graph::spec_mismatch */
inline Graph::spec_mismatch::spec_mismatch(const LinkSpec& l,
	const NodeSpec& p, const NodeSpec& c)
	: logic_error("spec_mismatch"), lspec(l), parent(p), child(c)
{
}

inline Graph::spec_mismatch::~spec_mismatch() {
}


/* Graph::Node  */

inline Graph::Node::~Node(void) {
}

/* Graph */

inline Graph::~Graph() {
}


#endif

