#ifndef __MODELWRITER_HH__
#define __MODELWRITER_HH__

#include <ostream>
#include <nodge/Graph.hh>
#include "MetaSchema.hh"


namespace MetaSchema {
	class ModelWriter {
		public:
			ModelWriter(const Graph& g, const Graph::const_node_iterator& model_node);

			void WriteHeader(std::ostream& ostr);

			void WriteCpp(std::ostream& ostr);

		protected:
			const Graph& _g;
			const Graph::const_node_iterator& _model_node;
			string _model_name;

			void WriteClassHeader(std::ostream& ostr, const Graph::const_node_iterator& dspec_node);
			void WriteCpp(std::ostream& ostr, const Graph::const_node_iterator& dspec_node);
	};
}

#endif
