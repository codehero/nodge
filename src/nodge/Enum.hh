#ifndef ENUM_STRUCT_H
#define ENUM_STRUCT_H

#include "Type.hh"

struct EnumValue{
	/** @brief Cpp name for the enumeration. uses std::string since its for internal use only. */
	const string cppname;
	/** @brief C++ value. uses std::string since its for internal use only. */
	const string value;
};

struct Enum{
	/** @brief Cpp name for the typedef enum{}. uses std::string since its for internal use only. */
	const string cppname;
	const Type& type;
	const EnumValue* const values;
	const unsigned num_values;
};
#endif
