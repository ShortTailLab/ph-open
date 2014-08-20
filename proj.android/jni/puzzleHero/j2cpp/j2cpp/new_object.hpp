#ifndef J2CPP_NEW_OBJECT_HPP
#define J2CPP_NEW_OBJECT_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/jni_call_traits.hpp>
#include <j2cpp/get_method_id.hpp>
#include <j2cpp/get_class.hpp>

namespace j2cpp {

	template
	<
		const char *Class, const char *Name, const char *Signature
	>
	jobject call_new_object()
	{
		return environment::get()->jenv()->NewObjectA(
			get_class<Class>(), get_method_id<Class,Name,Signature,false>(), 0
		);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature,
		typename A0
	>
	jobject call_new_object(A0 a0)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0))
		};
		return environment::get()->jenv()->NewObjectA(
			get_class<Class>(), get_method_id<Class,Name,Signature,false>(), args
		);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature,
		typename A0, typename A1
	>
	jobject call_new_object(A0 a0, A1 a1)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1))
		};
		return environment::get()->jenv()->NewObjectA(
			get_class<Class>(), get_method_id<Class,Name,Signature,false>(), args
		);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature,
		typename A0, typename A1, typename A2
	>
	jobject call_new_object(A0 a0, A1 a1, A2 a2)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2))
		};
		return environment::get()->jenv()->NewObjectA(
			get_class<Class>(), get_method_id<Class,Name,Signature,false>(), args
		);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature,
		typename A0, typename A1, typename A2, typename A3
	>
	jobject call_new_object(A0 a0, A1 a1, A2 a2, A3 a3)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3))
		};
		return environment::get()->jenv()->NewObjectA(
			get_class<Class>(), get_method_id<Class,Name,Signature,false>(), args
		);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature,
		typename A0, typename A1, typename A2, typename A3, typename A4
	>
	jobject call_new_object(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
	{
		jvalue args[]=
		{
			jni_value(jni_argument_cast(a0)),
			jni_value(jni_argument_cast(a1)),
			jni_value(jni_argument_cast(a2)),
			jni_value(jni_argument_cast(a3)),
			jni_value(jni_argument_cast(a4))
		};
		return environment::get()->jenv()->NewObjectA(
			get_class<Class>(), get_method_id<Class,Name,Signature,false>(), args
		);
	}
	
	template
	<
		const char *Class, const char *Name, const char *Signature,
		typename A0, typename A1, typename A2, typename A3, typename A4, typename A5
	>
	jobject call_new_object(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
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
		return environment::get()->jenv()->NewObjectA(
			get_class<Class>(), get_method_id<Class,Name,Signature, false>(), args
		);
	}

} //namespace j2cpp


#endif //J2CPP_NEW_OBJECT_HPP
