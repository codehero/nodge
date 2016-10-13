#include "MetaSchema.hh"

const Type MetaSchema::type_list[Type::NUM_TYPES] = {
	 {"M_BOOL", "bool"}
	,{"M_UNSIGNED_INT", "unsigned"}
	,{"M_INT", "int"}
	,{"M_UNSIGNED_LONG", "unsigned long"}
	,{"M_LONG", "long"}
	,{"M_STRING", "string"}
	,{"M_DOUBLE", "double"}
};

/* type_spec */

using MetaSchema::type_spec;

type_spec::type_spec() noexcept {
}

type_spec::~type_spec() noexcept{
}

type_spec* type_spec::Clone() const {
	type_spec* retval = new type_spec;
	*retval = *this;
	return retval;
}

static EnumValue data_mbr_types[] = {
	{"M_BOOL", "0"},
	{"M_UNSIGNED_INT", "1"},
	{"M_INT", "2"},
	{"M_UNSIGNED_LONG", "3"},
	{"M_LONG", "4"},
	{"M_STRING", "5"},
	{"M_FLOAT", "6"},
	{"M_DOUBLE", "7"},
};

static const Enum member_enum =
	{"MemberEnum", MetaSchema::type_list[Type::M_UNSIGNED_INT], data_mbr_types, 11};


static const MemberSpec type_spec_member_specs[] = {
	MemberSpec(0, Type::M_STRING, "cpp_type", NULL),
	MemberSpec(1, Type::M_STRING, "enum_name", &member_enum),
};

void type_spec::Get(const MemberSpec& ms, const string*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 0:
			x = &cpp_type;
			break;
		case 1:
			x = &enum_name;
			break;
		default: Data::Get(ms, x);
	}
}


/* enum_spec  */

using MetaSchema::enum_spec;

enum_spec::enum_spec() noexcept {
}

enum_spec::~enum_spec() noexcept{
}

enum_spec* enum_spec::Clone() const {
	enum_spec* retval = new enum_spec;
	*retval = *this;
	return retval;
}

static const MemberSpec enum_spec_member_specs[] = {
	MemberSpec(0, Type::M_STRING, "cpp_name", NULL),
};

void enum_spec::Get(const MemberSpec& ms, const string*& x) const throw(parameter_error){
	switch(ms.id){
		case 0:
			x = &cpp_name;
			break;
		default: Data::Get(ms, x);
	}
}

/* enum_value_spec */

using MetaSchema::enum_value_spec;

static const MemberSpec enum_value_spec_member_specs[] = {
	MemberSpec(0, Type::M_STRING, "cpp_name", NULL),
	MemberSpec(1, Type::M_STRING, "value", NULL),
	MemberSpec(2, Type::M_UNSIGNED_INT, "order", NULL),
};

enum_value_spec::enum_value_spec() noexcept {
}

enum_value_spec::~enum_value_spec() noexcept{
}

enum_value_spec* enum_value_spec::Clone() const {
	enum_value_spec* retval = new enum_value_spec;
	*retval = *this;
	return retval;
}

void enum_value_spec::Get(const MemberSpec& ms, const string*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 0:
			x = &cpp_name;
			break;
		case 1:
			x = &value;
			break;
		default: Data::Get(ms, x);
	}
}

void enum_value_spec::Get(const MemberSpec& ms, const unsigned*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 2:
			x = &order;
			break;
		default: Data::Get(ms, x);
	}
}


/* mem_spec */

using MetaSchema::mem_spec;

static const MemberSpec mem_spec_member_specs[] = {
	MemberSpec(0, Type::M_UNSIGNED_INT, "id", NULL),
	MemberSpec(1, Type::M_STRING, "name", NULL),
};

mem_spec::mem_spec() noexcept {
}

mem_spec::~mem_spec() noexcept{
}

mem_spec* mem_spec::Clone() const {
	mem_spec* retval = new mem_spec;
	*retval = *this;
	return retval;
}

void mem_spec::Get(const MemberSpec& ms, const unsigned*& x) const throw(parameter_error){
	switch(ms.id){
		case 0:
			x = &id;
			break;
		default: Data::Get(ms, x);
	}
}

void mem_spec::Get(const MemberSpec& ms, const string*& x) const throw(parameter_error){
	switch(ms.id){
		case 1:
			x = &name;
			break;
		default: Data::Get(ms, x);
	}
}

/* data_spec */

using MetaSchema::data_spec;

static const MemberSpec data_spec_member_specs[] = {
	MemberSpec(0, Type::M_UNSIGNED_INT, "id", NULL),
	MemberSpec(1, Type::M_STRING, "name", NULL),
};

data_spec::data_spec() noexcept {
}

data_spec::~data_spec() noexcept{
}

data_spec* data_spec::Clone() const {
	data_spec* retval = new data_spec;
	*retval = *this;
	return retval;
}

void data_spec::Get(const MemberSpec& ms, const unsigned*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 0:
			x = &id;
			break;
		default: Data::Get(ms, x);
	}
}

void data_spec::Get(const MemberSpec& ms, const string*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 1:
			x = &name;
			break;
		default: Data::Get(ms, x);
	}
}

/* node_spec */

using MetaSchema::node_spec;

static const MemberSpec node_spec_member_specs[] = {
	MemberSpec(0, Type::M_UNSIGNED_INT, "id", NULL),
	MemberSpec(1, Type::M_UNSIGNED_INT, "num_parent_links", NULL),
	MemberSpec(2, Type::M_UNSIGNED_INT, "num_child_links", NULL),
	MemberSpec(3, Type::M_STRING, "name", NULL),
};

node_spec::node_spec() noexcept {
}

node_spec::~node_spec() noexcept {
}

node_spec* node_spec::Clone() const {
	node_spec* retval = new node_spec;
	*retval = *this;
	return retval;
}

void node_spec::Get(const MemberSpec& ms, const unsigned*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 0:
			x = &id;
			break;
		case 1:
			x = &num_parent_links;
			break;
		case 2:
			x = &num_child_links;
			break;
		default: Data::Get(ms, x);
	}
}

void node_spec::Get(const MemberSpec& ms, const string*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 3:
			x = &name;
			break;
		default: Data::Get(ms, x);
	}
}


/* link_spec */

using MetaSchema::link_spec;

static const MemberSpec link_spec_member_specs[] = {
	MemberSpec(0, Type::M_UNSIGNED_INT, "id", NULL),
	MemberSpec(1, Type::M_UNSIGNED_INT, "num_parents", NULL),
	MemberSpec(2, Type::M_UNSIGNED_INT, "num_children", NULL),
	MemberSpec(3, Type::M_STRING, "name", NULL),
	MemberSpec(4, Type::M_BOOL, "ordered_by_insert", NULL),
};

link_spec::link_spec() noexcept {
}
link_spec::~link_spec() noexcept{ }

link_spec* link_spec::Clone() const {
	link_spec* retval = new link_spec;
	*retval = *this;
	return retval;
}

void link_spec::Get(const MemberSpec& ms, const unsigned*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case MEM_ID:
			x = &id;
			break;
		case MEM_NUM_PARENTS:
			x = &num_parents;
			break;
		case MEM_NUM_CHILDREN:
			x = &num_children;
			break;

		case MEM_NUM_PARENT_TYPES:
			x = &num_parent_types;
			break;

		case MEM_NUM_CHILDREN_TYPES:
			x = &num_children_types;
			break;

		default: Data::Get(ms, x);
	}
}

void link_spec::Get(const MemberSpec& ms, const string*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case MEM_NAME:
			x = &name;
			break;
		default: Data::Get(ms, x);
	}
}

void link_spec::Get(const MemberSpec& ms, const bool*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case MEM_NAME:
			x = &ordered_by_insert;
			break;
		default: Data::Get(ms, x);
	}
}

/* model_spec  */

using MetaSchema::model_spec;

static const MemberSpec model_spec_member_specs[] = {
	MemberSpec(0, Type::M_STRING, "name", NULL),
	MemberSpec(1, Type::M_UNSIGNED_INT, "id", NULL),
};

model_spec::model_spec() noexcept {
}

model_spec::~model_spec() noexcept {
}

model_spec* model_spec::Clone() const {
	model_spec* retval = new model_spec;
	*retval = *this;
	return retval;
}

void model_spec::Get(const MemberSpec& ms, const unsigned*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 1:
			x = &id;
			break;
		default: Data::Get(ms, x);
	}
}

void model_spec::Get(const MemberSpec& ms, const string*& x) const
	throw(parameter_error)
{
	switch(ms.id){
		case 0:
			x = &name;
			break;
		default: Data::Get(ms, x);
	}
}


/**********************************************************************/

/* Model Level Stuff. */

extern "C" Model& MetaSchema_get_handle(void){
	return MetaSchema::model;
}

/* This is just for sanity here: */
enum {
	DIV_MEM_KIND = 0
	,DIV_ENUM_EVALUE = 3
	,DIV_ENUM_TYPE = 5
	,DIV_DATA_MEM = 7

	,DIV_DATA_ANCESTOR = 9
	,DIV_NODE_DATA = 11
	,DIV_LINK_PARENT = 13
	,DIV_LINK_CHILD = 15

	,DIV_MODEL_LINK = 17
	,DIV_OWNER_MODEL = 19
	,DIV_MODEL_ORDER = 24
};

/* Data::Spec */
/* data_specs */
extern const Data::Spec MetaSchema::data_specs[DATA_SPEC_COUNT] =
{
	Data::Spec("type_spec", type_spec_member_specs,
		sizeof(type_spec_member_specs)/sizeof(MemberSpec),
		&type_spec::CreateInstance),

	Data::Spec("enum_spec", enum_spec_member_specs,
		sizeof(enum_spec_member_specs)/sizeof(MemberSpec),
		&enum_spec::CreateInstance),

	Data::Spec("enum_value_spec", enum_value_spec_member_specs,
		sizeof(enum_value_spec_member_specs)/sizeof(MemberSpec),
		&enum_value_spec::CreateInstance),

	Data::Spec("mem_spec", mem_spec_member_specs,
		sizeof(mem_spec_member_specs)/sizeof(MemberSpec), &mem_spec::CreateInstance),

	Data::Spec("data_spec", data_spec_member_specs,
		sizeof(data_spec_member_specs)/sizeof(MemberSpec),
		&data_spec::CreateInstance),

	Data::Spec("node_spec", node_spec_member_specs,
		sizeof(node_spec_member_specs)/sizeof(MemberSpec),
		&node_spec::CreateInstance),

	Data::Spec("link_spec", link_spec_member_specs,
		sizeof(link_spec_member_specs)/sizeof(MemberSpec),
		&link_spec::CreateInstance),

	Data::Spec("model_spec", model_spec_member_specs,
		sizeof(model_spec_member_specs)/sizeof(MemberSpec),
		&model_spec::CreateInstance)
};

const NodeSpec * link_node_specs[] = {
	MetaSchema::node_specs + MetaSchema::MEM_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::TYPE_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::ENUM_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::ENUM_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::ENUM_VALUE_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::ENUM_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::TYPE_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::DATA_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::MEM_SPEC_NODE,


	MetaSchema::node_specs + MetaSchema::DATA_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::DATA_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::NODE_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::DATA_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::LINK_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::NODE_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::LINK_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::NODE_SPEC_NODE,


	MetaSchema::node_specs + MetaSchema::MODEL_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::LINK_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::NODE_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::MEM_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::DATA_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::ENUM_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::MODEL_SPEC_NODE,

	MetaSchema::node_specs + MetaSchema::MODEL_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::TYPE_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::DATA_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::NODE_SPEC_NODE,
	MetaSchema::node_specs + MetaSchema::LINK_SPEC_NODE,
};

const LinkSpec MetaSchema::link_specs[] = {
	LinkSpec(MetaSchema::model,
			"lnk_mem_kind",
			LinkSpec::MANY,
			1,
			1,
			2,
			link_node_specs + DIV_MEM_KIND
			),

	LinkSpec(MetaSchema::model,
			"lnk_enum_enum_value"
			,1
			,LinkSpec::MANY
			,1
			,1
			,link_node_specs +  DIV_ENUM_EVALUE
			,true
			),

	LinkSpec(MetaSchema::model,
			"lnk_enum_type",
			LinkSpec::MANY,
			1,
			1,
			1,
			link_node_specs + DIV_ENUM_TYPE
			),

	LinkSpec(MetaSchema::model,
			"lnk_data_mem"
			,LinkSpec::MANY
			,LinkSpec::MANY
			,1
			,1
			,link_node_specs + DIV_DATA_MEM
			,true
			),

	LinkSpec(MetaSchema::model,
			"lnk_data_ancestor",
			LinkSpec::MANY,
			1,
			1,
			1,
			link_node_specs + DIV_DATA_ANCESTOR
			),

	LinkSpec(MetaSchema::model,
			"lnk_node_data",
			LinkSpec::MANY,
			1,
			1,
			1,
			link_node_specs + DIV_NODE_DATA
			),

	LinkSpec(MetaSchema::model,
			"lnk_link_parent_node",
			LinkSpec::MANY,
			LinkSpec::MANY,
			1,
			1,
			link_node_specs + DIV_LINK_PARENT
			),

	LinkSpec(MetaSchema::model,
			"lnk_link_child_node",
			LinkSpec::MANY,
			LinkSpec::MANY,
			1,
			1,
			link_node_specs + DIV_LINK_CHILD
			),

	LinkSpec(MetaSchema::model,
			"lnk_model_link",
			1,
			LinkSpec::MANY,
			1,
			1,
			link_node_specs + DIV_MODEL_LINK
			),

	LinkSpec(MetaSchema::model,
			"lnk_owner_model",
			LinkSpec::MANY,
			1,
			4,
			1,
			link_node_specs + DIV_OWNER_MODEL
			),

	LinkSpec(MetaSchema::model,
			"lnk_model_order",
			1,
			LinkSpec::MANY,
			1,
			4,
			link_node_specs + DIV_MODEL_ORDER
			,true
			),
};

/* NodeSpec list */

const NodeSpec MetaSchema::node_specs[MetaSchema::NODE_SPEC_COUNT] = {
	NodeSpec(MetaSchema::model,
			"type_spec_node",
			MetaSchema::data_specs[TYPE_DATA_SPEC]
			),

	NodeSpec(MetaSchema::model,
			"enum_value_spec_node",
			MetaSchema::data_specs[ENUM_VALUE_DATA_SPEC]
			),

	NodeSpec(MetaSchema::model,
			"enum_spec_node",
			MetaSchema::data_specs[ENUM_DATA_SPEC]
			),

	NodeSpec(MetaSchema::model,
			"mem_spec_node",
			MetaSchema::data_specs[MEM_DATA_SPEC]
			),

	NodeSpec(MetaSchema::model,
			"data_spec_node",
			MetaSchema::data_specs[DATA_DATA_SPEC]
			),

	NodeSpec(MetaSchema::model,
			"node_spec_node",
			MetaSchema::data_specs[NODE_DATA_SPEC]
			),

	NodeSpec(MetaSchema::model,
			"link_spec_node",
			MetaSchema::data_specs[LINK_DATA_SPEC]
			),

	NodeSpec(MetaSchema::model,
			"model_spec_node",
			MetaSchema::data_specs[MODEL_DATA_SPEC]
			)
};

/* External Nodes. */

/* Link Index. */

static const LinkSpec* link_index_arr[] = {
	/* node_specs[MetaSchema::TYPE_SPEC_NODE] */

	/* parents */

	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_TYPE],
	&MetaSchema::link_specs[MetaSchema::LNK_MEM_KIND],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* child */

	/* many count  */

	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_TYPE],
	&MetaSchema::link_specs[MetaSchema::LNK_MEM_KIND],

	/* constant neighbors */

	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* node_specs[MetaSchema::ENUM_VALUE_SPEC_NODE] */

	/* parents */
	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_ENUM_VALUE],

	/* child */

	/* many count  */

	/* constant neighbors  */
	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_ENUM_VALUE],

	/* node_specs[MetaSchema::ENUM_SPEC_NODE] */

	/* parents */
	&MetaSchema::link_specs[MetaSchema::LNK_MEM_KIND],

	/* children */
	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_TYPE],
	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_ENUM_VALUE],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],

	/* many */
	&MetaSchema::link_specs[MetaSchema::LNK_MEM_KIND],
	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_ENUM_VALUE],

	/* constant */
	&MetaSchema::link_specs[MetaSchema::LNK_ENUM_TYPE],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],

	/* node_specs[MetaSchema::MEM_SPEC_NODE] */
	/* Parents */
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_MEM],

	/* children */
	&MetaSchema::link_specs[MetaSchema::LNK_MEM_KIND],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],

	/* many */
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_MEM],

	/* constant */
	&MetaSchema::link_specs[MetaSchema::LNK_MEM_KIND],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],

	/* node_specs[MetaSchema::DATA_SPEC_NODE] */
	/* Parents */
	&MetaSchema::link_specs[MetaSchema::LNK_NODE_DATA],
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_ANCESTOR],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* children */
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_MEM],
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_ANCESTOR],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],

	/* many */
	&MetaSchema::link_specs[MetaSchema::LNK_NODE_DATA],
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_ANCESTOR],
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_MEM],

	/* constant */
	&MetaSchema::link_specs[MetaSchema::LNK_DATA_ANCESTOR],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* node_specs[MetaSchema::NODE_SPEC_NODE] */
	/* parents */
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_PARENT_NODE],
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_CHILD_NODE],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* children */
	&MetaSchema::link_specs[MetaSchema::LNK_NODE_DATA],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],

	/* many */
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_PARENT_NODE],
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_CHILD_NODE],

	/* constant */
	&MetaSchema::link_specs[MetaSchema::LNK_NODE_DATA],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* node_specs[MetaSchema::LINK_SPEC_NODE] */
	/* parents */
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_LINK],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* children */
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_PARENT_NODE],
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_CHILD_NODE],

	/* many */
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_LINK],
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_PARENT_NODE],
	&MetaSchema::link_specs[MetaSchema::LNK_LINK_CHILD_NODE],

	/* constant */
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* node_specs[MetaSchema::MODEL_SPEC_NODE] */
	/* parents */
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],

	/* children */
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_LINK],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* many */
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_LINK],
	&MetaSchema::link_specs[MetaSchema::LNK_OWNER_MODEL],
	&MetaSchema::link_specs[MetaSchema::LNK_MODEL_ORDER],

	/* constant */
};

/* Node Index. */

/* Graph search should create table indexed by node_spec and ordered by node_spec.id;
 * The task here is to create a table indexed by the node_spec
 * such that:
 *
 * -Column 1 = count of link_spec->lnk_child_node->node_spec
 * -Column 2 = count of link_spec->lnk_parent_node->node_spec
 * -Column 3 = count of ((link_spec->lnk_parent_node->node_spec
 *  	where link_spec.num_children == MANY) ||
 *    (link_spec->lnk_child_node->node_spec
 *  	where link_spec.num_parents == MANY))
 *
 * -Column 4 = count of ((link_spec->lnk_parent_node->node_spec
 *  	where link_spec.num_children != MANY) &&
 *    (link_spec->lnk_child_node->node_spec
 *  	where link_spec.num_parents != MANY))
 *
 *
 * Summary Values:
 * -Column 0 (offset) should be determined after traversal;
 *  counting will increase linearly
 *  by a later function (sum of 1,2,3,4) of all previous rows
 *
 * */
static Model::NodeLinks node_index_arr[] = {
	/* node_specs[MetaSchema::TYPE_SPEC_NODE] */
	{
		0, 3, 0, 2, 1, 1, 1
	},
	/* node_specs[MetaSchema::ENUM_VALUE_SPEC_NODE] */
	{
		6, 1, 0, 0, 1, 1, 1
	},
	/* node_specs[MetaSchema::ENUM_SPEC_NODE] */
	{
		8, 1, 3, 2, 2, 1, 2
	},
	/* node_specs[MetaSchema::MEM_SPEC_NODE] */
	{
		16, 1, 2, 1, 2, 1, 1
	},
	/* node_specs[MetaSchema::DATA_SPEC_NODE] */
	{
		22, 3, 3, 3, 3, 1, 3
	},
	/* node_specs[MetaSchema::NODE_SPEC_NODE] */
	{
		34, 3, 2, 2, 3, 0, 3
	},
	/* node_specs[MetaSchema::LINK_SPEC_NODE] */
	{
		44, 2, 2, 2, 2, 0, 2
	},
	/* node_specs[MetaSchema::MODEL_SPEC_NODE] */
	{
		52, 1, 2, 3, 0, 1, 0
	}
};

Model MetaSchema::model(string("MetaSchema"),
		static_cast<const NodeSpec*>(MetaSchema::node_specs),
		sizeof(MetaSchema::node_specs) / sizeof(NodeSpec),
		MetaSchema::link_specs,
		MetaSchema::LINK_SPEC_COUNT,
		MetaSchema::data_specs,
		MetaSchema::DATA_SPEC_COUNT,
		NULL, 0,
		link_index_arr, sizeof(link_index_arr) / sizeof(const LinkSpec*),
		node_index_arr, sizeof(node_index_arr) / sizeof(Model::NodeLinks)
);
