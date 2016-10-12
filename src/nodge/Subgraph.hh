#ifndef __SUBGRAPH_HH__
#define __SUBGRAPH_HH__

template<bool is_const_subgraph = true>
	class _subgraph : public Graph {
		public:

			typedef typename std::conditional<is_const_iterator, const Graph, Graph>::type GraphType;
			typedef typename std::conditional<is_const_iterator, const Node, Node>::type NodeType;
			_subgraph(GraphType& parent) : _parent(parent)
			{
			}

			virtual ~_subgraph(void){
			}

			virtual const LinkSpec* Spec(const const_link_iterator& i) const  = 0;
			virtual const NodeSpec* Spec(const const_node_iterator& i) const  = 0;


			/* Iterator initializers. */

			virtual const_node_iterator begin(void) const = 0;
			virtual node_iterator begin(void) = 0;
			virtual const_node_iterator end(void) const = 0;

			virtual const_node_iterator at(const const_link_iterator& i, bool parent) const = 0;
			virtual node_iterator at(const const_link_iterator& i, bool parent) = 0;

			virtual const_link_iterator neighbor_begin(const const_node_iterator& i) const = 0;
			virtual link_iterator neighbor_begin(const node_iterator& i) = 0;
			virtual const_link_iterator neighbor_end(const const_node_iterator& i) const = 0;

			virtual const_link_iterator neighbor_begin(const const_node_iterator& i, const LinkSpec& lspec) const = 0;
			virtual link_iterator neighbor_begin(const node_iterator& i, const LinkSpec& lspec) = 0;

			/* Iterator iteration functions. */

			virtual void advance(const_node_iterator& i) const  = 0;
			virtual void advance(const_link_iterator& i) const  = 0;
			virtual bool compare(const const_node_iterator& a, const const_node_iterator& b) const
				 = 0;
			virtual bool compare(const const_link_iterator& a, const const_link_iterator& b) const
				 = 0;


			/* Iterator copying; Copying/Freeing is logically const. */

			const_node_iterator Clone(const const_node_iterator& src) const {
				return _parent.Clone(src);
			}

			node_iterator Clone(const const_node_iterator& src){
				return _parent.Clone(src);
			}

			const_link_iterator Clone(const const_link_iterator& i) const;
			link_iterator Clone(const const_link_iterator& i);
			void Free(const_link_iterator& i) const;
			void Free(const_node_iterator& i) const;


			/* Data Access. */

			virtual const Node* Get(const const_node_iterator& i) const = 0;
			virtual Node* Get(const const_node_iterator& i);
			virtual const Node* Get(const const_link_iterator& i, bool parent) const  = 0;
			virtual Node* Get(const const_link_iterator& i, bool parent);


			virtual const Data::Spec& RefSpec(void) const  = 0;
			virtual Data* NodeRef(const const_node_iterator& node) const = 0;
			virtual const_node_iterator GetNodeFromID(const member_iterator& id_ref) const = 0;

			/** @brief  */
			virtual const Node* Get(const const_node_iterator& i, const LinkSpec& lspec, const_node_iterator& target) const;
			virtual node_iterator AddNode(const NodeSpec& nspec, const Data& data) = 0;
			virtual void RemoveNode(node_iterator& nit)  = 0;
			virtual void Insert(const link_iterator& oli, const node_iterator& child) = 0;
			virtual bool AddLink(const LinkSpec& lspec, const const_node_iterator& parent,
				const const_node_iterator& child) = 0;
			virtual bool RemoveLink(const_link_iterator& lit)  = 0;

		protected:
			GraphType& _parent;
	};

#endif
