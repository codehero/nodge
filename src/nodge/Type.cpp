#include "Type.hh"

template<> const Type::ID TypeToEnum<bool>::enum_val(Type::M_BOOL);
template<> const Type::ID TypeToEnum<unsigned>::enum_val(Type::M_UNSIGNED_INT);
template<> const Type::ID TypeToEnum<unsigned long>::enum_val(Type::M_UNSIGNED_LONG);
template<> const Type::ID TypeToEnum<long>::enum_val(Type::M_LONG);
template<> const Type::ID TypeToEnum<string>::enum_val(Type::M_STRING);
template<> const Type::ID TypeToEnum<float>::enum_val(Type::M_FLOAT);
template<> const Type::ID TypeToEnum<double>::enum_val(Type::M_DOUBLE);
