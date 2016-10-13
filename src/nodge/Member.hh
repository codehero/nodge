#ifndef __MEMBER_H__
#define __MEMBER_H__

#include "Error.hh"
#include "Enum.hh"

typedef unsigned MemberType;

/** @brief MemberSpec describes a name-value pair. */
struct MemberSpec{

	/** @brief Unique ID in the scope of a group of members. */
	unsigned id;

	/** @brief The data type of the member. Must be of Type::M_* */
	MemberType type;

	/** @brief Unique human readable name in the scope of a group of members. */
	string name;

	/** @brief Listing of possible values of this member, if !NULL. */
	const Enum* _enum;

	/** @brief */
	MemberSpec(void) : id(0), type(Type::M_UNSIGNED_INT), name(""), _enum(NULL)
	{
	}

	/** @brief */
	MemberSpec(const unsigned& pID, const MemberType& ptype,
			const string& pname, const Enum* p_enum = NULL) :
		id(pID), type(ptype), name(pname), _enum(p_enum)
	{
	}
};

/** @brief */
class member_iterator{
	public:

		/** @brief */
		virtual void operator++() throw(exception) = 0;

		/** @brief */
		virtual const MemberSpec& GetSpec() const throw(exception) = 0;

		/** @brief */
		virtual void Get(bool& b) const throw(exception) = 0;

		/** @brief */
		virtual void Get(unsigned& u) const throw(exception) = 0;

		/** @brief */
		virtual void Get(unsigned long& ul) const throw(exception) = 0;

		/** @brief */
		virtual void Get(int& i) const throw(exception) = 0;

		/** @brief */
		virtual void Get(long& l) const throw(exception) = 0;

		/** @brief */
		virtual void Get(string& str) const throw(exception) = 0;

		/** @brief */
		virtual void Get(float& d) const throw(exception) = 0;

		/** @brief */
		virtual void Get(double& d) const throw(exception) = 0;

		/** @brief */
		virtual void Reset(void) noexcept = 0;

		/** @brief */
		bool AtEnd(void) const noexcept {
			return m_end;
		}

		/** @brief */
		virtual ~member_iterator() noexcept {
		}

		/** @brief Function for calling functors with operators (MemberSpec, T) . */
		template<typename F>
		void ForEach(F& f) {
			while(!AtEnd()){
				const MemberSpec& mspec = GetSpec();
				switch(mspec.type){
					case Type::M_BOOL:
						{
							bool b;
							Get(b);
							f(mspec, b);
						}
						break;

					case Type::M_UNSIGNED_INT:
						{
							unsigned u;
							Get(u);
							f(mspec, u);
						}
						break;

					case Type::M_INT:
						{
							int i;
							Get(i);
							f(mspec, i);
						}
						break;

					case Type::M_UNSIGNED_LONG:
						{
							unsigned long ul;
							Get(ul);
							f(mspec, ul);
						}
						break;

					case Type::M_LONG:
						{
							long l;
							Get(l);
							f(mspec, l);
						}
						break;

					case Type::M_STRING:
						{
							string s;
							Get(s);
							f(mspec, s);
						}
						break;

					case Type::M_FLOAT:
						{
							float fl;
							Get(fl);
							f(mspec, fl);
						}
						break;

					case Type::M_DOUBLE:
						{
							double d;
							Get(d);
							f(mspec, d);
						}
						break;

					default:
						break;
				}
				this->operator ++();
			}
		}

	protected:
		/** @brief */
		bool m_end;

		/** @brief */
		member_iterator(void) noexcept {
			/* descendant implementation must set m_end. */
		}
};

#endif
