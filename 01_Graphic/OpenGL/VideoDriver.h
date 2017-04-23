#pragma once
#include "Object.h"
#include "Mutex.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "../GraphicDefs.h"
#include "GLGPUObject.h"
#include "../ShaderManager.h"
#include "../Shader.h"

namespace Sapphire
{
	typedef GLFWwindow WindowsHandle;
	class Texture;
	class Geometry;
	class XMLFile;
	class XMLElement;
 
	class  SAPPHIRE_API  VideoDriver : public Object
	{
		SAPPHIRE_OBJECT(VideoDriver, Object);

	public:

		VideoDriver(Context* context);
		~VideoDriver();

		void  Initialize();
		void  SetWindow(WindowsHandle* window);
		void  SetWindowPos(WindowsHandle* window, IntVector2& pos);
		WindowsHandle*  GetWindow();
		void   SetShaderManager(Sapphire::ShaderManager* manager);
		ShaderManager*  GetShaderManager();
		ETextureFilterMode  GetDefaultTextureFilterMode();
		void   SetTextureParametersDirty();
		bool GetAnisotropySupport() const { return m_bAnisotropySupport; }
		unsigned GetTextureAnisotropy() const { return m_textureAnisotropy; }
		void SetTextureAnisotropy(unsigned level);
		void SetDefaultTextureFilterMode(ETextureFilterMode mode);

		void   BeginFrame();
		void   render();
		void   EndFrame();

		void   prepareDraw();
		void   Draw(EPRIMITIVE_TYPE type, Geometry* geo);
		bool   IsInitialzed();
		void   release();
		int    getWidth();
		void   setWidth(int width);
		void   setClearColor(Color c);
		Color  getClearColor();
		void   setWindowName(String name);
		String getWindowName();
		void   setVBO(unsigned obj);
		void   setVAO(unsigned  obj);
		void   setEBO(unsigned  obj);

		//�Ƿ�֧��OpenGL3.0
		static bool   getGL3Support(){ return m_gl3support; };

	private:

		void createWindow();
		//�������� 
		void setTexture(int index, Texture* tex);

		SharedPtr<ShaderManager>  mShaderManager;
		WindowsHandle*  mWindowHandle;
		int  m_windth;
		int  m_height;
		bool      m_fullScreen;
		bool      m_canResize;
		Color     m_clearColor;
		String    m_windowName;
		//��Ⱦʹ�õ�����
		Texture*  m_textures[SAPPHIRE_MAX_TEXTURE_UNIT];
		//�������������
		int       m_activeTexture;
		//��������
		unsigned  m_textrueTypes[SAPPHIRE_MAX_TEXTURE_UNIT];
		/// GPU ����
		PODVector<GPUObject*>  m_gpuobjects;
		Mutex                 m_gpuobjectsMutex;
		bool                  m_bAnisotropySupport;
		unsigned              m_textureAnisotropy;
		ETextureFilterMode       m_defaultfilterMode;
		bool                  m_isInitialized;

		unsigned              m_Vbo;
		unsigned              m_Vao;
		unsigned              m_Ebo;

		//OpenGL3.0֧�ֱ�־
		static bool      m_gl3support;

	private:

		//��ȡXML�����ļ�
		void LoadConfig(XMLFile* xmlFile);
		void LoadConfigElement(XMLElement* element);

	};
}
