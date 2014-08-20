#ifndef J2CPP_FIELD_HPP
#define J2CPP_FIELD_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/get_class.hpp>
#include <j2cpp/get_field_id.hpp>
#include <j2cpp/local_ref.hpp>

namespace j2cpp {

	template < typename Type >
	struct static_field_access
	{
	};

	template < typename Type >
	struct field_access
	{
	};

	template < const char *Class, const char *Name, const char *Signature, typename Type >
	class static_field
	{
	public:

		inline static_field& operator =(Type const &v)
		{
			static_field_access<Type>::set(
				get_class<Class>(),
				get_field_id<Class,Name,Signature,true>(),
				v.get_jtype()
			);
			return *this;
		}

		inline operator Type ()
		{
			return static_field_access<Type>::get(
				get_class<Class>(),
				get_field_id<Class,Name,Signature,true>()
			);
		}
	};

	template < const char *Class, const char *Name, const char *Signature, typename Type >
	class field
	{
	public:
		field(jobject jobj): m_jobject(jobj) {}

		template < typename AssignType >
		inline field& operator =(AssignType v)
		{
			field_access<Type>::set(
				m_jobject,
				get_field_id<Class,Name,Signature,false>(),
				v
			);
			return *this;
		}

		template < typename AssignType >
		inline field& operator =(local_ref<AssignType> const &v)
		{
			field_access<Type>::set(
				m_jobject,
				get_field_id<Class,Name,Signature,false>(),
				v
			);
			return *this;
		}

		template < typename CastType >
		inline operator CastType ()
		{
			return field_access<Type>::get(
				m_jobject,
				get_field_id<Class,Name,Signature,false>()
			);
		}

	private:
		jobject m_jobject;
	};


	template < typename Type >
	struct static_field_access< local_ref<Type> >
	{
		static void set(jclass c, jfieldID f, local_ref<Type> const &v)
		{
			environment::get()->jenv()->SetStaticObjectField(c, f, v.get_jobject());
		}

		static local_ref<Type> get(jclass c, jfieldID f)
		{
			return local_ref<Type>(
				environment::get()->jenv()->GetStaticObjectField(c, f)
			);
		}
	};

	template < typename Type >
	struct field_access< local_ref<Type> >
	{
		static void set(jobject o, jfieldID f, local_ref<Type> const &v)
		{
			environment::get()->jenv()->SetObjectField(o, f, v.get_jobject());
		}

		static local_ref<Type> get(jobject o, jfieldID f)
		{
			return local_ref<Type>(
				environment::get()->jenv()->GetObjectField(o, f)
			);
		}
	};

	template <>
	struct static_field_access<jboolean>
	{
		static void set(jclass c, jfieldID f, jboolean v)
		{
			environment::get()->jenv()->SetStaticBooleanField(c,f,v);
		}

		static jboolean get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticBooleanField(c,f);
		}
	};

	template <>
	struct static_field_access<jbyte>
	{
		static void set(jclass c, jfieldID f, jbyte v)
		{
			environment::get()->jenv()->SetStaticByteField(c,f,v);
		}

		static jbyte get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticByteField(c,f);
		}
	};

	template <>
	struct static_field_access<jchar>
	{
		static void set(jclass c, jfieldID f, jchar v)
		{
			environment::get()->jenv()->SetStaticCharField(c,f,v);
		}

		static jchar get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticCharField(c,f);
		}
	};

	template <>
	struct static_field_access<jdouble>
	{
		static void set(jclass c, jfieldID f, jdouble v)
		{
			environment::get()->jenv()->SetStaticDoubleField(c,f,v);
		}

		static jdouble get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticDoubleField(c,f);
		}
	};


	template <>
	struct static_field_access<jfloat>
	{
		static void set(jclass c, jfieldID f, jfloat v)
		{
			environment::get()->jenv()->SetStaticFloatField(c,f,v);
		}

		static jfloat get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticFloatField(c,f);
		}
	};

	template <>
	struct static_field_access<jint>
	{
		static void set(jclass c, jfieldID f, jint v)
		{
			environment::get()->jenv()->SetStaticIntField(c,f,v);
		}

		static jint get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticIntField(c,f);
		}
	};

	template <>
	struct static_field_access<jlong>
	{
		static void set(jclass c, jfieldID f, jlong v)
		{
			environment::get()->jenv()->SetStaticLongField(c,f,v);
		}

		static jlong get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticLongField(c,f);
		}
	};

	template <>
	struct static_field_access<jshort>
	{
		static void set(jclass c, jfieldID f, jshort v)
		{
			environment::get()->jenv()->SetStaticShortField(c,f,v);
		}

		jshort get(jclass c, jfieldID f)
		{
			return environment::get()->jenv()->GetStaticShortField(c,f);
		}
	};

	//fields
	template <>
	struct field_access<jboolean>
	{
		static void set(jobject o, jfieldID f, jboolean v)
		{
			environment::get()->jenv()->SetBooleanField(o,f,v);
		}

		static jboolean get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetBooleanField(o,f);
		}
	};

	template <>
	struct field_access<jbyte>
	{
		static void set(jobject o, jfieldID f, jbyte v)
		{
			environment::get()->jenv()->SetByteField(o,f,v);
		}

		static jbyte get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetByteField(o,f);
		}
	};

	template <>
	struct field_access<jchar>
	{
		static void set(jobject o, jfieldID f, jchar v)
		{
			environment::get()->jenv()->SetCharField(o,f,v);
		}

		static jchar get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetCharField(o,f);
		}
	};

	template <>
	struct field_access<jdouble>
	{
		static void set(jobject o, jfieldID f, jdouble v)
		{
			environment::get()->jenv()->SetDoubleField(o,f,v);
		}

		static jdouble get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetDoubleField(o,f);
		}
	};


	template <>
	struct field_access<jfloat>
	{
		static void set(jobject o, jfieldID f, jfloat v)
		{
			environment::get()->jenv()->SetFloatField(o,f,v);
		}

		static jfloat get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetFloatField(o,f);
		}
	};

	template <>
	struct field_access<jint>
	{
		static void set(jobject o, jfieldID f, jint const &v)
		{
			environment::get()->jenv()->SetIntField(o,f,v);
		}

		static jint get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetIntField(o,f);
		}
	};

	template <>
	struct field_access<jlong>
	{
		static void set(jobject o, jfieldID f, jlong const &v)
		{
			environment::get()->jenv()->SetLongField(o,f,v);
		}

		static jlong get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetLongField(o,f);
		}
	};

	template <>
	struct field_access<jshort>
	{
		static void set(jobject o, jfieldID f, jshort v)
		{
			environment::get()->jenv()->SetShortField(o,f,v);
		}

		jshort get(jobject o, jfieldID f)
		{
			return environment::get()->jenv()->GetShortField(o,f);
		}
	};



} //namespace j2cpp

#define J2CPP_DECLARE_FIELD(id)\
		static const char _j2cpp_field_name_##id[];\
		static const char _j2cpp_field_signature_##id[];

#define J2CPP_FIELD_NAME(id) _j2cpp_field_name_##id
#define J2CPP_FIELD_SIGNATURE(id) _j2cpp_field_signature_##id

#define J2CPP_DEFINE_FIELD(fqc, id, name, signature)\
		const char fqc::_j2cpp_field_name_##id[]=name;\
		const char fqc::_j2cpp_field_signature_##id[]=signature;

#endif //J2CPP_STATIC_FIELD_HPP
