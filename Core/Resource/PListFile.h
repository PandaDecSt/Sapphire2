#pragma once


#include "../Resource/Resource.h"

namespace Sapphire
{

	class XMLElement;

	/// PList value ����.
	enum PListValueType
	{
		PLVT_NONE = 0,
		PLVT_INT,
		PLVT_BOOL,
		PLVT_FLOAT,
		PLVT_STRING,
		PLVT_VALUEMAP,
		PLVT_VALUEVECTOR,
	};

	class PListValue;

	/// PList value map.
	typedef HashMap<String, PListValue> PListValueMap;

	/// PList value����
	typedef Vector<PListValue> PListValueVector;

	/// PList value.
	class SAPPHIRE_API PListValue
	{
	public:
		 
		PListValue();
		 
		PListValue(int value);
		 
		PListValue(bool value);
		 
		PListValue(float value);
		 
		PListValue(const String& value);
		 
		PListValue(PListValueMap& valueMap);
		 
		PListValue(PListValueVector& valueVector);
		 
		PListValue(const PListValue& value);
		 
		~PListValue();

		 
		PListValue& operator =(const PListValue& rhs);

		/// �����Ƿ���Ч
		operator bool() const { return type_ != PLVT_NONE; }

	 
		void SetInt(int value);
	 
		void SetBool(bool value);
		 
		void SetFloat(float value);
		 
		void SetString(const String& value);
		 
		void SetValueMap(const PListValueMap& valueMap);
	 
		void SetValueVector(const PListValueVector& valueVector);

		/// ��������
		PListValueType GetType() const { return type_; }

		 
		int GetInt() const;
		 
		bool GetBool() const;
		 
		float GetFloat() const;
		 
		const String& GetString() const;
		 
		//��һ���ַ���ת��IntRect
		IntRect GetIntRect() const;
		//��һ���ַ���ת��IntVector
		IntVector2 GetIntVector2() const;
		 
		const PListValueMap& GetValueMap() const;
		 
		const PListValueVector& GetValueVector() const;

		/// ת����value map  
		PListValueMap& ConvertToValueMap();
		/// ת����value vector  
		PListValueVector& ConvertToValueVector();

	private:
		/// ����.
		void Reset();

		 
		PListValueType type_;
	 
		union
		{
			int int_;
			bool bool_;
			float float_;
			String* string_;
			PListValueMap* valueMap_;
			PListValueVector* valueVector_;
		};
	};

	/// Property list (plist).
	class SAPPHIRE_API PListFile : public Resource
	{
		SAPPHIRE_OBJECT(PListFile, Resource);

	public:
		 
		PListFile(Context* context);
		 
		virtual ~PListFile();
		/// ע����󹤳�
		static void RegisterObject(Context* context);

		/// �����м�����Դ�����Դӹ����߳��е��ã�����ɹ�����true
		virtual bool BeginLoad(Deserializer& source);

		/// ���ظ�
		const PListValueMap& GetRoot() const { return root_; }

	private:
		/// �����ֵ�
		bool LoadDict(PListValueMap& dict, const XMLElement& dictElem);
		/// ��������
		bool LoadArray(PListValueVector& array, const XMLElement& arrayElem);
		/// ����ֵ
		bool LoadValue(PListValue& value, const XMLElement& valueElem);

		/// ���ֵ� 
		PListValueMap root_;
	};
}