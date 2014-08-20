#ifndef J2CPP_GET_FIELD_ID_HPP
#define J2CPP_GET_FIELD_ID_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/get_class.hpp>

namespace j2cpp {

	template <bool>
	static jfieldID get_field_id(jclass, const char *, const char *);

	template <>
	jfieldID get_field_id<true>(jclass c, const char *n, const char *s)
	{
		return environment::get()->get_static_field_id(c,n,s);
	}

	template <>
	jfieldID get_field_id<false>(jclass c, const char *n, const char *s)
	{
		return environment::get()->get_field_id(c,n,s);
	}

	template < const char *Class, const char *Name, const char *Signature, bool is_static>
	static jfieldID get_field_id()
	{
		static jfieldID field_id=get_field_id<is_static>(
				get_class<Class>(),
				Name,
				Signature
		);
		return field_id;
	}


} //namespace j2cpp

#endif //J2CPP_GET_FIELD_ID_HPP

