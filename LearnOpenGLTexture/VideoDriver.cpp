#include "stdafx.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "ShaderManager.h"
#include "VideoDriver.h"


bool OpenGLVideoDriver::Load(ShaderStruct * shader)
{
	if (!shader->isCompiled)
	{
		return false;
	}
	mCurShader = shader;
	return true;
}
void OpenGLVideoDriver::drawLine(const Sapphire::Line3d & line)
{
	Sapphire::Line3d l = line;

	GLfloat vertices[] =
	{
		l.getStart().x_,l.getStart().y_, l.getStart().z_,l.getColor().r_,l.getColor().g_,l.getColor().b_,l.getColor().a_,
		l.getEnd().x_,l.getEnd().y_, l.getEnd().z_,l.getColor().r_,l.getColor().g_,l.getColor().b_,l.getColor().a_
	};

	ulong size = sizeof(vertices);

	//����VBO������
	GLuint vbo;
	glGenBuffers(1, &vbo);
	GLuint vao;
	//����VAO
	glGenVertexArrays(1, &vao);
	GLuint ebo;
	//����EBO����
	glGenBuffers(1, &ebo);
	if (mCurShader == NULL || mCurShader->isCompiled == false)
	{
		return;
	}
	//����VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//���û���������
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	//����VAO
	{
		glBindVertexArray(vao);
		//���ö������Խṹ
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)0);
		//���ö����������飨index < MAX_VERTEX_ATTRBS-1��
		//�򿪶�������0 λ��
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
		//�򿪶�������1��ɫ
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);

	}
	glUseProgram(mCurShader->ShaderProgram);
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void OpenGLVideoDriver::drawRenderList()
{
}
