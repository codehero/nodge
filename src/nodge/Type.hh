#ifndef __Type_h__
#define __Type_h__

#include "String.hh"

struct Type{

	/** @brief Enumeration of the possible data member types. */
	typedef enum{
		M_BOOL = 0
		,M_UNSIGNED_INT
		,M_INT
		,M_UNSIGNED_LONG
		,M_LONG
		,M_STRING
		,M_FLOAT
		,M_DOUBLE

		,NUM_TYPES
	} ID;

	/** @brief The associated Data::MemberType. */
	const string enum_name;

	/** @brief The C++ type. */
	const string cpp_type;
};

template<typename T>
	struct TypeToEnum{
		static const typename Type::ID enum_val;
	};

template class TypeToEnum<bool>;
template class TypeToEnum<unsigned>;
template class TypeToEnum<unsigned long>;
template class TypeToEnum<int>;
template class TypeToEnum<long>;
template class TypeToEnum<string>;
template class TypeToEnum<float>;
template class TypeToEnum<double>;

#endif
