#pragma once

#include "Object.h"


namespace Sapphire
{
	/// ֡��ʼ�¼�
	SAPPHIRE_EVENT(E_BEGINFRAME, BeginFrame)
	{
		SAPPHIRE_PARAM(P_FRAMENUMBER, FrameNumber);      // unsigned
		SAPPHIRE_PARAM(P_TIMESTEP, TimeStep);            // float
	}

	/// ���ڳ����߼������¼�
	SAPPHIRE_EVENT(E_UPDATE, Update)
	{
		SAPPHIRE_PARAM(P_TIMESTEP, TimeStep);            // float
	}

	/// Ӧ�ó����߼�������¼�
	SAPPHIRE_EVENT(E_POSTUPDATE, PostUpdate)
	{
		SAPPHIRE_PARAM(P_TIMESTEP, TimeStep);            // float
	}

	/// ��Ⱦ�����¼�
	SAPPHIRE_EVENT(E_RENDERUPDATE, RenderUpdate)
	{
		SAPPHIRE_PARAM(P_TIMESTEP, TimeStep);            // float
	}

	/// ������Ⱦ�����¼�
	SAPPHIRE_EVENT(E_POSTRENDERUPDATE, PostRenderUpdate)
	{
		SAPPHIRE_PARAM(P_TIMESTEP, TimeStep);            // float
	}

	/// ֡�����¼�
	SAPPHIRE_EVENT(E_ENDFRAME, EndFrame)
	{
	}
}