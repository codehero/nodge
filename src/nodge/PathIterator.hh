#ifndef __PATH_ITERATOR_HH__
#define __PATH_ITERATOR_HH__

#include <Graph/Graph.hh>

class PathIterator {
	public:
		PathIterator(Graph& g, const Graph::node_iterator& start);

		/** @brief  */
		virtual ~PathIterator();

		/** @brief Go onto next edge.
		 *  @return Change in depth. */
		int next(void) throw();

		/** @brief Length of current path.
		 *  @return If 0, Iterator has reached the end. */
		unsigned size(void) const throw();

		/** @brief Retrieve node starting from end of path. */
		const Graph::Node& Node(unsigned rev_idx) const throw(std::exception);

		/** @brief Retrieve linkspec starting from end of path. */
		const LinkSpec& Link(unsigned rev_idx) const throw(std::exception);

		/** @brief  */
		void at(unsigned rev_idx, Graph::node_iterator*& ret_ni) throw(std::exception);
};

/** @brief Base Interface class for descendants. */
class PathMatcher {
	public:
		PathMatcher(void);

		virtual ~PathMatcher();

		virtual bool Match(const PathIterator& pi) = 0;
};

class LSPEC : public PathMatcher{
	public:
		LSPEC(const LinkSpec& lspec);
	
		bool Match(const PathIterator& pi);
	private:
		const LinkSpec& _lspec;
};

class NSPEC : public PathMatcher{
	public:
		NSPEC(const NodeSpec& nspec);
	
		bool Match(const PathIterator& pi);
	private:
		const LinkSpec& _nspec;
};

class ANY : public PathMatcher{
	public:
		ANY(void);

		bool Match(const PathIterator& pi);
};

/* Inlines. */

inline LSPEC::Match(const PathIterator& pi){
	return _lspec == pi.Link(0);
}

inline NSPEC::Match(const PathIterator& pi){
	return _nspec == pi.Node(0).Spec();
}

inline ANY::Match(const PathIterator& pi){
	return true;
}

#endif
