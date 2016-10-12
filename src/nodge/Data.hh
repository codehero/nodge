#ifndef DATACLASS_H
#define DATACLASS_H

#include "Enum.hh"
#include "Member.hh"

/*
 * @author David Bender
 * @date Nov 3 2005
 *
 * complete rewrite
*/

class Data{
	public:

		/** @brief Spec describes the derived Data class.
		 * Each descendant of Data will have a static const member of this type. */
		class Spec{
			public:
				/** @brief Human readable name summarizing content of data. */
				const string name;

				/** @brief Array of Specs for each member. */
				const MemberSpec * const member_specs;

				/** @brief Length of member_specs. */
				const unsigned num_members;

				typedef Data* (*DataCtor)(void);

				/** @brief Pointer to function that can instantiate object of derived Data class. */
				DataCtor data_ctor;

				/** @brief ctor. */
				Spec(const string& pname, const MemberSpec* pmember_specs,
						unsigned pnum_members, DataCtor ctor) throw();

				~Spec() throw();

				/** @brief Get the MemberSpec given the MemberSpec of the member. */
				const MemberSpec& GetSpec(unsigned id) const throw(invalid_input);

				/** @brief Get the MemberSpec given the name of the member. */
				const MemberSpec& GetSpec(const string& name) const throw(invalid_input);

				bool operator ==(const Spec& other) const throw(){
					/* Do pointer comparison. Assume that only one legal copy exists. */
					return (this == &other);
				}

				/** @brief Create an instance of this kind of data.
				 *  @return Pointer to instance allocated by new; uses default ctor. */
				Data* CreateInstance(void) const{
					return data_ctor();
				}
		};

		/** @brief member_iterator interface over a Data object. */
		class const_mem_iterator : public member_iterator{
			public:
				/* Overrides from member_iterator. */

				virtual void operator++() throw(exception){
					/* Increment index and set m_end flag. */
					++m_mem_idx;
					m_end = m_mem_idx >= spec.num_members;
				}

				const MemberSpec& GetSpec() const throw(exception){
					return spec.member_specs[m_mem_idx];
				}

				void Get(bool& b) const throw(exception){
					const bool* p;
					m_data.Get(GetSpec(), p);
					b = *p;
				}

				void Get(unsigned& u) const throw(exception){
					const unsigned* p;
					m_data.Get(GetSpec(), p);
					u = *p;
				}

				void Get(unsigned long& ul) const throw(exception){
					const unsigned long* p;
					m_data.Get(GetSpec(), p);
					ul = *p;
				}

				void Get(int& i) const throw(exception){
					const int* p;
					m_data.Get(GetSpec(), p);
					i = *p;
				}

				void Get(long& l) const throw(exception){
					const long* p;
					m_data.Get(GetSpec(), p);
					l = *p;
				}

				void Get(string& str) const throw(exception){
					const string* p;
					m_data.Get(GetSpec(), p);
					str = *p;
				}

				void Get(float& f) const throw(exception){
					const float* p;
					m_data.Get(GetSpec(), p);
					f = *p;
				}

				void Get(double& d) const throw(exception){
					const double* p;
					m_data.Get(GetSpec(), p);
					d = *p;
				}

				void Reset(void) throw(){
					m_mem_idx = 0;
					/* Just in case we have Data with zero members. */
					m_end = m_mem_idx >= spec.num_members;
				}

				const_mem_iterator(const Data& data, unsigned offset) throw() :
					m_data(data), spec(data.GetSpec()), m_mem_idx(offset)
				{
					m_end = m_mem_idx >= spec.num_members;
				}

				virtual ~const_mem_iterator() throw(){
				}

			protected:
				/** @brief Data over which we are iterating. */
				const Data& m_data;

				/** @brief convenience/cached spec of m_data. */
				const Data::Spec& spec;

				/** @brief current member index. */
				unsigned m_mem_idx;
		};

		/** @brief Get Specs for the derived Data's members. The one stop shop for anything you wanted to know about a Data class. */
		virtual const Spec& GetSpec(void) const throw() = 0;

		/** @brief Copy data from src to this. */
		virtual Data* Clone(void) const{
			/* That ought to crash shit. */
			return NULL;
		}

		/** @brief Determine if two members have the same value. */
		//bool MemberEqual(const MemberSpec& member, const Data& other) const throw();

		/** @brief Retrieve const pointer to specified boolean. */
		virtual void Get(const MemberSpec& id, const bool*& x) const throw(parameter_error){
			throw parameter_error(string("bool not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, bool b) throw(parameter_error){
			const bool* ptr;
			Get(ms, ptr);
			bool& a = const_cast<bool&>(*ptr);
			a = b;
		}

		/** @brief Retrieve const pointer to specified unsigned integer. */
		virtual void Get(const MemberSpec& id, const unsigned*& x) const throw(parameter_error){
			throw parameter_error(string("unsigned not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, unsigned b) throw(parameter_error){
			const unsigned* ptr;
			Get(ms, ptr);
			unsigned& a = const_cast<unsigned&>(*ptr);
			a = b;
		}

		/** @brief Retrieve const pointer to specified unsigned long. */
		virtual void Get(const MemberSpec& id, const unsigned long*& x) const throw(parameter_error){
			throw parameter_error(string("unsigned long not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, unsigned long b) throw(parameter_error){
			const unsigned long* ptr;
			Get(ms, ptr);
			unsigned long& a = const_cast<unsigned long&>(*ptr);
			a = b;
		}


		/** @brief Retrieve const pointer to specified signed integer. */
		virtual void Get(const MemberSpec& id, const int*& x) const throw(parameter_error){
			throw parameter_error(string("int not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, int b) throw(parameter_error){
			const int* ptr;
			Get(ms, ptr);
			int& a = const_cast<int&>(*ptr);
			a = b;
		}


		/** @brief Retrieve const pointer to specified long. */
		virtual void Get(const MemberSpec& id, const long*& x) const throw(parameter_error){
			throw parameter_error(string("long not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, long b) throw(parameter_error){
			const long* ptr;
			Get(ms, ptr);
			long& a = const_cast<long&>(*ptr);
			a = b;
		}


		/** @brief Retrieve const pointer to specified String. */
		virtual void Get(const MemberSpec& id, const string*& x) const throw(parameter_error){
			throw parameter_error(string("String not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, const string& b) throw(parameter_error){
			const string* ptr;
			Get(ms, ptr);
			string& a = const_cast<string&>(*ptr);
			a = b;
		}

		/** @brief Retrieve const pointer to specified String. */
		virtual void Get(const MemberSpec& id, const float*& x) const throw(parameter_error){
			throw parameter_error(string("float not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, const float& b) throw(parameter_error){
			const float* ptr;
			Get(ms, ptr);
			float& a = const_cast<float&>(*ptr);
			a = b;
		}

		/** @brief Retrieve const pointer to specified String. */
		virtual void Get(const MemberSpec& id, const double*& x) const throw(parameter_error){
			throw parameter_error(string("double not found: ") + id.name, 0x1);
		}

		void Set(const MemberSpec& ms, const double& b) throw(parameter_error){
			const double* ptr;
			Get(ms, ptr);
			double& a = const_cast<double&>(*ptr);
			a = b;
		}

		/** @brief Convenience template that directly returns type given the MemberSpec */
		template<typename T>
			const T& Get(const MemberSpec& ms) const throw(parameter_error){
				const T* ptr;
				Get(ms, ptr);
				return *ptr;
			}

		/** @brief Test equality between the same members of two different Data GIVEN the member type. */
		template<typename T>
			bool MemberEqualT(const MemberSpec& member, const Data& other) const{
				const T *a, *b;
				Get(member, a);
				other.Get(member, b);
				return *a == *b;
			}

		/** @brief Test equality between the same members of two different Data without knowing type. */
		bool MemberEqual(const MemberSpec& member, const Data& other) const;

		/** @brief Use this iterator to find things in changes that are different from a reference. */
		class diff_iterator : public const_mem_iterator{
			public:
				void operator++() throw();

				/** @brief */
				diff_iterator(const Data& changes, const Data& reference, unsigned offset);

			protected:
				/** @brief */
				const Data& other;
		};

		virtual ~Data() throw(){
		}
};

void SetMembers(Data& data, member_iterator& j) throw(exception);

#endif
