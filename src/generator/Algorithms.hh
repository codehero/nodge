#ifndef __BASE_SCHEMABUILDER_HH__
#define __BASE_SCHEMABUILDER_HH__

#include <nodge/Graph.hh>
#include "MetaSchema.hh"

namespace MetaSchema {

	/** @brief  */
	class Builder {
		public:
			/** @brief ctor
			 *  @param type_nodes Graph containing basic types.
			 *  @param model_graph Graph in which will contain the model */
			Builder(Graph& model_graph, const string& name, unsigned id_val);

			/** @brief  */
			~Builder();

			Graph::node_iterator make_type(const string& enum_name, const string& cpp_type);

			/** @brief Make an enum_spec node; this enum becomes current.
			 *  @param name Name of the enum (as in enum name{})
			 *  @param type The type of the enum
			 *  @return Newly created enum node. */
			Graph::node_iterator
				make_enum(const string& name, const Graph::node_iterator& type_ref);

			/** @brief Add enum value to current enum.
			 *  @param name Identifier
			 *  @param value actual value in enum
			 *  @return pointer to newly created enum_value */
			Graph::node_iterator
				add_enum_value(const string& name, const string& value);

			/** @brief Create independent member_spec
			 *  @param name Name of the member
			 *  @param type Node containing the member's type,
			 *  	an enum_spec is also acceptable.
			 *  @return new member spec. */
			Graph::node_iterator
				make_member_spec(const string& name, const Graph::node_iterator& type);

			/** @brief Makes a data spec given the name and whether it is UserData.
			 *  @param name The name to use for the class, DB table, etc.
			 *  @param ancestor If not NULL, node_iterator to an ancestor Data class
			 *  @return iterator to node containing the Data Spec.
			 *
			 *  side effect : change state so add_member_spec will update
			 *  to most recently created data spec. */
			Graph::node_iterator make_data_spec(const string& name,
					const Graph::node_iterator* ancestor = NULL);

			/** @brief 
			 *  @param
			 *  @return */
			Graph::node_iterator make_node_spec(const std::string& name,
				const Graph::node_iterator& data_spec_ref);

			/** @brief Add member spec to most recently created data spec.
			 *  @param name Name of the member
			 *  @param type Node containing the member's type,
			 *  	an enum_spec is also acceptable.
			 *  @return new member spec. */
			Graph::node_iterator
				add_member_spec(const string& name, const Graph::node_iterator& type);

			/** @brief Add existing member spec to most recently created data spec.
			 *  @param name Name of the member
			 *  @param type Node containing the member's type,
			 *  	an enum_spec is also acceptable.
			 *  @return new member spec. */
			void link_member_spec(const Graph::node_iterator& mem_node);

			/** @brief Make a Node representing a Link between two Nodes (parent and child)
			 *  @param num_parents Number of parents in the link
			 *  @param num_children Number of children in the link
			 *  @param name Name of the linkspec
			 *  @param parent parent's data_spec
			 *  @param child child's data_spec
			 *  @return nothing; side effect: links the Schema and the Link, creates NodeSpec. */
			Graph::node_iterator make_link(unsigned num_parents,
				unsigned num_children, const string& name,
				const Graph::node_iterator& parent, const Graph::node_iterator& child,
				bool ordered_by_insert = false);

			/** @brief  */
			void add_to_link(Graph::node_iterator& node, bool parent_type);

			/** @brief Generate the typelist enum from the list of types made. */
			Graph::node_iterator create_type_enum(const std::string& enum_name);

			/** @brief Create type list. */
			const Type* generate_type_list(void) const;

		protected:

			/** @brief the graph we are building up. */
			Graph& _model_graph;

			/** @brief The most recently made enum_spec. */
			Graph::node_iterator _cur_enum_spec;

			/** @brief The most recently made data_spec. */
			Graph::node_iterator _cur_data_spec;

			/** @brief The most recently made link_spec. */
			Graph::node_iterator _cur_link;

			/** @brief The Top level model node. */
			Graph::node_iterator _model_node;

			std::vector<Graph::node_iterator> _type_nodes;

			/** @brief Running count of data specs. */
			unsigned _data_id_count;

			/** @brief Running count of node specs. */
			unsigned _node_id_count;

			/** @brief Running count of member specs in most recently created data spec. */
			unsigned _mem_id_count;

			/** @brief Running count of link specs. */
			unsigned _link_count;

			/** @brief Current enum value order. */
			unsigned _cur_enum_order;
	};

	/* Functions. */

	void Generate(const Graph& g, const Graph::node_iterator& model_node);

}
#endif
