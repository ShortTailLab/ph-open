#ifndef J2CPP_DETAIL_SPINLOCK_HPP
#define J2CPP_DETAIL_SPINLOCK_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/detail/yield_k.hpp>

namespace j2cpp { namespace detail {

	class spinlock
	{
	public:
		inline bool try_lock()
		{
			_GLIBCXX_WRITE_MEM_BARRIER;
			if(*const_cast<int volatile*>(&m_v))
				return false;
			*const_cast<int volatile*>(&m_v)=1;
			return true;
		}

		inline void lock()
		{
			for(unsigned k=0;!try_lock();++k)
				detail::yield(k);
		}

		
		inline void unlock()
		{
			_GLIBCXX_WRITE_MEM_BARRIER;
			*const_cast<int volatile*>(&m_v)=0;
		}
	
	public:
		spinlock(): m_v(0) {}

		class scoped_lock
		{
		private:
			scoped_lock(scoped_lock const &);
			scoped_lock& operator=(scoped_lock const&);
			
		public:
			explicit scoped_lock(spinlock &sp): m_sp(sp)
			{
				sp.lock();
			}
	
			~scoped_lock()
			{
				m_sp.unlock();
			}
		private:
			spinlock &m_sp;
		};
		
	private:
		int m_v;
	};
	
} //namespace detail
} //namespace j2cpp

#endif //J2CPP_DETAIL_SPINLOCK_HPP 
