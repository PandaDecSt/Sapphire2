#include "stdafx.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "ShaderManager.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VideoDriver.h"
#include "ITexture.h"


void Sapphire::OpenGLVideoDriver::drawLine(const Sapphire::Line3d & line, const char* shaderName)
{
	Sapphire::Line3d l = line;
	ShaderStruct* shader = m_pShaderManager->FindShader(shaderName);
	if (!shader || !shader->ShaderProgram || !shader->isCompiled)
	{
		return;
	}

	GLfloat vertices[] =
	{
		l.getStart().x_,l.getStart().y_, l.getStart().z_,l.getColor().r_,l.getColor().g_,l.getColor().b_,l.getColor().a_,
		l.getEnd().x_,l.getEnd().y_, l.getEnd().z_,l.getColor().r_,l.getColor().g_,l.getColor().b_,l.getColor().a_
	};

	ulong size = sizeof(vertices);

	//创建VBO缓冲区
	GLuint vbo;
	glGenBuffers(1, &vbo);
	GLuint vao;
	//创建VAO
	glGenVertexArrays(1, &vao);
	GLuint ebo;
	//创建EBO对象
	glGenBuffers(1, &ebo);
	//设置VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//设置缓冲区数据
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	//设置VAO
	{
		glBindVertexArray(vao);
		//设置顶点属性结构
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)0);
		//启用顶点属性数组（index < MAX_VERTEX_ATTRBS-1）
		//打开顶点属性0 位置
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
		//打开顶点属性1颜色
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);

	}
	glUseProgram(shader->ShaderProgram);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Sapphire::OpenGLVideoDriver::drawGeometry(Sapphire::Geometry * geo, const char* shaderName)
{
	if (!geo)
	{
		return;
	}
	ShaderStruct* shader = m_pShaderManager->FindShader(shaderName);
	if (!shader || !shader->ShaderProgram || !shader->isCompiled)
	{
		return;
	}

	Sapphire::VertexBuffer* pVb = geo->getVertexBuffer();
	Sapphire::IndexBuffer* pIb = geo->getIndexBuffer();
	if (!pVb || !pIb)
	{
		return;
	}
	ULONG vbSize = 0;
	GLfloat* vertices = NULL;
	GLuint   vbStride = 0;
	vertices = (GLfloat*)pVb->getData(vbSize, vbStride);
	ULONG ibSize = 0;
	GLuint* indexs = NULL;
	GLuint  ibStride = 0;
	indexs = (GLuint*)pIb->getData(ibSize, ibStride);
	//创建VBO缓冲区
	GLuint vbo;
	glGenBuffers(1, &vbo);
	GLuint vao;
	//创建VAO
	glGenVertexArrays(1, &vao);
	GLuint ebo;
	//创建EBO对象
	glGenBuffers(1, &ebo);
	//设置VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	//设置缓冲区数据
	glBufferData(GL_ARRAY_BUFFER, vbSize, vertices, GL_STATIC_DRAW);

	Sapphire::VertexAttributeInfoList attributesInfo = pVb->getAttributeInfo();

	//设置VAO
	{
		glBindVertexArray(vao);
		for (int i = 0; i < attributesInfo.size(); i++)
		{
			int len = attributesInfo[i].length / sizeof(GL_FLOAT);
			int stride = pVb->getStride();
			int offset = attributesInfo[i].getOffset();
			//设置顶点属性结构
			glVertexAttribPointer(i, len, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(offset));
			//打开顶点属性i 位置
			glEnableVertexAttribArray(i);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibSize, indexs, GL_STATIC_DRAW);
		glBindVertexArray(0);

	}
	glUseProgram(shader->ShaderProgram);
	IMaterial* material = geo->getMaterial();
	if (material)
	{
		EVertexType vType = material->GetVertexType();
		switch (vType)
		{
		case EVT_COLOR:
			break;
		case EVT_NORMAL:
			break;
		case EVT_TCOORD:
		{
			ITexture2D* itexture = geo->getMaterial()->GetTexture("tex1");
			shader = geo->getMaterial()->GetShader("TQuadShader");
			GLuint textureHandle = 0;
			glGenTextures(1, &textureHandle);
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			void* dataPtr = 0;
			bool bRet = itexture->GetData(dataPtr);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, itexture->GetWidth(), itexture->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, dataPtr);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			glUniform1i(glGetUniformLocation(shader->ShaderProgram, "texture1"), 0);

		}
		break;
		case EVT_2TCOORD:
			break;
		case EVT_TANGENTS:
			break;
		default:
			break;
		}
	}

	
	 
	
	
	//使用VAO
	glBindVertexArray(vao);
	if (pIb->getType() == Sapphire::EIT_16BIT)
	{
		glDrawElements(GL_TRIANGLES, pIb->size(), GL_UNSIGNED_SHORT, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, pIb->size(), GL_UNSIGNED_INT, 0);
		
	}
	GLenum errorcode = glGetError();
	//取消绑定VAO
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

}



Sapphire::ShaderManager * Sapphire::OpenGLVideoDriver::GetShaderManager()
{
	return m_pShaderManager;
}

void Sapphire::OpenGLVideoDriver::release()
{

}


