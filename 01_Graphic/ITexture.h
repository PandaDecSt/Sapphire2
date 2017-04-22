#pragma once
#include "Resource\Resource.h"
#include "IGPUObject.h"
#include <GL\glew.h>
#include "GraphicDefs.h"

namespace Sapphire
{
	class Image;

	static GLenum glWrapModes[] =
	{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
#ifndef GL_ES_VERSION_2_0
		GL_CLAMP
#else
		GL_CLAMP_TO_EDGE
#endif
	};

#ifndef GL_ES_VERSION_2_0
	static GLenum gl3WrapModes[] =
	{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER
	};
#endif

	//��ȡ����Ѱַģʽ
	static GLenum GetWrapMode(TextureAddressingMode mode);
	

	class SAPPHIRE_API Texture : public Resource, public GPUObject
	{
		SAPPHIRE_OBJECT(Texture, Resource);

	public:

		Texture(Context* ctx);
		virtual ~Texture();

		void SetSize(int width, int height, int depth = 32);
		void SetWidth(int width);
		void SetHeight(int height);
		void SetDepth(int depth);
		int GetDepth();
	    int GetWidth();
		int GetHeight();
		unsigned  GetTextureTarget();
		bool     GetTextureParametersDirty(){ return m_parametersDirty; }
		//�����������
		void   UpdateParameters();

	protected:
		//����Ŀ��GL_TEXTURE_2D/3D
		unsigned   m_target;
		int m_width;
		int m_height;
		int m_depth;
		//����������־
		bool  m_parametersDirty;
		//u/v/w����Ѱַģʽ
		TextureAddressingMode m_addressMode[TextureCoordinate::MAX_COORDS];

	private:

		
		


	};

}

