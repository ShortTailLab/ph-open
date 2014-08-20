#ifndef J2CPP_GET_METHOD_HPP
#define J2CPP_GET_METHOD_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/get_class.hpp>

namespace j2cpp {

	template <bool>
	static jmethodID get_method_id(jclass, const char *, const char *);

	template <>
	jmethodID get_method_id<true>(jclass c, const char *n, const char *s)
	{
		return environment::get()->get_static_method_id(c,n,s);
	}

	template <>
	jmethodID get_method_id<false>(jclass c, const char *n, const char *s)
	{
		return environment::get()->get_method_id(c,n,s);
	}

	template < const char *Class, const char *Name, const char *Signature, bool is_static >
	static jmethodID get_method_id()
	{
		static jmethodID method_id=get_method_id<is_static>(
			get_class<Class>(),
			Name,
			Signature
		);
		return method_id;
	}

} //namespace j2cpp

#endif //J2CPP_GET_METHOD_HPP
