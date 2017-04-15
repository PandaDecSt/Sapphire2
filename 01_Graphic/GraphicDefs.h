#pragma once

namespace Sapphire
{
	//! ö��ͼԪ����
	// ��������ͼԪ�ķ���
	enum EPRIMITIVE_TYPE
	{
		//��������
		EPT_POINTS = 0,

		//���ж������ӳ�һ�������߶�
		EPT_LINE_STRIP,

		//!ͬ�ϣ�������λ���
		EPT_LINE_LOOP,

		//! ÿ������������ΪN/2���߶�
		EPT_LINES,

		//!  ��ǰ����������ÿ�����㹹��һ����������N-2
		EPT_TRIANGLE_STRIP,

		//!  ��ǰ����������ÿ�����㹹��һ����������N-2
		//! ���е������ζ����Ƶ�һ������
		EPT_TRIANGLE_FAN,

		//! �������ж��㵽ÿһ��������
		EPT_TRIANGLES,

		//! ÿ�ĸ����㹹��һ���ı���
		EPT_QUADS,

		//! ����һ��LINE_LOOP,�����
		EPT_POLYGON,

		//! ��GPU��һ��������չ��һ���ı��εĹ�����
		EPT_POINT_SPRITES
	};

	//���ģʽ
	enum FillMode
	{
		FILL_SOLID = 0,
		FILL_WIREFRAME,
		FILL_POINT
	};

	enum EVertexAttribute
	{
		EVA_NULL,
		EVA_POSITION,
		EVA_COLOR,
		EVA_NORMAL,
		EVA_TCOORD,
		EVA_2TCOORD,
		EVA_TANGENT,
		MAX_EVA
	};

	enum EVertexType
	{
		EVT_STARNDRD,
		EVT_COLOR,
		EVT_NORMAL,
		EVT_TCOORD,
		EVT_2TCOORD,
		EVT_TANGENTS,
		MAX_EVT
	};

	enum EIndexType
	{
		EIT_16BIT,
		EIT_32BIT
	};

	
}