#ifndef J2CPP_DETAIL_YIELD_K_HPP
#define J2CPP_DETAIL_YIELD_K_HPP

#include <j2cpp/config.hpp>

namespace j2cpp { namespace detail {

	inline void yield(unsigned k)
	{
		if(k<4)
		{
		}
		else
		if(k<32 || k&1)
		{
			sched_yield();
		}
		else
		{
			struct timespec rqtp={0,0};
			rqtp.tv_sec=0;
			rqtp.tv_nsec=1000;
			nanosleep(&rqtp,0);
		}
    }

} //namespace detail
} //namepsace boost

#endif //J2CPP_DETAIL_YIELD_K_HPP
