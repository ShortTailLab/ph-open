#ifndef J2CPP_GET_CLASS_HPP
#define J2CPP_GET_CLASS_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/jobject_global_ref.hpp>

namespace j2cpp {

	template < const char *ClassName >
	static jclass get_class()
	{
		static jobject_global_ref<jclass> clazz(environment::get()->find_class(ClassName));
		return clazz.get_jtype();
	}

	template < typename ObjectType >
	static jclass get_object_class()
	{
		return get_class< ObjectType::J2CPP_CLASS_NAME >();
	}

} //namespace j2cpp

#endif //J2CPP_GET_CLASS_HPP
