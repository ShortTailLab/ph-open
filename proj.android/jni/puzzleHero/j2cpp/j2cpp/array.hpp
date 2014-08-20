#ifndef J2CPP_ARRAY_HPP
#define J2CPP_ARRAY_HPP

#include <j2cpp/config.hpp>
#include <j2cpp/environment.hpp>
#include <j2cpp/object.hpp>
#include <j2cpp/local_ref.hpp>
#include <j2cpp/get_class.hpp>

namespace j2cpp {

	template < typename Type, int Dim > 
	class array
	{
	};
	
	
	template < typename Type >
	struct array_access
	{
	};
	
	template < typename ValueType > 
	class array<ValueType, 1>
	: public object< array<ValueType, 1> >
	{
	public:
		typedef ValueType 						value_type;
		typedef object< array<value_type,1> >	base_type;
		
		explicit array(jobject obj)
		: base_type(obj)
		, m_is_copy(JNI_FALSE)
		, m_data(0)
		{
			m_data=array_access<value_type>::get_array_elements(
				base_type::get_jobject(), m_is_copy
			);
		}
		
		array(jsize s)
		: base_type(array_access<value_type>::new_array(s))
		, m_is_copy(JNI_FALSE)
		, m_data(0)
		{
			m_data=array_access<value_type>::get_array_elements(
				base_type::get_jobject(), m_is_copy
			);
		}
		
		template < typename SomeValueType >
		array(SomeValueType const *d, jsize s)
		: base_type(array_access<value_type>::new_array(s))
		, m_is_copy(JNI_FALSE)
		, m_data(0)
		{
			m_data=array_access<value_type>::get_array_elements(
				base_type::get_jobject(), m_is_copy
			);
			if(d) for(unsigned long i=0;i<s;++i) m_data[i]=value_type(d[i]);
		}

		template < typename SomeValueType, jsize S >
		array(SomeValueType(&arr)[S])
		: base_type(array_access<value_type>::new_array(S))
		, m_is_copy(JNI_FALSE)
		, m_data(0)
		{
			m_data=array_access<value_type>::get_array_elements(
				base_type::get_jobject(), m_is_copy
			);
			for(unsigned long i=0;i<S;++i) m_data[i]=value_type(arr[i]);
		}
		
		~array()
		{
			array_access<value_type>::release_array_elements(
				base_type::get_jobject(), m_data, 0
			);
		}
		
		value_type& operator [](unsigned long idx)
		{
			return m_data[idx];
		}

		value_type const& operator [](unsigned long idx) const
		{
			return m_data[idx];
		}

		value_type* data()
		{
			return m_data;
		}
		
		jsize length() const
		{
			return array_access<value_type>::get_array_length(
				base_type::get_jobject()
			);
		}
		
		jsize size() const
		{
			return array_access<value_type>::get_array_size(
				base_type::get_jobject()
			);
		}

	private:
		jboolean	m_is_copy;
		value_type	*m_data;
	};
	
	
	template < typename ObjectType >
	class array< local_ref<ObjectType>, 1>
	: public object< array< local_ref<ObjectType>, 1> >
	{
	public:
		typedef local_ref<ObjectType> 						value_type;
		typedef array< local_ref<ObjectType>, 1 >			array_type;
		typedef object< array< local_ref<ObjectType>, 1> >	base_type;
		

		class element_holder
		{
		public:
			explicit element_holder(array_type const &arr, unsigned long idx)
			: m_array(arr)
			, m_idx(idx)
			{
			}

			operator value_type () const
			{
				return m_array.get_at(m_idx);
			}

			element_holder& operator =(value_type const &v)
			{
				m_array.set_at(m_idx, v);
				return *this;
			}

		private:
			array_type		m_array;
			unsigned long	m_idx;
		};

		explicit array(jobject obj)
		: base_type(obj)
		{
		}

		array(jsize s, jobject initObj = 0)
		: base_type(array_access<value_type>::new_array(s, initObj))
		{
		}

		~array()
		{
		}

		element_holder operator [](unsigned long idx)
		{
			return element_holder(*this, idx);
		}

		element_holder const operator [](unsigned long idx) const
		{
			return element_holder(*this, idx);
		}

		jsize length() const
		{
			return array_access<value_type>::get_array_length(
				base_type::get_jobject()
			);
		}

		value_type get_at(unsigned long idx) const
		{
			return value_type(
				array_access< value_type >::get_object_array_element(
					base_type::get_jobject(),
					idx
				)
			);
		}

		void set_at(unsigned long idx, value_type const &v)
		{
			array_access< value_type >::set_object_array_element(
				base_type::get_jobject(),
				idx,
				v.get_jobject()
			);
		}
	};
	
	template <>
	struct array_access<jboolean>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jboolean);
		}

		static inline jboolean* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetBooleanArrayElements(
				reinterpret_cast<jbooleanArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewBooleanArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jboolean *data, jint mode)
		{
			environment::get().get()->jenv()->ReleaseBooleanArrayElements(
				reinterpret_cast<jbooleanArray>(jobj), data, mode
			);
		}
	};
	
	
	template <>
	struct array_access<jbyte>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jbyte);
		}

		static inline jbyte* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetByteArrayElements(
				reinterpret_cast<jbyteArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewByteArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jbyte *data, jint mode)
		{
			environment::get()->jenv()->ReleaseByteArrayElements(
				reinterpret_cast<jbyteArray>(jobj), data, mode
			);
		}
	};
	
	template <>
	struct array_access<jchar>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jchar);
		}

		static inline jchar* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetCharArrayElements(
				reinterpret_cast<jcharArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewCharArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jchar *data, jint mode)
		{
			environment::get()->jenv()->ReleaseCharArrayElements(
				reinterpret_cast<jcharArray>(jobj), data, mode
			);
		}
	};
	
	template <>
	struct array_access<jshort>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jshort);
		}

		static inline jshort* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetShortArrayElements(
				reinterpret_cast<jshortArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewShortArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jshort *data, jint mode)
		{
			environment::get()->jenv()->ReleaseShortArrayElements(
				reinterpret_cast<jshortArray>(jobj), data, mode
			);
		}
	};
	
	
	template <>
	struct array_access<jint>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jshort);
		}

		static inline jint* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetIntArrayElements(
				reinterpret_cast<jintArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewIntArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jint *data, jint mode)
		{
			environment::get()->jenv()->ReleaseIntArrayElements(
				reinterpret_cast<jintArray>(jobj), data, mode
			);
		}
	};
	
	
	template <>
	struct array_access<jlong>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jlong);
		}

		static inline jlong* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetLongArrayElements(
				reinterpret_cast<jlongArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewLongArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jlong *data, jint mode)
		{
			environment::get()->jenv()->ReleaseLongArrayElements(
				reinterpret_cast<jlongArray>(jobj), data, mode
			);
		}
	};
	
	template <>
	struct array_access<jfloat>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jfloat);
		}

		static inline jfloat* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetFloatArrayElements(
				reinterpret_cast<jfloatArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewFloatArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jfloat *data, jint mode)
		{
			environment::get()->jenv()->ReleaseFloatArrayElements(
				reinterpret_cast<jfloatArray>(jobj), data, mode
			);
		}
	};
	
	template <>
	struct array_access<jdouble>
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jsize get_array_size(jobject jobj)
		{
			return get_array_length(jobj)*sizeof(jdouble);
		}

		static inline jdouble* get_array_elements(jobject jobj, jboolean &is_copy)
		{
			return environment::get()->jenv()->GetDoubleArrayElements(
				reinterpret_cast<jdoubleArray>(jobj), &is_copy
			);
		}
		
		static inline jobject new_array(jsize s)
		{
			return environment::get()->jenv()->NewDoubleArray(s);
		}
		
		static inline void release_array_elements(jobject jobj, jdouble *data, jint mode)
		{
			environment::get()->jenv()->ReleaseDoubleArrayElements(
				reinterpret_cast<jdoubleArray>(jobj), data, mode
			);
		}
	};


	template <typename ObjectType>
	struct array_access< local_ref<ObjectType> >
	{
		static inline jsize get_array_length(jobject jobj)
		{
			return environment::get()->jenv()->GetArrayLength(
				reinterpret_cast<jarray>(jobj)
			);
		}

		static inline jobject get_object_array_element(jobject jobj, jsize idx)
		{
			return environment::get()->jenv()->GetObjectArrayElement(
				reinterpret_cast<jobjectArray>(jobj), idx
			);
		}

		static inline void set_object_array_element(jobject jobj, jsize idx, jobject value)
		{
			environment::get()->jenv()->SetObjectArrayElement(
				reinterpret_cast<jobjectArray>(jobj), idx, value
			);
		}

		static inline jobject new_array(jsize s, jobject initObj = 0)
		{
			return environment::get()->jenv()->NewObjectArray(
					s, get_object_class<ObjectType>(), initObj);
		}
	};

} //namespace j2cpp

#endif //J2CPP_ARRAY_HPP
