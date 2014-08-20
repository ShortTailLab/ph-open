#ifndef J2CPP_JNI_CALL_TRAITS_HPP
#define J2CPP_JNI_CALL_TRAITS_HPP

#include <j2cpp/config.hpp>

namespace j2cpp {

	template < typename Type > 
	struct jni_call_traits
	{
		typedef Type argument_type;
		typedef Type return_type;
	};
	
	template < typename ObjectType >
	struct jni_call_traits< local_ref<ObjectType> >
	{
		typedef jobject 				argument_type;
		typedef local_ref<ObjectType>	return_type;
	};

	template < typename Type >
	static inline typename jni_call_traits<Type>::argument_type jni_argument_cast(Type arg)
	{
		return typename jni_call_traits<Type>::argument_type(arg);
	}

	template < typename Type >
	static inline typename jni_call_traits<Type>::return_type jni_return_cast(Type ret)
	{
		return typename jni_call_traits<Type>::return_type(ret);
	}

	template < typename ObjectType >
	static inline typename jni_call_traits< local_ref<ObjectType> >::argument_type jni_argument_cast(local_ref<ObjectType> const &arg)
	{
		return arg.get_jobject();
	}

	template < typename ObjectType >
	static inline typename jni_call_traits< local_ref<ObjectType> >::return_type jni_return_cast(jobject ret)
	{
		return local_ref<ObjectType>(ret);
	}

	template < typename JniType >
	static inline jvalue jni_value(JniType);

	template <>
	jvalue jni_value(jboolean z)
	{
		jvalue _val; _val.z=z; return _val;
	}

	template <>
	jvalue jni_value(jbyte b)
	{
		jvalue _val; _val.b=b; return _val;
	}

	template <>
	jvalue jni_value(jchar c)
	{
		jvalue _val; _val.c=c; return _val;
	}

	template <>
	jvalue jni_value(jshort s)
	{
		jvalue _val; _val.s=s; return _val;
	}

	template <>
	jvalue jni_value(jint i)
	{
		jvalue _val; _val.i=i; return _val;
	}

	template <>
	jvalue jni_value(jlong j)
	{
		jvalue _val; _val.j=j; return _val;
	}

	template <>
	jvalue jni_value(jfloat f)
	{
		jvalue _val; _val.f=f; return _val;
	}

	template <>
	jvalue jni_value(jdouble d)
	{
		jvalue _val; _val.d=d; return _val;
	}

	template <>
	jvalue jni_value(jobject l)
	{
		jvalue _val; _val.l=l; return _val;
	}

} //namespace j2cpp


#endif //J2CPP_JNI_CALL_TRAITS_HPP
