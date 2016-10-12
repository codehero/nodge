#include "Error.hh"

conversion_error::conversion_error(const char* src, const char* dest, const string& str) throw() :
	runtime_error(string("Error converting") + src + " to " + dest + str)
{
}

conversion_error::~conversion_error(void) throw(){
}



parser_error::parser_error(const string& str) throw() :
	runtime_error(str)
{
}

parser_error::~parser_error() throw(){
}



invalid_input::invalid_input(const string& str) throw() :
	runtime_error(str)
{
}

invalid_input::~invalid_input() throw(){
}



/** @brief Any error in discovering a misconfiguration of for this executable. */

config_error::config_error(const string& str) throw() :
	runtime_error(str)
{
}

config_error::~config_error() throw(){
}



parameter_error::parameter_error(const string& pdesc, unsigned pmask) throw() :
	logic_error(pdesc), m_mask(pmask)
{
}

parameter_error::~parameter_error() throw(){
}
