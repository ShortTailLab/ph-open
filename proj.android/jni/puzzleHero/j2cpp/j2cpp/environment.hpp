#ifndef J2CPP_ENVIRONMENT_HPP
#define J2CPP_ENVIRONMENT_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/shared_ptr.hpp>

namespace j2cpp {

	class environment
	{
	protected:
		environment& operator =(environment const &);

	private:
		static shared_ptr<environment>& g_env()
		{
			static shared_ptr<environment> _g_env;
			return _g_env;
		}

	public:
		
		virtual ~environment()
		{
		}

		static inline shared_ptr<environment> const& get()
		{
			return g_env();
		}

		static bool init(shared_ptr<environment> const &env)
		{
			if(!env || g_env())
				return false;

			g_env()=env;
			return true;
		}

		virtual JNIEnv*	jenv()=0;
		virtual jclass find_class(char const *cn)=0;
		virtual jmethodID get_method_id(jclass c, char const *n, char const *s)=0;
		virtual jmethodID get_static_method_id(jclass c, char const *n, char const *s)=0;
		virtual jfieldID get_field_id(jclass c, char const *n, char const *s)=0;
		virtual jfieldID get_static_field_id(jclass c, char const *n, char const *s)=0;
		virtual bool exception_check()=0;
		virtual void exception_clear()=0;
		virtual void exception_describe()=0;
		virtual bool attach_current_thread()=0;
		virtual bool detach_current_thread()=0;
	};

} //namespace j2cpp

#endif //J2CPP_ENVIRONMENT_HPP
