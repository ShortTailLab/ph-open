#ifndef J2CPP_OBJECT_H
#define J2CPP_OBJECT_H

#include <j2cpp/config.hpp>

namespace j2cpp {

	template < typename Derived >
	class object
	{
	public:
		typedef Derived derived_type;

		object(): m_jobject(0) { }
		object(jobject jobj): m_jobject(jobj) { }
		~object(){ }

		jobject get_jobject() const
		{
			return m_jobject;
		}
	
	protected:
		jobject	m_jobject;
	};

} //namespace j2cpp

#define J2CPP_CLASS_NAME _j2cpp_class_name

#define J2CPP_DECLARE_CLASS static const char _j2cpp_class_name[];
#define J2CPP_DECLARE_METHOD(id)\
		static const char _j2cpp_method_name_##id[];\
		static const char _j2cpp_method_signature_##id[];

#define J2CPP_METHOD_NAME(id) _j2cpp_method_name_##id
#define J2CPP_METHOD_SIGNATURE(id) _j2cpp_method_signature_##id

#define J2CPP_DEFINE_CLASS(fqc, name) char const fqc::_j2cpp_class_name[]=name;
#define J2CPP_DEFINE_METHOD(fqc, id, name, signature)\
		const char fqc::_j2cpp_method_name_##id[]=name;\
		const char fqc::_j2cpp_method_signature_##id[]=signature;

#endif //J2CPP_CPP_OBJECT_H
