#ifndef J2CPP_DETAIL_SWAP_HPP
#define J2CPP_DETAIL_SWAP_HPP

#include <j2cpp/config.hpp>

namespace j2cpp { namespace detail {

	template < typename T >
	inline void swap(T& left, T& right)
	{
		if(&left!=&right)
		{
			T tmp=left;
			left=right;
			right=tmp;
		}
	}

} //namespace detail
} //namespace j2cpp

#endif //J2CPP_DETAIL_SWAP_HPP
