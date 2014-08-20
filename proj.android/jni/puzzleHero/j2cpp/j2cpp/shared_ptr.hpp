#ifndef J2CPP_SHARED_PTR_HPP
#define J2CPP_SHARED_PTR_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/detail/atomic_count.hpp>
#include <j2cpp/detail/swap.hpp>

namespace j2cpp {

	template < typename T>
	class shared_ptr
	{
	private:
	    typedef detail::atomic_count count_type;

	public:
	    template < typename U > friend class shared_ptr;

	    typedef T element_type;
	    typedef T value_type;

	    explicit shared_ptr(T *p=0)
	    : m_px(p)
	    {
	    	m_pn=new count_type(1);
	        if(m_pn==0) delete p;
	    }

	    ~shared_ptr()
	    {
	        if(--*m_pn==0)
	        {
	            delete m_px;
	            delete m_pn;
	        }
	    }

	    shared_ptr(shared_ptr const &r)
	    : m_px(r.m_px)
	    , m_pn(r.m_pn)
	    {
	        ++*m_pn;
	    }

	    template < typename U >
	    shared_ptr(shared_ptr<U> const &r)
	    : m_px(static_cast<element_type*>(r.m_px))
	    , m_pn(r.m_pn)
	    {
	    	++*m_pn;
	    }

	    shared_ptr& operator=(shared_ptr const &r)
	    {
	        shared_ptr(r).swap(*this);
	        return *this;
	    }

	    void reset(T *p=0)
	    {
	        shared_ptr(p).swap(*this);
	    }

	    T& operator*() const
	    {
	    	return *m_px;
	    }

	    T* operator->() const
	    {
	        return m_px;
	    }

	    T* get() const
	    {
	        return m_px;
	    }

	    long use_count() const
	    {
	        return *m_pn;
	    }

	    bool unique() const
	    {
	        return *m_pn == 1;
	    }

	    void swap(shared_ptr<T> &other)
	    {
	        detail::swap(m_px, other.m_px);
	        detail::swap(m_pn, other.m_pn);
	    }

	    operator bool() const
		{
	    	return m_px?true:false;
	    }

	    bool operator !() const
	    {
	    	return m_px?false:true;
	    }

	private:
	    T			*m_px;
	    count_type	*m_pn;
	};

	template <typename T, typename U> inline bool operator==(shared_ptr<T> const &a, shared_ptr<U> const &b)
	{
		return a.get()==b.get();
	}

	template <typename T, typename U> inline bool operator!=(shared_ptr<T> const &a, shared_ptr<U> const &b)
	{
	    return a.get()!=b.get();
	}

	template <typename T> inline bool operator<(shared_ptr<T> const &a, shared_ptr<T> const &b)
	{
	    return (a.get()<b.get());
	}

	template <typename T> void swap(shared_ptr<T> &a, shared_ptr<T> &b)
	{
	    a.swap(b);
	}

	template <typename T> inline T* get_pointer(shared_ptr<T> const &p)
	{
	    return p.get();
	}

} //namespace j2cpp

#endif //J2CPP_SHARED_PTR_HPP
