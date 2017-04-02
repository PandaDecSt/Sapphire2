#pragma once

#include "Resource.h"
#include "JSONValue.h"

namespace Sapphire
{
	/// JSON �ĵ���Դ
	class SAPPHIRE_API JSONFile : public Resource
	{
		SAPPHIRE_OBJECT(JSONFile, Resource);

	public:
		 
		JSONFile(Context* context);
		 
		virtual ~JSONFile();
		/// ע����󹤳�
		static void RegisterObject(Context* context);

		/// �����м�����Դ�����Դӹ����̵߳��á�����ɹ�����true
		virtual bool BeginLoad(Deserializer& source);
		/// ��Ĭ�ϵ�����������Դ������ɹ�����true
		virtual bool Save(Serializer& dest) const;
		/// ���û����������������Դ
		bool Save(Serializer& dest, const String& indendation) const;

		/// ����root
		JSONValue& GetRoot() { return root_; }
		/// ����root
		const JSONValue& GetRoot() const { return root_; }

	private:
		/// JSON root value.
		JSONValue root_;
	};

}