#include "VideoDriver.h"
#include "Context.h"
#include "Color.h"
#include "../ITexture.h"

Sapphire::VideoDriver::VideoDriver(Context * context) :Object(context)
{
}

Sapphire::VideoDriver::~VideoDriver()
{
}

bool Sapphire::VideoDriver::m_gl3support = false;

void Sapphire::VideoDriver::Initialize()
{
	//glfwInit����ʼ��GLFW��Ȼ�����ǿ���ʹ��glfwWindowHint������GLFW��
	glfwInit();
	//glfwWindowHint�ĵ�һ�������������ǣ������ĸ�ѡ���Щѡ�����GLFW_ǰ׺���ڶ���������һ������������������Ϊѡ�������õ�ֵ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, SAPPHIRE_OPENGL_VERSION_MAJOR); //����GLFW����ʹ�õ�OpenGL�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, SAPPHIRE_OPENGL_VERSION_MIRROR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //����ͬ������GLFW������ϣ����ȷ��ʹ��core-profile
	if (m_canResize)
	{
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	}

	SetShaderManager(context_->GetSubsystem<ShaderManager>());

	


}

void Sapphire::VideoDriver::SetWindow(WindowsHandle * window)
{
	if (window)
	{
		glfwMakeContextCurrent(window);
		if (mWindowHandle)
		{
			glfwDestroyWindow(mWindowHandle);
		}
		mWindowHandle = window;
	}
}

Sapphire::WindowsHandle * Sapphire::VideoDriver::GetWindow()
{
	return mWindowHandle==NULL? mWindowHandle:NULL;
}

void Sapphire::VideoDriver::SetShaderManager(Sapphire::ShaderManager * manager)
{
	if (manager)
	{
		if (mShaderManager.NotNull())
		{
			mShaderManager.Reset();
		}
		mShaderManager = manager;

	}
}

Sapphire::ShaderManager * Sapphire::VideoDriver::GetShaderManager()
{
	return mShaderManager.NotNull()?mShaderManager.Get():NULL;
}

Sapphire::ETextureFilterMode Sapphire::VideoDriver::GetDefaultTextureFilterMode()
{
	return ETextureFilterMode::EFILTER_NEAREST;
}

void Sapphire::VideoDriver::SetTextureParametersDirty()
{
	MutexLock lock(m_gpuobjectsMutex);
	for (PODVector<GPUObject*>::Iterator it = m_gpuobjects.Begin(); it != m_gpuobjects.End(); ++it)
	{
		Texture* texture = dynamic_cast<Texture*>(*it);
		if (texture)
			texture->SetParametersDirty();   //������������������
	}
}

void Sapphire::VideoDriver::BeginFrame()
{
}

void Sapphire::VideoDriver::render()
{
}

void Sapphire::VideoDriver::createWindow()
{
}

void Sapphire::VideoDriver::EndFrame()
{
}

void Sapphire::VideoDriver::release()
{
	glfwTerminate();
}

int Sapphire::VideoDriver::getWidth()
{
	return m_windth;
}

void Sapphire::VideoDriver::setWidth(int width)
{
	m_windth = width;
}

void Sapphire::VideoDriver::setClearColor(Color c)
{
	m_clearColor = c;
}

Sapphire::Color Sapphire::VideoDriver::getClearColor()
{
	return m_clearColor;
}

void Sapphire::VideoDriver::setWindowName(String name)
{
	m_windowName = name;
}

Sapphire::String Sapphire::VideoDriver::getWindowName()
{
	return m_windowName;
}

void Sapphire::VideoDriver::setTexture(int index, Texture* tex)
{
	if (index > SAPPHIRE_MAX_TEXTURE_UNIT)
	{
		SAPPHIRE_LOGGING("texture index exceeded!");
		return;
	}
	
	//�����Ѵ���
	if (m_textures[index] != tex)
	{
		if (m_activeTexture != index)
		{
			//���������
			glActiveTexture(GL_TEXTURE0 + index);
			m_activeTexture = index;
		}

		if (tex)
		{
			//���������Ч
			unsigned glType = tex->GetTextureTarget();
			if (m_textrueTypes[index] && m_textrueTypes[index] != glType)
			{
				//ȡ��ԭ�󶨵�����
				glBindTexture(m_textrueTypes[index], 0);
			}
			//�����ڵ�����
			glBindTexture(glType, tex->GetGPUHandle());
			m_textrueTypes[index] = glType;

		}
		else if (m_textrueTypes[index])
		{
			//���ԭ��İ�
			glBindTexture(m_textrueTypes[index], 0);
			m_textrueTypes[index] = 0;
		}

	}
	else
	{
		if (tex && tex->GetTextureParametersDirty())
		{
			if (m_activeTexture != index)
			{    
				//�����µ�����
				glActiveTexture(GL_TEXTURE0 + index);
				m_activeTexture = index;
			}
			glBindTexture(m_textrueTypes[index], tex->GetGPUHandle());
		}
	}

}