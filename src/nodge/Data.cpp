#include "Data.hh"

Data::Spec::Spec(const string& pname, const MemberSpec* pmember_specs, unsigned pnum_members, DataCtor ctor) throw() :
	name(pname), member_specs(pmember_specs), num_members(pnum_members),
	data_ctor(ctor)
{
}

Data::Spec::~Spec(void) throw(){
}

const MemberSpec& Data::Spec::GetSpec(unsigned mid) const throw(invalid_input){
	/* Bounds check so we don't crash... */
	if((unsigned)mid >= num_members)
		throw parameter_error(string("Data::Spec::GetSpec():Invalid ID"), 0x1);
	return member_specs[mid];
}

const MemberSpec& Data::Spec::GetSpec(const string& name) const throw(invalid_input){
	/* TODO: horribly inefficient; replace with something later */
	for(unsigned i = 0; i < num_members; ++i){
		const MemberSpec& ms = member_specs[i];
		if(ms.name == name) return ms;
	}
	throw invalid_input(string("Data::Spec::GetSpec(): Invalid Name") + name);
}

struct Setter{
	template<typename T>
		void operator()(const MemberSpec& ms, const T& val){
			data->Set(ms, val);
		}

	Data* data;
};

bool Data::MemberEqual(const MemberSpec& member, const Data& other) const{
	switch(member.type){
		case Type::M_BOOL:
			return MemberEqualT<bool>(member, other);

		case Type::M_UNSIGNED_INT:
			return MemberEqualT<unsigned int>(member, other);

		case Type::M_INT:
			return MemberEqualT<int>(member, other);

		case Type::M_UNSIGNED_LONG:
			return MemberEqualT<unsigned long>(member, other);

		case Type::M_LONG:
			return MemberEqualT<long>(member, other);

		case Type::M_STRING:
			return MemberEqualT<string>(member, other);

		case Type::M_FLOAT:
			return MemberEqualT<float>(member, other);

		case Type::M_DOUBLE:
			return MemberEqualT<double>(member, other);

		default:
			return false;
	}
}

/** @brief Function for setting all the members of a Data object or conforming node containing data. */
void SetMembers(Data& data, member_iterator& j) throw(exception){
	Setter s;
	s.data = &data;
	j.ForEach<Setter>(s);
}

void Data::diff_iterator::operator++() throw(){
	do{
		++m_mem_idx;
		m_end = m_mem_idx >= spec.num_members;
	} while(!m_end && m_data.MemberEqual(GetSpec(), other));
}

Data::diff_iterator::diff_iterator(const Data& changes, const Data& reference, unsigned offset) :
	const_mem_iterator(changes, offset), other(reference)
{
	m_end = m_mem_idx >= spec.num_members;
	while(!m_end && m_data.MemberEqual(GetSpec(), other)){
		++m_mem_idx;
		m_end = m_mem_idx >= spec.num_members;
	}
}
