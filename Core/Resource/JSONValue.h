#pragma once
#include "../Container/Vector.h"
#include "../Container/HashMap.h"
#include "Str.h"
#include "../Variant.h"
#include "../Context.h"

namespace Sapphire
{
	/// JSON ֵ����
	enum JSONValueType
	{
		 
		JSON_NULL = 0,
	 
		JSON_BOOL,
	 
		JSON_NUMBER,
		 
		JSON_STRING,
	 
		JSON_ARRAY,
		//���򼯺ϼ�ֵ��
		JSON_OBJECT,
	};

	/// JSON��������
	enum JSONNumberType
	{
		/// ����һ������
		JSONNT_NAN = 0,
		/// ����.
		JSONNT_INT,
		/// �޷�������
		JSONNT_UINT,
		/// �����Ȼ�˫���ȸ���
		JSONNT_FLOAT_DOUBLE,
	};

	class JSONValue;

	/// JSON ��������
	typedef Vector<JSONValue> JSONArray;
	/// JSON Object
	typedef HashMap<String, JSONValue> JSONObject;
	/// JSON object iterator.
	typedef JSONObject::Iterator JSONObjectIterator;
	/// Constant JSON object iterator.
	typedef JSONObject::ConstIterator ConstJSONObjectIterator;

	/// JSON value 
	class SAPPHIRE_API JSONValue
	{
	public:
		 
		JSONValue() :
			type_(0)
		{
		}
		 
		JSONValue(bool value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(int value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(unsigned value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(float value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(double value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(const String& value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(const char* value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(const JSONArray& value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(const JSONObject& value) :
			type_(0)
		{
			*this = value;
		}
		 
		JSONValue(const JSONValue& value) :
			type_(0)
		{
			*this = value;
		}
		 
		~JSONValue()
		{
			SetType(JSON_NULL);
		}

	 
		JSONValue& operator =(bool rhs);
		 
		JSONValue& operator =(int rhs);
		 
		JSONValue& operator =(unsigned rhs);
		 
		JSONValue& operator =(float rhs);
		 
		JSONValue& operator =(double rhs);
		 
		JSONValue& operator =(const String& rhs);
		 
		JSONValue& operator =(const char* rhs);
		 
		JSONValue& operator =(const JSONArray& rhs);
		 
		JSONValue& operator =(const JSONObject& rhs);
		 
		JSONValue& operator =(const JSONValue& rhs);

		 
		JSONValueType GetValueType() const;
		 
		JSONNumberType GetNumberType() const;
		 
		bool IsNull() const { return GetValueType() == JSON_NULL; }
	 
		bool IsBool() const { return GetValueType() == JSON_BOOL; }
		 
		bool IsNumber() const { return GetValueType() == JSON_NUMBER; }
		 
		bool IsString() const { return GetValueType() == JSON_STRING; }
		 
		bool IsArray() const { return GetValueType() == JSON_ARRAY; }
		 
		bool IsObject() const { return GetValueType() == JSON_OBJECT; }

		 
		bool GetBool() const { return IsBool() ? boolValue_ : false; }
		 
		int GetInt() const { return IsNumber() ? (int)numberValue_ : 0; }
		 
		unsigned GetUInt() const { return IsNumber() ? (unsigned)numberValue_ : 0; }
		 
		float GetFloat() const { return IsNumber() ? (float)numberValue_ : 0.0f; }
		 
		double GetDouble() const { return IsNumber() ? numberValue_ : 0.0; }
		 
		const String& GetString() const { return IsString() ? *stringValue_ : String::EMPTY; }
	 
		const char* GetCString() const { return IsString() ? stringValue_->CString() : 0; }
		 
		const JSONArray& GetArray() const { return IsArray() ? *arrayValue_ : emptyArray; }
		 
		const JSONObject& GetObject() const { return IsObject() ? *objectValue_ : emptyObject; }

		// JSON ����
		/// ����ָ������JSONֵ
		JSONValue& operator [](unsigned index);
		/// ����ָ������JSONֵ
		const JSONValue& operator [](unsigned index) const;
		/// ���JSON ֵ
		void Push(const JSONValue& value);
		/// �Ƴ������ӵ�JSON ֵ
		void Pop();
		/// Insert an JSON value at position.
		void Insert(unsigned pos, const JSONValue& value);
		/// Erase a range of JSON values.
		void Erase(unsigned pos, unsigned length = 1);
		/// Resize array.
		void Resize(unsigned newSize);
		/// Return size of array.
		unsigned Size() const;

		// JSON object functions
		/// Return JSON value with key.
		JSONValue& operator [](const String& key);
		/// Return JSON value with key.
		const JSONValue& operator [](const String& key) const;
		/// Set JSON value with key.
		void Set(const String& key, const JSONValue& value);
		/// Return JSON value with key.
		const JSONValue& Get(const String& key) const;
		/// Erase a pair by key.
		bool Erase(const String& key);
		/// Return whether contains a pair with key.
		bool Contains(const String& key) const;
		/// Return iterator to the beginning.
		JSONObjectIterator Begin();
		/// Return iterator to the beginning.
		ConstJSONObjectIterator Begin() const;
		/// Return iterator to the end.
		JSONObjectIterator End();
		/// Return iterator to the beginning.
		ConstJSONObjectIterator End() const;

		/// Clear array or object.
		void Clear();

		/// Set value type and number type, internal function.
		void SetType(JSONValueType valueType, JSONNumberType numberType = JSONNT_NAN);

		/// Set variant, context must provide for resource ref.
		void SetVariant(const Variant& variant, Context* context = 0);
		/// Return a variant.
		Variant GetVariant() const;
		/// Set variant value, context must provide for resource ref.
		void SetVariantValue(const Variant& variant, Context* context = 0);
		/// Return a variant with type.
		Variant GetVariantValue(VariantType type) const;
		/// Set variant map, context must provide for resource ref.
		void SetVariantMap(const VariantMap& variantMap, Context* context = 0);
		/// Return a variant map.
		VariantMap GetVariantMap() const;
		/// Set variant vector, context must provide for resource ref.
		void SetVariantVector(const VariantVector& variantVector, Context* context = 0);
		/// Return a variant vector.
		VariantVector GetVariantVector() const;

		/// Empty JSON value.
		static const JSONValue EMPTY;
		/// Empty JSON array.
		static const JSONArray emptyArray;
		/// Empty JSON object.
		static const JSONObject emptyObject;

	private:
		/// ����.
		unsigned type_;
		union
		{
			/// Boolean.
			bool boolValue_;
			/// ��ֵ
			double numberValue_;
			/// �ַ���
			String* stringValue_;
			/// ����
			JSONArray* arrayValue_;
			/// ����
			JSONObject* objectValue_;
		};
	};
}