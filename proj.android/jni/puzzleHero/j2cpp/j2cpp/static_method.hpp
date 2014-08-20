#ifndef J2CPP_STATIC_METHOD_HPP
#define J2CPP_STATIC_METHOD_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/jni_call_traits.hpp>
#include <j2cpp/get_method_id.hpp>
#include <j2cpp/get_class.hpp>

namespace j2cpp {

	template < typename Type >
	struct static_method { };

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type
	>
	typename jni_call_traits<Type>::return_type call_static_method()
	{
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), 0);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0))
		};
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1 
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1))
		};
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2 
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2))
		};
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3 
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3))
		};
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4 
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4))
		};
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}

	template
	<
		const char *Class, const char *Name, const char *Signature, typename Type,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12
	>
	typename jni_call_traits<Type>::return_type call_static_method(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12)
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
		return static_method<Type>::call(get_class<Class>(), get_method_id<Class,Name,Signature, true>(), args);
	}


	template < typename ObjectType >
	struct static_method< local_ref<ObjectType> >
	{
		static inline
		typename jni_call_traits< local_ref<ObjectType> >::return_type
		call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return typename jni_call_traits< local_ref<ObjectType> >::return_type(
				environment::get()->jenv()->CallStaticObjectMethodA(jclazz,sm,args)
			);
		}
	};

	template < >
	struct static_method< jboolean >
	{
		static inline
		jboolean call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticBooleanMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< jbyte >
	{
		static inline
		jbyte call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticByteMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< jchar >
	{
		static inline
		jchar call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticCharMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< jshort >
	{
		static inline
		jshort call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticShortMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< jint >
	{
		static inline
		jint call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticIntMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< jlong >
	{
		static inline
		jlong call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticLongMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< jfloat >
	{
		static inline
		jfloat call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticFloatMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< jdouble >
	{
		static inline
		jdouble call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			return environment::get()->jenv()->CallStaticDoubleMethodA(jclazz,sm,args);
		}
	};

	template < >
	struct static_method< void >
	{
		static inline
		void call(jclass jclazz, jmethodID sm, jvalue *args)
		{
			environment::get()->jenv()->CallStaticVoidMethodA(jclazz,sm,args);
		}
	};


} //namespace j2cpp


#endif //J2CPP_STATIC_METHOD_HPP
