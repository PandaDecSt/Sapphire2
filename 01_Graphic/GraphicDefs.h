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
		FILL_POINT,
		MAX_FILLMODE
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

	//����Ѱַģʽ
	enum ETextureAddressingMode
	{
		EAD_WRAP = 0,
		EAD_MIRROR,
		EAD_CLAMP,
		EAD_BORDER,
		MAX_EAD_MODES
	};

	//��������
	enum ETextureCoordinate
	{
		ECOORD_U = 0,
		ECOORD_V,
		ECOORD_W,
		MAX_COORDS
	};

	//�����˲�ģʽ
	enum ETextureFilterMode
	{
		EFILTER_NEAREST = 0,
		FILTER_BILINEAR,
		FILTER_TRILINEAR,
		FILTER_ANISOTROPIC,
		FILTER_DEFAULT,
		MAX_EFILTERMODES
	};

	//����Ԫ����
	enum ETextureUnit
	{
		TU_DIFFUSE = 0,
		TU_ALBEDOBUFFER = 0,
		TU_NORMAL = 1,
		TU_NORMALBUFFER = 1,
		TU_SPECULAR = 2,
		TU_EMISSIVE = 3,
		TU_ENVIRONMENT = 4,
#ifdef SAPPHIRE_DESKTOP_GRAPHICS
		TU_VOLUMEMAP = 5,
		TU_CUSTOM1 = 6,
		TU_CUSTOM2 = 7,
		TU_LIGHTRAMP = 8,
		TU_LIGHTSHAPE = 9,
		TU_SHADOWMAP = 10,
		TU_FACESELECT = 11,
		TU_INDIRECTION = 12,
		TU_DEPTHBUFFER = 13,
		TU_LIGHTBUFFER = 14,
		TU_ZONE = 15,
		MAX_MATERIAL_TEXTURE_UNITS = 8,
		MAX_TEXTURE_UNITS = 16
#else
		TU_LIGHTRAMP = 5,
		TU_LIGHTSHAPE = 6,
		TU_SHADOWMAP = 7,
		MAX_MATERIAL_TEXTURE_UNITS = 5,
		MAX_TEXTURE_UNITS = 8
#endif
	};
}