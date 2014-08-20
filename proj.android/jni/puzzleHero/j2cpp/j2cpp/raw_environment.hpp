#ifndef J2CPP_RAW_ENVIRONMENT_H
#define J2CPP_RAW_ENVIRONMENT_H

#include <j2cpp/environment.hpp>

namespace j2cpp {

	class raw_environment
		: public environment
	{
	public:
		raw_environment(JavaVM *jvm)
			: m_jvm(jvm)
		{
		}

		~raw_environment()
		{
		}

		virtual JNIEnv*	jenv()
		{
			if(!m_jvm) return 0;

			JNIEnv *je(0);
			if(JNI_OK!=m_jvm->GetEnv((void **)&je, JNI_VERSION_1_4))
				return 0;
			return je;
		}

		virtual jclass find_class(char const *cn)
		{
			if(JNIEnv *je=jenv())
				return je->FindClass(cn);
			return 0;
		}

		virtual jmethodID get_method_id(jclass c, char const *n, char const *s)
		{
			if(JNIEnv *je=jenv())
				return je->GetMethodID(c,n,s);
			return 0;
		}

		virtual jmethodID get_static_method_id(jclass c, char const *n, char const *s)
		{
			if(JNIEnv *je=jenv())
				return je->GetStaticMethodID(c,n,s);
			return 0;
		}

		virtual jfieldID get_field_id(jclass c, char const *n, char const *s)
		{
			if(JNIEnv *je=jenv())
				return je->GetFieldID(c,n,s);
			return 0;
		}

		virtual jfieldID get_static_field_id(jclass c, char const *n, char const *s)
		{
			if(JNIEnv *je=jenv())
				return je->GetStaticFieldID(c,n,s);
			return 0;
		}

		virtual bool exception_check()
		{
			if(JNIEnv *je=jenv())
				return je->ExceptionCheck()==JNI_TRUE;
			return JNI_FALSE;
		}

		virtual void exception_clear()
		{
			if(JNIEnv *je=jenv())
				je->ExceptionClear();
		}

		virtual void exception_describe()
		{
			if(JNIEnv *je=jenv())
				je->ExceptionDescribe();
		}

		virtual bool attach_current_thread()
		{
			if(!m_jvm)
				return false;

			JNIEnv *je(0);
			if(JNI_OK!=m_jvm->AttachCurrentThread(&je,0))
				return false;
			if(!je)
				return false;
			return true;
		}

		virtual bool detach_current_thread()
		{
			if(!m_jvm)
				return false;
			return (JNI_OK==m_jvm->DetachCurrentThread());
		}

	protected:
		JavaVM		*m_jvm;
	};

} //namespace j2cpp

#endif //J2CPP_RAW_ENVIRONMENT_H
