#ifndef __GRAPH_ALGORITHMS_HH__
#define __GRAPH_ALGORITHMS_HH__

#include "Graph.hh"

template<bool is_const_iterator = true>
	class _dfs_explorer {
		public:
			typedef typename const Graph::const_node_iterator const_node_iterator;
			typedef typename const Graph::node_iterator nonconst_node_iterator;
			typedef typename std::conditional<
				is_const_iterator,
				const_node_iterator,
				nonconst_node_iterator>::type node_iterator;

			typedef typename const Graph::const_link_iterator const_link_iterator;
			typedef typename const Graph::link_iterator nonconst_link_iterator;
			typedef typename std::conditional<
				is_const_iterator,
				const_link_iterator,
				nonconst_link_iterator>::type link_iterator;

			/** @brief Iterate
			 *  @param forward_links parent-to-child direction counts toward child's indegree
			 *  @param reverse_links parent-to-child direction counts toward parent's indegree */
			_dfs_explorer(const LinkSpec* forward_links,
				const LinkSpec* reverse_links, const node_iterator& start)
				: _forward_links(forward_links), _reverse_links(reverse_links)
			{
			}

			void begin(){
				_stack.clear();
				_stack.push_back(_start);
			}

			virtual ~_dfs_explorer(){
			}

		protected:
			const LinkSpec* const _forward_links;
			const LinkSpec* const _reverse_links;
			node_iterator _start;

			/** @brief Traversal stack */
			std::list<node_iterator> _stack;
	};

typedef _dfs_explorer<true> const_dfs_explorer:
typedef _dfs_explorer<false> dfs_iterator;

template<bool is_const_iterator = true>
	class _topological_sort_iterator {
		public:
			typedef typename const Graph::const_node_iterator const_node_iterator;
			typedef typename const Graph::node_iterator nonconst_node_iterator;
			typedef typename std::conditional<
				is_const_iterator,
				const_node_iterator,
				nonconst_node_iterator>::type node_iterator;

			typedef typename const Graph::const_link_iterator const_link_iterator;
			typedef typename const Graph::link_iterator nonconst_link_iterator;
			typedef typename std::conditional<
				is_const_iterator,
				const_link_iterator,
				nonconst_link_iterator>::type link_iterator;

			/** @brief Given two sets of link types, arrange nodes in topological sort.
			 *  @param forward_links parent-to-child direction counts toward child's indegree
			 *  @param reverse_links parent-to-child direction counts toward parent's indegree */
			_topological_sort_iterator(link_iterator&& i, const const_link_iterator& end)
				: _i(i), _end(end)
			{
				while(i != end){
				}
			}

			virtual ~_topological_sort_iterator(){
			}

		protected:
			link_iterator _i;
			const_link_iterator _end;
	};

typedef _topological_sort_iterator<true> const_topological_sort_iterator;
typedef _topological_sort_iterator<false> topological_sort_iterator;

#endif
