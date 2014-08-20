#ifndef J2CPP_SCOPED_ARRAY_HPP
#define J2CPP_SCOPED_ARRAY_HPP

#include <j2cpp/config.hpp>

namespace j2cpp {
	
	template < typename T >
	class scoped_array
	{
	private:
		scoped_array(scoped_array const &);
		scoped_array & operator=(scoped_array const &);

		void operator==( scoped_array const& ) const;
		void operator!=( scoped_array const& ) const;
		
	public:
		typedef T	element_type;
		typedef scoped_array<element_type> scoped_array_type;
		
		explicit scoped_array(element_type *p=0)
		: m_px(p)
		{
		}
		
		~scoped_array()
		{
			delete [] m_px;
		}

		void reset(element_type *p=0)
		{
			scoped_array_type(p).swap(*this);
		}
		
		element_type& operator[](unsigned long idx) const
		{
			return m_px[idx];
		}
		
		element_type* get() const
		{
			return m_px;
		}

		void swap(scoped_array &b) // never throws
		{
			element_type *tmp=b.m_px;
			b.m_px=m_px;
			m_px=tmp;
		}
	
	private:
		element_type	*m_px;
	};
	
	template < typename T > inline void swap(scoped_array<T> &a, scoped_array<T> &b)
	{
		a.swap(b);
	}

} //namespace j2cpp

#endif J2CPP_SCOPED_ARRAY_HPP