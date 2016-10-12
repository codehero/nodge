void get_label(const Graph::Node& n, char buffer[1024]){
	const Data& d = n.Get();
	switch(MetaSchema::model.ModelID(n.Spec())){
		case MetaSchema::TYPE_SPEC_NODE:
			strcpy(buffer, reinterpret_cast<const MetaSchema::type_spec&>(d).cpp_type.c_str());
			break;

		case MetaSchema::ENUM_VALUE_SPEC_NODE:
			strcpy(buffer,
				reinterpret_cast<const MetaSchema::enum_value_spec&>(d).cpp_name.c_str());
			break;

		case MetaSchema::ENUM_SPEC_NODE:
			strcpy(buffer,
				reinterpret_cast<const MetaSchema::enum_spec&>(d).cpp_name.c_str());
			break;

		case MetaSchema::MEM_SPEC_NODE:
			strcpy(buffer,
				reinterpret_cast<const MetaSchema::mem_spec&>(d).name.c_str());
			break;

		case MetaSchema::DATA_SPEC_NODE:
			strcpy(buffer,
				reinterpret_cast<const MetaSchema::data_spec&>(d).name.c_str());
			break;

		case MetaSchema::NODE_SPEC_NODE:
			strcpy(buffer,
				reinterpret_cast<const MetaSchema::node_spec&>(d).name.c_str());
			break;

		case MetaSchema::LINK_SPEC_NODE:
			strcpy(buffer,
				reinterpret_cast<const MetaSchema::link_spec&>(d).name.c_str());
			break;

		case MetaSchema::MODEL_SPEC_NODE:
			strcpy(buffer,
				reinterpret_cast<const MetaSchema::model_spec&>(d).name.c_str());
			break;

		default:
			buffer[0] = '\0';
			break;
	}
}

void write_dot(std::ostream& ostr, const Graph& graph){

		ostr << "digraph galaxy { graph[overlap=false,splines=false]" << std::endl;

		Graph::node_iterator ni, end;
		graph.begin(ni);
		graph.end(end);

		std::ostringstream lostr;
		char buffer[1024];
		while(ni != end){
			const Graph::Node& n = *graph.Get(ni);
			get_label(n, buffer);
			ostr << "n_" << n.Spec().name << "_" << buffer << "[" << std::endl;
			ostr << "label = \"" << buffer << "\"" << std::endl;

			Graph::link_iterator k, iend;
			graph.neighbor_begin(ni, k);
			graph.neighbor_end(ni, iend);
			while(k != iend){
				Graph::node_iterator parent, child;
				graph.at(k, true, parent);
				graph.at(k, false, child);
				if(parent == ni){

					const Graph::Node& p = *graph.Get(parent);
					get_label(p, buffer);
					lostr << "n_" << p.Spec().name << "_" << buffer << " -> ";

					const Graph::Node& c = *graph.Get(child);
					get_label(c, buffer);
					lostr << "n_" << c.Spec().name << "_" << buffer;
					lostr << "[]" << std::endl;
				}
				++k;
			}
			++ni;
			ostr << "]" << std::endl;
		}

		ostr << lostr.str();

		ostr << "}" << std::endl;
}

#if 0
int main(int argc, const char* argv[]){
	try{
		BasicGraph model_graph(MetaSchema::model);
		MetaSchema::TypeNodes types(model_graph);

		Populate(types, model_graph);

		Graph::node_iterator ni, end;
		model_graph.begin(ni);
		model_graph.end(end);

		unsigned count = 0;

		Graph::node_iterator model_node;
		while(ni != end){
			if(MetaSchema::model.ModelID(*ni.Spec()) == MetaSchema::MODEL_SPEC_NODE){
				model_graph.Clone(ni, model_node);
			}

			Graph::link_iterator k, iend;
			model_graph.neighbor_begin(ni, k);
			model_graph.neighbor_end(ni, iend);
			while(!(k == iend))
				++k;
			++ni;
			++count;
		}

		MetaSchema::Generate(model_graph, model_node);

		std::cerr << "Node Count : " << count << std::endl;

		model_graph.begin(ni);

		Graph::node_iterator *iters = new Graph::node_iterator[count];
		Data **refs = new Data*[count];

		unsigned k = 0;
		while(!(ni == end)){
			model_graph.Clone(ni, iters[k]);
			refs[k] = model_graph.NodeRef(ni);
			++ni;
			++k;
		}

		std::cerr << "Verifying references : " << std::endl;
		for(k = 0; k < count; ++k){
			Data::const_mem_iterator m(*refs[k], 0);
			Graph::node_iterator n;
			if(!model_graph.GetNode(m, n)){
				std::cerr << "Error at " << k << std::endl;
			}
		}

		delete [] iters;
		delete [] refs;

		write_dot(std::cout, model_graph);

	}
	catch(const Graph::spec_mismatch& e){
		std::cerr << e.what() << std::endl;
		std::cerr << e.lspec.name << std::endl;
		std::cerr << e.parent.name << std::endl;
		std::cerr << e.child.name << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
#endif
