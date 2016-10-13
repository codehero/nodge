#ifndef GRAPHSPEC_H
#define GRAPHSPEC_H

#include "Data.hh"

/* Forward Declarations. */
class Model;
struct NodeSpec;

/** @brief  */
struct LinkSpec{

	/** @brief For MANY to MANY, this will be the generated Data::Spec
	 * for the intermediate table. */
	typedef enum{
		MANY_TO_MANY,
		ONE_TO_MANY,
		MANY_TO_ONE,
		ONE_TO_ONE
	} Relationship;

	/** @brief  */
	static const unsigned MANY = 0;

	/** @brief Model containing the linkspec. This field should be considered
	 * Note that the pointer to this LinkSpec could be passed to Model
	 * to achieve the same purpose as this reference;
	 * the model field is here for caching reasons! */
	const Model& model;

	/** @brief Text identifier of the link. */
	string name;

	/** @brief Convenience value for determining relationship type without looking
	 * at num_parents and num_children. */
	Relationship relationship;

	/** @brief How many parents link to the children. */
	unsigned num_parents;

	/** @brief How many children for each parent. */
	unsigned num_children;

	/** @brief How many different kinds of parents there can be. */
	unsigned num_parent_types;

	/** @brief How many different kinds of parents there can be. */
	unsigned num_children_types;

	/** @brief Array of all possible types involved in link.
	 * First set is [0..num_parent_types-1],
	 * Second set is [num_parent_types, num_parent_types+num_children_types-1]*/
	const NodeSpec* const * endpoint_types;

	/** @brief Whether insertion order determines iteration order of this kind of link. */
	bool ordered_by_insert;

	/** @brief Ancestor linkspec off which this is based. */
	const LinkSpec* ancestor;

	/** @brief 
	 *  @param p_model
	 *  @param p_name
	 *  @param p_num_parents
	 *  @param p_num_children
	 *  @param p_num_p_types
	 *  @param p_num_c_types
	 *  @param p_node_specs
	 *  @param p_ordered
	 *  @param p_ancestor */
	LinkSpec(const Model& p_model, const string& p_name,
		unsigned p_num_parents, unsigned p_num_children,
		unsigned p_num_p_types, unsigned p_num_c_types,
		const NodeSpec* const * p_node_specs,
		bool p_ordered = false,
		const LinkSpec* p_ancestor = NULL);

	/** @brief  */
	bool operator ==(const LinkSpec& lspec) const noexcept;

	/** @brief  */
	unsigned ConstantNeighborsMax(void) const noexcept;

	/** @brief  */
	bool IsEndpointType(const NodeSpec& ns, bool parent) const noexcept;

	protected:
		/** @brief For use only by generation. */
		static Relationship calc_relation(unsigned p, unsigned c) noexcept;
};

/** @brief  */
typedef const LinkSpec* const * const Links;

/** @brief  */
struct NodeSpec{

	/** @brief Parent Model owning the nodespec. */
	const Model& model;

	/** @brief Text identifier of the node. */
	string name;

	/** @brief Reference to Data associated with this node. */
	const Data::Spec& dspec;

	/** @brief 
	 *  @param p_model
	 *  @param p_name
	 *  @param p_dspec */
	NodeSpec(const Model& p_model, const string& p_name,
		const Data::Spec& p_dspec);

	/** @brief  */
	bool operator ==(const NodeSpec& other) const noexcept;
};

/* LinkSpec */

inline LinkSpec::LinkSpec(const Model& p_model, const string& p_name,
	unsigned p_num_parents, unsigned p_num_children,
	unsigned p_num_p_types, unsigned p_num_c_types,
	const NodeSpec* const * node_specs,
	bool p_ordered,
	const LinkSpec* p_ancestor)
	: model(p_model), name(p_name)
	,relationship(calc_relation(p_num_parents, p_num_children))
	,num_parents(p_num_parents), num_children(p_num_children)
	,num_parent_types(p_num_p_types), num_children_types(p_num_c_types)
	,endpoint_types(node_specs), ordered_by_insert(p_ordered)
	,ancestor(p_ancestor)
{
}

inline bool LinkSpec::operator ==(const LinkSpec& lspec) const noexcept{
	return this == &lspec;
}

inline unsigned LinkSpec::ConstantNeighborsMax(void) const noexcept{
	return num_parents + num_children;
}

inline LinkSpec::Relationship LinkSpec::calc_relation(
	unsigned p, unsigned c) noexcept
{
	if(p == MANY && c == MANY)
		return MANY_TO_MANY;
	if(p != MANY && c != MANY)
		return ONE_TO_ONE;
	return (p == MANY) ? MANY_TO_ONE : ONE_TO_MANY;
}

/* NodeSpec */

inline bool NodeSpec::operator ==(const NodeSpec& other) const noexcept{
	return this == &other;
}

#endif
