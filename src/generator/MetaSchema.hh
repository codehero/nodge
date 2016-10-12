/* BEGIN model_hh */
#ifndef __DATASCHEMA_H__
#define __DATASCHEMA_H__

#include <nodge/Model.hh>

namespace MetaSchema {

	/* Ordered Sequence of data_spec */
	enum {
		TYPE_DATA_SPEC,
		ENUM_VALUE_DATA_SPEC,
		ENUM_DATA_SPEC,
		MEM_DATA_SPEC,
		DATA_DATA_SPEC,
		NODE_DATA_SPEC,
		LINK_DATA_SPEC,
		MODEL_DATA_SPEC,
		DATA_SPEC_COUNT
	};
	extern const Data::Spec data_specs[DATA_SPEC_COUNT];

	/* Ordered Sequence of node_spec */
	enum {
		TYPE_SPEC_NODE,
		ENUM_VALUE_SPEC_NODE,
		ENUM_SPEC_NODE,
		MEM_SPEC_NODE,
		DATA_SPEC_NODE,
		NODE_SPEC_NODE,
		LINK_SPEC_NODE,
		MODEL_SPEC_NODE,
		NODE_SPEC_COUNT
	};
	extern const NodeSpec node_specs[NODE_SPEC_COUNT];

	/* Ordered Sequence of link_spec */
	enum {
		LNK_MEM_KIND,
		LNK_ENUM_ENUM_VALUE,
		LNK_ENUM_TYPE,
		LNK_DATA_MEM,
		LNK_DATA_ANCESTOR,
		LNK_NODE_DATA,
		LNK_LINK_PARENT_NODE,
		LNK_LINK_CHILD_NODE,
		LNK_MODEL_LINK,
		LNK_OWNER_MODEL,
		LNK_MODEL_ORDER,
		LINK_SPEC_COUNT
	};
	extern const LinkSpec link_specs[LINK_SPEC_COUNT];

	/** @brief Listing of type metadata. */
	extern const Type type_list[Type::NUM_TYPES];

	/* BEGIN data_spec_hh */

	/* Context under data_spec */
	class type_spec : public Data {
		public:
			/* Ordered Sequence of mem_spec */
			enum {
				MEM_CPP_TYPE,
				MEM_ENUM_NAME,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = TYPE_DATA_SPEC;

			/* Unordered Non-Repeated Sequence of mem_spec */
			string cpp_type;
			string enum_name;

			/* Unordered Non-Repeated Sequence of type_specs under data_spec */
			void Get(const MemberSpec& id, const string*& x) const
				throw(parameter_error);

			type_spec(void) throw();
			~type_spec() throw();
			type_spec* Clone() const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[TYPE_DATA_SPEC];
			}

			static Data* CreateInstance(void){
				return new type_spec;
			}
	};

	class enum_value_spec : public Data {
		public:
			enum {
				MEM_CPP_NAME,
				MEM_VALUE,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = ENUM_VALUE_DATA_SPEC;

			string cpp_name;
			string value;
			unsigned order;

			void Get(const MemberSpec& id, const string*& x) const
				throw(parameter_error);

			void Get(const MemberSpec& id, const unsigned*& x) const
				throw(parameter_error);

			enum_value_spec(void) throw();
			~enum_value_spec() throw();
			enum_value_spec* Clone(void) const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[ENUM_VALUE_DATA_SPEC];
			}
			static Data* CreateInstance(void){
				return new enum_value_spec;
			}
	};

	class enum_spec : public Data {
		public:
			enum {
				MEM_CPP_NAME,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = ENUM_DATA_SPEC;

			string cpp_name;

			void Get(const MemberSpec& id, const string*& x) const
				throw(parameter_error);

			enum_spec(void) throw();
			~enum_spec() throw();
			enum_spec* Clone(void) const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[ENUM_DATA_SPEC];
			}
			static Data* CreateInstance(void){
				return new enum_spec;
			}
	};

	class mem_spec : public Data {
		public:
			enum {
				MEM_ID,
				MEM_NAME,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = MEM_DATA_SPEC;

			unsigned id;
			string name;

			void Get(const MemberSpec& id, const unsigned*& x) const
				throw(parameter_error);

			void Get(const MemberSpec& id, const string*& x) const
				throw(parameter_error);

			mem_spec(void) throw();
			~mem_spec() throw();
			mem_spec* Clone(void) const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[MEM_DATA_SPEC];
			}
			static Data* CreateInstance(void){
				return new mem_spec;
			}
	};

	class data_spec : public Data {
		public:
			enum {
				MEM_ID,
				MEM_NAME,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = DATA_DATA_SPEC;

			unsigned id;
			string name;

			void Get(const MemberSpec& id, const unsigned*& x) const
				throw(parameter_error);
			void Get(const MemberSpec& id, const string*& x) const
				throw(parameter_error);

			data_spec(void) throw();
			~data_spec() throw();
			data_spec* Clone(void) const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[DATA_DATA_SPEC];
			}
			static Data* CreateInstance(void){
				return new data_spec;
			}
	};

	class node_spec : public Data {
		public:
			enum {
				MEM_ID,
				MEM_NUM_PARENT_LINKS,
				MEM_NUM_CHILD_LINKS,
				MEM_NAME,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = NODE_DATA_SPEC;

			unsigned id;
			unsigned num_parent_links;
			unsigned num_child_links;
			string name;

			void Get(const MemberSpec& id, const unsigned*& x) const
				throw(parameter_error);

			void Get(const MemberSpec& id, const string*& x) const
				throw(parameter_error);

			node_spec(void) throw();
			~node_spec() throw();
			node_spec* Clone(void) const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[NODE_DATA_SPEC];
			}
			static Data* CreateInstance(void){
				return new node_spec;
			}
	};

	class link_spec : public Data {
		public:
			enum {
				MEM_ID,
				MEM_NUM_PARENTS,
				MEM_NUM_CHILDREN,
				MEM_NUM_PARENT_TYPES,
				MEM_NUM_CHILDREN_TYPES,
				MEM_NAME,
				MEM_ORDERED_BY_INSERT,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = LINK_DATA_SPEC;

			unsigned id;
			unsigned num_parents;
			unsigned num_children;
			unsigned num_parent_types;
			unsigned num_children_types;
			string name;
			bool ordered_by_insert;

			void Get(const MemberSpec& id, const unsigned*& x) const
				throw(parameter_error);
			void Get(const MemberSpec& id, const string*& x) const
				throw(parameter_error);
			void Get(const MemberSpec& id, const bool*& x) const
				throw(parameter_error);

			link_spec(void) throw();
			~link_spec() throw();
			link_spec* Clone(void) const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[LINK_DATA_SPEC];
			}
			static Data* CreateInstance(void){
				return new link_spec;
			}
	};

	class model_spec : public Data {
		public:
			enum {
				MEM_NAME,
				MEM_ID,
				MEM_SPEC_COUNT
			};

			static const unsigned ID = MODEL_DATA_SPEC;

			string name;
			unsigned id;

			void Get(const MemberSpec& id, const unsigned*& x) const
				throw(parameter_error);
			void Get(const MemberSpec& id, const string*& x) const throw(parameter_error);

			model_spec(void) throw();
			~model_spec() throw();
			model_spec* Clone(void) const;

			const Spec& GetSpec(void) const throw(){
				return data_specs[MODEL_DATA_SPEC];
			}
			static Data* CreateInstance(void){
				return new model_spec;
			}
	};

	/* END data_spec_hh */

	extern Model model;
}

extern "C" Model& MetaSchema_get_handle(void);

#endif
