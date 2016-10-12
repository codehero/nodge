#ifndef __MODEL_HH__
#define __MODEL_HH__

#include <vector>

#include "GraphSpec.hh"

/** @brief Model serves several roles:
 * 1) Model groups together related Data classes
 * 2) Model specifies a one to one mapping between NodeSpecs and DataSpecs
 * 3) Model specifies LinkSpecs between NodeSpecs
 *
 * Notes:
 * NodeSpecs and LinkSpecs may be owned by only one Model, though other
 * Models, NodeSpecs, and LinkSpecs may reference them.
 *
 * Data::Specs do not have this restriction; they do not have to be associated
 * with Models.
 * */
class Model {
	public:
		/** @brief Type for retrieving handle of static schema instance;
		 * 	used for loading Models dynamically from shared libs. */
		typedef Model& get_handle_t();

		/** @brief  */
		typedef std::vector<const Model*> ModelList;

		/** @brief  */
		typedef const LinkSpec * const LinkSpecList;

		/** @brief  */
		typedef const NodeSpec * const NodeSpecList;

		/** @brief  */
		typedef const Data::Spec* const DataSpecList;

		/** @brief  */
		typedef const LinkSpec* const * lspec_iterator;

		/** @brief Return value for GetID() functions. */
		static const unsigned INVALID_SPEC = 0;


		/** @brief 
		 *  @param
		 *  @return */
		bool operator ==(const Model& m) const throw();


		/* NodeSpec Properties. */

		/** @brief Retrieve Model ID of NodeSpec in context of this Model.
		 *  @param nspec Reference to nspec
		 *  @return 0 < if nspec does not exist in Model; id otherwise. */
		int ModelID(const NodeSpec& nspec) const throw();

		/** @brief Maximum possible number of Constant Neighbors */
		unsigned ConstantNeighborsMax(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator parent_begin(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator parent_end(const NodeSpec& nspec) const throw();

		/** @brief  */
		unsigned parent_count(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator child_begin(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator child_end(const NodeSpec& nspec) const throw();

		/** @brief  */
		unsigned child_count(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator many_begin(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator many_end(const NodeSpec& nspec) const throw();

		/** @brief  */
		unsigned many_count(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator constant_begin(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator constant_end(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator ordered_begin(const NodeSpec& nspec) const throw();

		/** @brief  */
		lspec_iterator ordered_end(const NodeSpec& nspec) const throw();

		/** @brief  */
		unsigned ordered_count(const NodeSpec& nspec) const throw();

		/** @brief  */
		unsigned constant_count(const NodeSpec& nspec) const throw();


		/** @brief 
		 *  @param id */
		const NodeSpec* GetNodeSpec(unsigned id) const throw();

		/** @brief 
		 *  @param name */
		const NodeSpec* GetNodeSpec(const string& name) const throw();


		/* LinkSpec Properties. */

		/** @brief Retrieve Model ID of LinkSpec in context of this Model.
		 *  @param lspec Reference to lspec
		 *  @return 0 < if lspec does not exist in Model; id otherwise. */
		int ModelID(const LinkSpec& lspec) const throw();

		/** @brief 
		 *  @param id */
		const LinkSpec* GetLinkSpec(unsigned id) const throw();

		/** @brief 
		 *  @param name */
		const LinkSpec* GetLinkSpec(const string& name) const throw();


		/* Data::Spec Properties. */

		/** @brief Retrieve Model ID of Data::Spec in context of this Model.
		 *  @param dspec Reference to dspec
		 *  @return 0 if dspec does not exist in Model; id otherwise. */
		int ModelID(const Data::Spec& dspec) const throw();

		/** @brief 
		 *  @param id */
		const Data::Spec* GetDataSpec(unsigned id) const throw();

		/** @brief 
		 *  @param name */
		const Data::Spec* GetDataSpec(const string& name) const throw();


		/** @brief 
		 *  @return */
		const LinkSpec* begin(void) const throw();

		/** @brief 
		 *  @return */
		const LinkSpec* end(void) const throw();

		/** @brief Retrieve iterator beginning to linkspecs between two models.
		 *  @param m1 if NULL, then this Model
		 *  @param m2 if NULL, then this Model
		 *  @return end if no links between m1, m2; begin otherwise. */
		const LinkSpec* const* model_begin(const Model* m1 = NULL,
			const Model* m2 = NULL) const throw();

		/** @brief Retrieve iterator beginning to linkspecs between two models.
		 *  @param m1 if NULL, then this Model
		 *  @param m2 if NULL, then this Model
		 *  @return end() if no links between m1, m2; begin otherwise. */
		const LinkSpec* const* model_end(const Model* m1 = NULL,
			const Model* m2 = NULL) const throw();


		/** @brief 
		 *  @param
		 *  @return */
		bool OwnsNodeSpec(const NodeSpec& nspec) const throw();

		/** @brief 
		 *  @param
		 *  @return */
		bool OwnsLinkSpec(const LinkSpec& lspec) const throw();


		/** @brief Check if this Model refers to other.
		 *  @param other Model we are looking for.
		 *  @return true if this refers to other or (other == this) */
		bool References(const Model& other) const throw();

		/** @brief  */
		unsigned NumLinks(void) const throw();

		/** @brief  */
		unsigned NumNodes(void) const throw();

		/** @brief  */
		const string& GetID(void) const throw();


		/** @brief
		 * 	@param name */
		static const Model* GetModel(const string& name) throw();


		/** @brief Suffix to add 
		 *  @param model_name */
		static const std::string get_handle_name(const std::string& model_name){
			return model_name + "_get_handle";
		}

		/** @brief Index data generated for each node (including external nodes) */
		struct NodeLinks{
			/** @brief offset into Master LinkSpec pointer list. */
			unsigned offset;

			/** @brief Number of links in which node is a child. */
			unsigned parent_count;

			/** @brief Number of links in which node is a parent. */
			unsigned child_count;

			/** @brief Number of links involving MANY neighbors. */
			unsigned many_count;

			/** @brief Number of links involving a constant number of neighbors. */
			unsigned constant_count;

			/** @brief Number of links involving ordered MANY neighbors. */
			unsigned ordered_count;

			/** @brief Total count of links involving constant neighbors. */
			unsigned max_const_neighors;
		};

		/** @brief ctor
		 * FIXME
		 * Need to add external Model stuff
		 *  @param id_str */
		Model(const std::string& id_str,
			const NodeSpecList& nodes, unsigned node_count,
			const LinkSpecList& links, unsigned link_count,
			const DataSpecList& specs, unsigned spec_count,
			const NodeSpec* const * external_nodes, unsigned external_node_count,
			const LinkSpec* const * link_index, unsigned link_index_count,
			const NodeLinks* const node_index, unsigned node_index_count);

		/** @brief  */
		~Model();

		static const NodeSpec& s_wildcard_node;

		static const Model s_common;
	protected:

		/** @brief  */
		int get_offset(const NodeSpec& nspec) const throw();

		/** @brief  */
		static ModelList s_models;

		/** @brief  */
		const std::string _id_str;

		/** @brief  */
		NodeSpecList _nodespecs;

		/** @brief  */
		unsigned _nodespec_count;

		/** @brief  */
		LinkSpecList _linkspecs;

		/** @brief  */
		unsigned _linkspec_count;

		/** @brief  */
		DataSpecList _dataspecs;

		/** @brief  */
		unsigned _dataspec_count;

		/** @brief List of external nodes. */
		const NodeSpec* const * _external_nodes;

		/** @brief Total number of external nodes. */
		unsigned _external_node_count;

		/** @brief Master index list of all linkspecs. */
		const LinkSpec* const * const _link_index;

		/** @brief Master index list of all linkspecs. */
		unsigned _link_index_count;

		/** @brief Master list of node data. */
		const NodeLinks* const _node_index;

		/** @brief Master index list of all linkspecs. */
		unsigned _node_index_count;

		/** @brief Total of both internal and external nodes. */
		unsigned _total_nodes;
};

/* Auxiliary functions. */

unsigned SumNeighbors(const Model& m, const NodeSpec& nspec, const LinkSpec& ls)
	throw();


/* Model */

inline bool Model::operator ==(const Model& m) const throw(){
	return this == &m;
}

inline unsigned Model::ConstantNeighborsMax(const NodeSpec& nspec) const
	throw()
{
	return _node_index[get_offset(nspec)].max_const_neighors;
}

inline Model::lspec_iterator
	Model::parent_begin(const NodeSpec& nspec) const throw()
{
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset;
}

inline Model::lspec_iterator
	Model::parent_end(const NodeSpec& nspec) const throw()
{
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count;
}

inline unsigned Model::parent_count(const NodeSpec& nspec) const throw(){
	return _node_index[get_offset(nspec)].parent_count;
}

inline Model::lspec_iterator
	Model::child_begin(const NodeSpec& nspec) const throw()
{
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count;
}

inline Model::lspec_iterator Model::child_end(const NodeSpec& nspec) const throw(){
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count + nl.child_count;
}

inline unsigned Model::child_count(const NodeSpec& nspec) const throw(){
	return _node_index[get_offset(nspec)].child_count;
}

inline Model::lspec_iterator
	Model::many_begin(const NodeSpec& nspec) const throw()
{
	const NodeLinks& nl =_node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count + nl.child_count;
}

inline Model::lspec_iterator
	Model::many_end(const NodeSpec& nspec) const throw()
{
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count + nl.child_count
		+ nl.many_count - nl.ordered_count;
}

inline unsigned Model::many_count(const NodeSpec& nspec) const throw(){
	return _node_index[get_offset(nspec)].many_count;
}

inline Model::lspec_iterator
	Model::constant_begin(const NodeSpec& nspec) const throw()
{
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count + nl.child_count
		+ nl.many_count;
}

inline Model::lspec_iterator
	Model::constant_end(const NodeSpec& nspec) const throw()
{
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count + nl.child_count
		+ nl.many_count + nl.constant_count;
}

inline Model::lspec_iterator Model::ordered_begin(const NodeSpec& nspec) const throw(){
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count + nl.child_count
		+ nl.many_count - nl.ordered_count;
}

inline Model::lspec_iterator Model::ordered_end(const NodeSpec& nspec) const throw(){
	const NodeLinks& nl = _node_index[get_offset(nspec)]; 
	return _link_index + nl.offset + nl.parent_count + nl.child_count
		+ nl.many_count + nl.constant_count;
}

inline unsigned Model::ordered_count(const NodeSpec& nspec) const throw(){
	return _node_index[get_offset(nspec)].ordered_count;
}

inline unsigned Model::constant_count(const NodeSpec& nspec) const throw(){
	return _node_index[get_offset(nspec)].constant_count;
}


inline const LinkSpec* Model::begin(void) const throw(){
	return _linkspecs;
}

inline const LinkSpec* Model::end(void) const throw(){
	return _linkspecs + _linkspec_count;
}


inline bool Model::OwnsNodeSpec(const NodeSpec& nspec) const throw(){
	return &nspec.model == this;
}

inline bool Model::OwnsLinkSpec(const LinkSpec& lspec) const throw(){
	return &lspec.model == this;
}


inline unsigned Model::NumLinks(void) const throw(){
	return _linkspec_count;
}

inline unsigned Model::NumNodes(void) const throw(){
	return _nodespec_count;
}


inline int Model::ModelID(const LinkSpec& lspec) const throw(){
	int diff = &lspec - _linkspecs;
	if(diff < 0)
		return -1;
	return (static_cast<unsigned>(diff) >= _linkspec_count) ? -1 : diff;
}

inline int Model::ModelID(const NodeSpec& nspec) const throw(){
	int diff = &nspec - _nodespecs;
	if(diff < 0)
		return -1;
	return (static_cast<unsigned>(diff) >= _nodespec_count) ? -1 : diff;
}

inline int Model::ModelID(const Data::Spec& dspec) const throw(){
	int diff = &dspec - _dataspecs;
	if(diff < 0)
		return -1;
	return (static_cast<unsigned>(diff) >= _dataspec_count) ? -1 : diff;
}

#endif
