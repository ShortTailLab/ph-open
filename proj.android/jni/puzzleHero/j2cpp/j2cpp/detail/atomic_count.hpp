#ifndef J2CPP_DETAIL_ATOMIC_COUNT_HPP
#define J2CPP_DETAIL_ATOMIC_COUNT_HPP

#include <j2cpp/config.hpp>

#if __GNUC__ * 100 + __GNUC_MINOR__ >= 402
# include <ext/atomicity.h>
#else
# include <bits/atomicity.h>
#endif


namespace j2cpp { namespace detail {

#if defined(__GLIBCXX__) // g++ 3.4+
	using __gnu_cxx::__atomic_add;
	using __gnu_cxx::__exchange_and_add;
#endif

	class atomic_count
	{
	private:
		atomic_count(atomic_count const &);
		atomic_count & operator=(atomic_count const &);
	public:
		explicit atomic_count(long v): m_value(v) {}

		long operator++()
		{
			return __exchange_and_add(&m_value,+1)+1;
		}

		long operator--()
		{
			return __exchange_and_add(&m_value,-1)-1;
		}

		operator long() const
		{
			return __exchange_and_add(&m_value,0);
		}

	private:
		mutable _Atomic_word m_value;
	};

} //namespace detail
} //namespace j2cpp

#endif //J2CPP_DETAIL_ATOMIC_COUNT_HPP
