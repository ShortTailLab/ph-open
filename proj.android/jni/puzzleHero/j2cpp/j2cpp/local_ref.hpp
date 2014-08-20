#ifndef J2CPP_LOCAL_REF_HPP
#define J2CPP_LOCAL_REF_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/detail/atomic_count.hpp>
#include <j2cpp/detail/swap.hpp>

namespace j2cpp {

	template < typename T > class global_ref;

	template < typename ObjType >
	class local_ref
	{
	public:
		template < typename T > friend class global_ref;

		typedef ObjType					object_type;
		typedef detail::atomic_count	count_type;

		typedef local_ref<ObjType>		value_type;

		local_ref()
		: m_px(0)
		{
			m_pn=new count_type(1);
		}


		explicit local_ref(jobject jobj)
		: m_px(0)
		, m_pn(0)
		{
			if(jobject lref=(jobj?
				environment::get()->jenv()->NewLocalRef(jobj):0)
			)
			{
				m_px=new object_type(lref);
			}
			m_pn=new count_type(1);
		}

		~local_ref()
		{
			if(--*m_pn==0)
			{
				if(m_px && m_px->get_jobject())
					environment::get()->jenv()->DeleteLocalRef(m_px->get_jobject());
				delete m_px;
				delete m_pn;
			}
		}

		local_ref(object_type const &obj)
		: m_px(0)
		, m_pn(0)
		{
			if(jobject lref=(obj.get_jobject()?
				environment::get()->jenv()->NewLocalRef(obj.get_jobject()):0)
			)
			{
				m_px=new object_type(lref);
			}
			m_pn=new count_type(1);
		}

		local_ref(local_ref const &that)
		: m_px(that.m_px)
		, m_pn(that.m_pn)
		{
			++*m_pn;
		}

		local_ref(global_ref<object_type> const &gref)
		: m_px(0)
		, m_pn(0)
		{
			if(jobject lref=(gref.get_jobject()?
				environment::get()->jenv()->NewLocalRef(gref.get_jobject()):0)
			)
			{
				m_px=new object_type(lref);
			}
			m_pn=new count_type(1);
		}

		template < typename OtherObject >
		operator local_ref<OtherObject> () const
		{
			return local_ref<OtherObject>(*m_px);
		}

		local_ref& operator=(local_ref const &rhs)
		{
			local_ref(rhs).swap(*this);
			return *this;
		}

		object_type* operator->() const
		{
			return m_px;
		}

		object_type& operator *() const
		{
			return *m_px;
		}


		jobject get_jobject() const
		{
			return (m_px?m_px->get_jobject():0);
		}

		bool operator !() const
		{
			return m_px==0;
		}

		operator bool() const
		{
			return m_px!=0;
		}

		void swap(local_ref &rhs)
		{
			detail::swap(m_px, rhs.m_px);
			detail::swap(m_pn, rhs.m_pn);
		}

	private:
		object_type	*m_px;
		count_type	*m_pn;
	};

} //namespace j2cpp

#endif //J2CPP_LOCAL_REF_HPP
