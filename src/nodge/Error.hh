#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include "String.hh"

using std::exception;
using std::runtime_error;
using std::logic_error;

/* Runtime errors. */

/** @brief Thrown when converting to/from a type.
 *  Only meant for conversion of one particular datum.
 *  More complex conversions fall under the domain of parsing. */
class conversion_error : public runtime_error{
	public:
		conversion_error(const char* src, const char* dest, const string& str) noexcept;
		~conversion_error() noexcept;

};

/** @brief An error occurred in parsing an input format to an internal representation.
 *  Note this is NOT for individual pieces; rather it is meant for errors in the structure of the input itself. */
class parser_error : public runtime_error{
	public:
		parser_error(const string& str) noexcept;
		~parser_error() noexcept;

};

/** @brief Thrown when the structure of an input format is ok, but the data itself is the error. */
class invalid_input : public runtime_error{
	public:
		invalid_input(const string& str) noexcept;
		~invalid_input() noexcept;
};

/** @brief Any error in discovering a misconfiguration of for this executable. */
class config_error : public runtime_error{
	public:
		config_error(const string& str) noexcept;
		~config_error() noexcept;
};

/* Logic errors. */

/** @brief Error in specifying a parameter to a function.
 *  Every function that takes a constrained parameter in theory should throw this.
 *  Note that these are LOGIC errors...they indicate bugs in programming. */
class parameter_error : public logic_error{
	public:
		parameter_error(const string& pdesc, unsigned pmask) noexcept;
		~parameter_error() noexcept;

		/** @brief bitmask of bad parameters. */
		const unsigned m_mask;
};


#endif
