#ifndef J2CPP_METHOD_HPP
#define J2CPP_METHOD_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/jni_call_traits.hpp>
#include <j2cpp/get_method_id.hpp>

namespace j2cpp {

	template < typename Type >
	struct method { };
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj)
	{
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), 0);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5)),
			jni_value(jni_argument_cast(a6))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5)),
			jni_value(jni_argument_cast(a6)),
			jni_value(jni_argument_cast(a7))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5)),
			jni_value(jni_argument_cast(a6)),
			jni_value(jni_argument_cast(a7)),
			jni_value(jni_argument_cast(a8))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5)),
			jni_value(jni_argument_cast(a6)),
			jni_value(jni_argument_cast(a7)),
			jni_value(jni_argument_cast(a8)),
			jni_value(jni_argument_cast(a9))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5)),
			jni_value(jni_argument_cast(a6)),
			jni_value(jni_argument_cast(a7)),
			jni_value(jni_argument_cast(a8)),
			jni_value(jni_argument_cast(a9)),
			jni_value(jni_argument_cast(a10))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5)),
			jni_value(jni_argument_cast(a6)),
			jni_value(jni_argument_cast(a7)),
			jni_value(jni_argument_cast(a8)),
			jni_value(jni_argument_cast(a9)),
			jni_value(jni_argument_cast(a10)),
			jni_value(jni_argument_cast(a11))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12
	>
	typename jni_call_traits<Type>::return_type call_method(jobject jobj, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4)),
			jni_value(jni_argument_cast(a5)),
			jni_value(jni_argument_cast(a6)),
			jni_value(jni_argument_cast(a7)),
			jni_value(jni_argument_cast(a8)),
			jni_value(jni_argument_cast(a9)),
			jni_value(jni_argument_cast(a10)),
			jni_value(jni_argument_cast(a11)),
			jni_value(jni_argument_cast(a12))
		};
		return method<Type>::call(jobj, get_method_id<Class,Name,Signature, false>(), args);
	}

	template < typename ObjectType >
	struct method< local_ref<ObjectType> >
	{
		static inline
		typename jni_call_traits< local_ref<ObjectType> >::return_type
		call(jobject jobj, jmethodID m, jvalue *args)
		{
			return typename jni_call_traits< local_ref<ObjectType> >::return_type(
				environment::get()->jenv()->CallObjectMethodA(jobj,m,args)
			);
		}
	};

	template < >
	struct method< jboolean >
	{
		static inline
		jboolean call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallBooleanMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< jbyte >
	{
		static inline
		jbyte call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallByteMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< jchar >
	{
		static inline
		jchar call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallCharMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< jshort >
	{
		static inline
		jshort call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallShortMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< jint >
	{
		static inline
		jint call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallIntMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< jlong >
	{
		static inline
		jlong call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallLongMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< jfloat >
	{
		static inline
		jfloat call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallFloatMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< jdouble >
	{
		static inline
		jdouble call(jobject jobj, jmethodID m, jvalue *args)
		{
			return environment::get()->jenv()->CallDoubleMethodA(jobj,m,args);
		}
	};

	template < >
	struct method< void >
	{
		static inline
		void call(jobject jobj, jmethodID m, jvalue *args)
		{
			environment::get()->jenv()->CallVoidMethodA(jobj,m,args);
		}
	};

} //namespace j2cpp

#endif //J2CPP_METHOD_HPP
