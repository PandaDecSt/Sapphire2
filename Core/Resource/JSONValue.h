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
		/// ��ָ��λ�ò���һ��JSONֵ
		void Insert(unsigned pos, const JSONValue& value);
		/// ɾ��һ����Χ��JSONֵ
		void Erase(unsigned pos, unsigned length = 1);
		/// ������ֵ��С
		void Resize(unsigned newSize);
		/// ���ش�С
		unsigned Size() const;

		// JSON ������
		/// ����key��Ӧ��JSON ֵ
		JSONValue& operator [](const String& key);
		/// ����Key��Ӧ��JSONֵ
		const JSONValue& operator [](const String& key) const;
		/// ����JSONֵ
		void Set(const String& key, const JSONValue& value);
		/// ����JSONֵ
		const JSONValue& Get(const String& key) const;
		/// ɾ��һ��pair
		bool Erase(const String& key);
		/// �����Ƿ����key
		bool Contains(const String& key) const;
		/// ������ʼ������
		JSONObjectIterator Begin();
	 
		ConstJSONObjectIterator Begin() const;
	 
		JSONObjectIterator End();
		 
		ConstJSONObjectIterator End() const;

		/// �������
		void Clear();

		///����ֵ���ͺ���ֵ���� 
		void SetType(JSONValueType valueType, JSONNumberType numberType = JSONNT_NAN);

		/// ����variant context�����ṩ��Դ����
		void SetVariant(const Variant& variant, Context* context = 0);
		/// ����variant
		Variant GetVariant() const;
		/// ����variant��context�����ṩ��Դ����
		void SetVariantValue(const Variant& variant, Context* context = 0);
		/// ����variant����
		Variant GetVariantValue(VariantType type) const;
		/// ���� variant map.
		void SetVariantMap(const VariantMap& variantMap, Context* context = 0);
		/// ����variant map
		VariantMap GetVariantMap() const;
		/// ����variant ���飬 context�����ṩ��Դ����
		void SetVariantVector(const VariantVector& variantVector, Context* context = 0);
		/// ����variant ����
		VariantVector GetVariantVector() const;

		/// ��JSON
		static const JSONValue EMPTY;
		/// ��JSON����
		static const JSONArray emptyArray;
		/// ��JSON����
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