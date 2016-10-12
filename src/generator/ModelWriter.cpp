#include "ModelWriter.hh"

static Graph::const_node_iterator s_get_member_type(const Graph& g,
	const Graph::const_node_iterator& mi)
{
	/*  */
	Graph::const_link_iterator li(
		g.neighbor_begin(mi, MetaSchema::link_specs[MetaSchema::LNK_MEM_KIND]));

	Graph::const_node_iterator ti(g.at(li, false));
	if(MetaSchema::model.ModelID(*ti.Spec()) == MetaSchema::ENUM_SPEC_NODE){
		li = g.neighbor_begin(ti, MetaSchema::link_specs[MetaSchema::LNK_ENUM_TYPE]);
		ti = g.at(li, false);
	}

	if(MetaSchema::model.ModelID(*ti.Spec()) != MetaSchema::TYPE_SPEC_NODE)
		throw std::runtime_error("bad type");

	return ti;
}

MetaSchema::ModelWriter::ModelWriter(const Graph& g,
		const Graph::const_node_iterator& model_node)
	: _g(g), _model_node(model_node)
{
	const auto model_data = model_node.Data<const model_spec>();
	_model_name = model_data.name;
}

void MetaSchema::ModelWriter::WriteHeader(std::ostream& ostr){
	ostr << "#ifndef __" << _model_name << "_MODEL_HH__" << std::endl;
	ostr << "#define __" << _model_name << "_MODEL_HH__" << std::endl;
	ostr << "#include <nodge/Model.hh>" << std::endl;
	ostr << "namespace " << _model_name << " { " << std::endl;

	/* Output the header enumerations */
	Graph::const_link_iterator li(
		_g.neighbor_begin(_model_node, link_specs[LNK_MODEL_ORDER]));
	Graph::const_link_iterator end(_g.neighbor_end(_model_node));
	int cur_model_id = -1;
	while(1){

		int next_model_id = -1;
		if(end != li){
			Graph::const_node_iterator child(_g.at(li, false));
			next_model_id = model.ModelID(*child.Spec());
		}

		if(next_model_id != cur_model_id){
			/* Ending the current model id */
			switch(cur_model_id){
				case DATA_SPEC_NODE:
					ostr << "\t\tDATA_SPEC_COUNT" << std::endl;
					ostr << "\t};" << std::endl;
					ostr << "\textern const Data::Spec data_specs[DATA_SPEC_COUNT];" << std::endl;
					ostr << std::endl;
					break;

				case NODE_SPEC_NODE:
					ostr << "\t\tNODE_SPEC_COUNT" << std::endl;
					ostr << "\t};" << std::endl;
					ostr << "\textern const NodeSpec node_specs[NODE_SPEC_COUNT];" << std::endl;
					ostr << std::endl;
					break;

				case LINK_SPEC_NODE:
					ostr << "\t\tLINK_SPEC_COUNT" << std::endl;
					ostr << "\t};" << std::endl;
					ostr << "\textern const LinkSpec link_specs[LINK_SPEC_COUNT];" << std::endl;
					ostr << std::endl;
					break;

				default:
					break;
			}
		}

		if(-1 == next_model_id)
			break;

		if(next_model_id != cur_model_id){
			/* Starting the new current model id */
			switch(next_model_id){
				case DATA_SPEC_NODE:
					ostr << "\tenum {"<< std::endl;
					break;

				case NODE_SPEC_NODE:
					ostr << "\tenum {"<< std::endl;
					break;

				case LINK_SPEC_NODE:
					ostr << "\tenum {"<< std::endl;
					break;

				default:
					break;
			}
			cur_model_id = next_model_id;
		}

		Graph::const_node_iterator child(_g.at(li, false));
		switch(cur_model_id){
			case DATA_SPEC_NODE:
				{
					const auto d = child.Data<const data_spec>();
					ostr << "\t\t" << d.name << "," << std::endl;
				}
				break;

			case NODE_SPEC_NODE:
				{
					const auto ns = child.Data<const node_spec>();
					ostr << "\t\t" << ns.name << "," << std::endl;
				}
				break;

			case LINK_SPEC_NODE:
				{
					const auto link = child.Data<const link_spec>();
					ostr << "\t\t" << link.name << "," << std::endl;
				}
				break;

			default:
				break;
		}
		++li;
	}


	li = _g.neighbor_begin(_model_node, link_specs[LNK_MODEL_ORDER]);
	end = _g.neighbor_end(_model_node);
	cur_model_id = -1;
	while(li != end){
		Graph::const_node_iterator child(_g.at(li, false));
		if(DATA_SPEC_NODE == model.ModelID(*child.Spec())){
			WriteClassHeader(ostr, child);
			ostr << std::endl;
		}
		++li;
	};

	ostr << "\textern Model model;" << std::endl;

	ostr << "}" << std::endl;
	ostr << "#endif" << std::endl;
}

void MetaSchema::ModelWriter::WriteCpp(std::ostream& ostr) {
	/* Output the header enumerations */
	Graph::const_link_iterator li(
		_g.neighbor_begin(_model_node, link_specs[LNK_MODEL_ORDER]));
	Graph::const_link_iterator end(_g.neighbor_end(_model_node));
	int cur_model_id = -1;
	while(li != end){
		Graph::const_node_iterator child(_g.at(li, false));
		if(model.ModelID(*child.Spec()) != cur_model_id){

			switch(cur_model_id){
				case TYPE_SPEC_NODE:
					ostr << "};" << std::endl;
					break;

				default:
					break;
			}

			switch(model.ModelID(*child.Spec())){
				case TYPE_SPEC_NODE:
					ostr << "const Type " << _model_name << "::type_list["
						<< _model_name << "::NUM_TYPES] = {" << std::endl;
					break;

				default:
					break;
			}
			cur_model_id = model.ModelID(*child.Spec());
		}

		switch(model.ModelID(*child.Spec())){
			case TYPE_SPEC_NODE:
				{
					const auto t = child.Data<const type_spec>();
					ostr << "\t{\"" << t.enum_name << "\", \"" << t.cpp_type << "\"}," << std::endl;
				}
				break;

			default:
				break;
		}

		++li;
	}
}

void MetaSchema::ModelWriter::WriteClassHeader(std::ostream& ostr, const Graph::const_node_iterator& dspec_node){

	auto dspec = dspec_node.Data<const data_spec>();

	/* use ancestor class if defined. */
	string ancestor_name = "Data";
	{
		Graph::const_link_iterator li(
			_g.neighbor_begin(dspec_node, link_specs[LNK_DATA_ANCESTOR]));
		Graph::const_link_iterator end(_g.neighbor_end(dspec_node));
		if(end != li){
			Graph::const_node_iterator ancestor_node(_g.at(li, false));
			auto anc = ancestor_node.Data<const data_spec>();
			ancestor_name = anc.name;
		}
	}

	ostr << "\tclass " << dspec.name << " : public " << ancestor_name << " {" << std::endl;
	ostr << "\t\tpublic:" << std::endl;
	ostr << "\t\ttypedef " << ancestor_name << " Ancestor;" << std::endl;
	ostr << "\t\tstatic const unsigned ID = " << dspec.name << "_DATA_SPEC;" << std::endl;

	/* Output member definitions. */
	Graph::const_link_iterator li(
		_g.neighbor_begin(dspec_node, link_specs[LNK_DATA_MEM]));
	Graph::const_link_iterator end(_g.neighbor_end(dspec_node));
	while(li != end){
		Graph::const_node_iterator mem_node(_g.at(li, false));
		auto mem = mem_node.Data<const mem_spec>();
		Graph::const_node_iterator type_node(s_get_member_type(_g, mem_node));
		auto type = type_node.Data<const type_spec>();
		ostr << "\t\t" << type.cpp_type << " " << mem.name << ";" << std::endl;

		++li;
	}
	ostr << std::endl;

	/* Output member enumerations. */
	ostr << "\t\tenum {" << std::endl;
	li = _g.neighbor_begin(dspec_node, link_specs[LNK_DATA_MEM]);
	while(li != end){
		Graph::const_node_iterator mem_node(_g.at(li, false));
		auto mem = mem_node.Data<const mem_spec>();
		ostr << "\t\t\t" << mem.name << "_MEM_ID," << std::endl;
		++li;
	}
	ostr << "\t\t};" << std::endl;
	ostr << std::endl;

	/* Output member accessors. */
#if 0
virtual void Get(const MemberSpec& id, const unsigned*& x) const throw(parameter_error);
virtual void Get(const MemberSpec& id, const string*& x) const throw(parameter_error);
#endif

	ostr << "\t\t" << dspec.name << "(void);" << std::endl;
	ostr << "\t\t~" << dspec.name << "(void);" << std::endl;
	ostr << "\t\t" << dspec.name << "* Clone(void) const;" << std::endl;
	ostr << "\t\tconst Spec& GetSpec(void) const {" << std::endl;
	ostr << "\t\t\treturn data_specs[" << dspec.name << "_DATA_SPEC];" << std::endl;
	ostr << "\t\t}" << std::endl;
	ostr << "\t\tstatic Data* CreateInstance(void){" << std::endl;
	ostr << "\t\t\treturn new " << dspec.name << ";" << std::endl;
	ostr << "\t\t}" << std::endl;
	ostr << "\t};" << std::endl;
}
