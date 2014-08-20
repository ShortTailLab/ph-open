#ifndef J2CPP_JOBJECT_GLOBAL_REF_HPP
#define J2CPP_JOBJECT_GLOBAL_REF_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>

namespace j2cpp {

	template < typename T > class jobject_local_ref;

	template< typename JObjType >
	class jobject_global_ref
	{
	public:
		typedef JObjType jobject_type;

		template < typename T > friend class jobject_local_ref;

		jobject_global_ref(jobject_type jobj)
		: m_object(jobj?
			reinterpret_cast<jobject_type>(environment::get()->jenv()->NewGlobalRef(jobj)):
			0
		)
		{
		}

		jobject_global_ref(jobject_global_ref const &that)
		: m_object(that.m_object?
			reinterpret_cast<jobject_type>(environment::get()->jenv()->NewGlobalRef(that.m_object)):
			0
		)
		{
		}

		jobject_global_ref(jobject_local_ref<jobject_type> const &that)
		: m_object(that.m_object?
			reinterpret_cast<jobject_type>(environment::get()->jenv()->NewGlobalRef(that.m_object)):
			0
		)
		{
		}

		~jobject_global_ref()
		{
			if(m_object)
				environment::get()->jenv()->DeleteGlobalRef(m_object);
		}

		operator bool () const { return m_object?true:false; }
		jobject_type get_jtype() const { return  m_object; }

	private:
		jobject_type	m_object;
	};

} //namespace j2cpp

#endif //J2CPP_JOBJECT_GLOBAL_REF_HPP
