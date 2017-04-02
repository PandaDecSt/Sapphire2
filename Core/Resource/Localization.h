#pragma once

#include "../Context.h"
#include "JSONValue.h"


namespace Sapphire
{

	/// %���ػ���ϵͳ�������������Ե������ַ���
	class SAPPHIRE_API Localization : public Object
	{
		SAPPHIRE_OBJECT(Localization, Object);

	public:
		/// ������
		Localization(Context* context);
		/// �������ͷ�������Դ
		virtual ~Localization();

		/// �������Ե�����
		int GetNumLanguages() const { return (int)languages_.Size(); }

		/// ȡ�õ�ǰ���Ե������š� 
		int GetLanguageIndex() const { return languageIndex_; }

		/// ȡ�õ�ǰ���Ե������š�
		int GetLanguageIndex(const String& language);
		/// ���ص�ǰ���Ե�����
		String GetLanguage();
		/// ���ص�ǰ������
		String GetLanguage(int index);
		/// ���õ�ǰ����
		void SetLanguage(int index);
		/// ���õ�ǰ����
		void SetLanguage(const String& language);
		/// ����һ����ǰ���Ե��ַ��� ����String::EmptyΪ��
		String Get(const String& id);
		/// ������м��ص��ַ���
		void Reset();
		/// ��JSONValue�м����ַ���
		void LoadJSON(const JSONValue& source);
		/// ��JSON�ļ��м����ַ������ļ�Ӧ����UTF8����bom����
		void LoadJSONFile(const String& name);

	private:
		/// ������
		Vector<String> languages_;
		/// ��ǰ��������
		int languageIndex_;
		/// �洢�ַ��� <Language <StringId, Value> >.
		HashMap<StringHash, HashMap<StringHash, String> > strings_;
	};
}