#pragma once
#include "../Object.h"


namespace Sapphire
{
	/// ��Դ�ؼ��ؿ�ʼ
	SAPPHIRE_EVENT(E_RELOADSTARTED, ReloadStarted)
	{
	}

	/// ��Դ�ؼ������
	SAPPHIRE_EVENT(E_RELOADFINISHED, ReloadFinished)
	{
	}

	/// ��Դ�ؼ���ʧ��
	SAPPHIRE_EVENT(E_RELOADFAILED, ReloadFailed)
	{
	}

	/// ����ԴĿ¼�и��ٵ��ļ������ı�
	SAPPHIRE_EVENT(E_FILECHANGED, FileChanged)
	{
		SAPPHIRE_PARAM(P_FILENAME, FileName);                    // String
		SAPPHIRE_PARAM(P_RESOURCENAME, ResourceName);            // String
	}

	/// ��Դ����ʧ��
	SAPPHIRE_EVENT(E_LOADFAILED, LoadFailed)
	{
		SAPPHIRE_PARAM(P_RESOURCENAME, ResourceName);            // String
	}

	/// ��Դû���ҵ�
	SAPPHIRE_EVENT(E_RESOURCENOTFOUND, ResourceNotFound)
	{
		SAPPHIRE_PARAM(P_RESOURCENAME, ResourceName);            // String
	}

	/// δ֪����Դ����
	SAPPHIRE_EVENT(E_UNKNOWNRESOURCETYPE, UnknownResourceType)
	{
		SAPPHIRE_PARAM(P_RESOURCETYPE, ResourceType);            // StringHash
	}

	/// ��Դ��̨�������
	SAPPHIRE_EVENT(E_RESOURCEBACKGROUNDLOADED, ResourceBackgroundLoaded)
	{
		SAPPHIRE_PARAM(P_RESOURCENAME, ResourceName);            // String
		SAPPHIRE_PARAM(P_SUCCESS, Success);                      // bool
		SAPPHIRE_PARAM(P_RESOURCE, Resource);                    // Resource pointer
	}

	/// ���Ըı�
	SAPPHIRE_EVENT(E_CHANGELANGUAGE, ChangeLanguage)
	{
	}
}