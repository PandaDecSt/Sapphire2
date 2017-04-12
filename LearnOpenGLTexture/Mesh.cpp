#include "stdafx.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "ShaderManager.h"
#include "Mesh.h"
#include <iostream>



bool Sapphire::SMeshNode::Load(MeshStruct* mesh, MeshBuffer::BufferMode mode)
{
	if (!mesh)
		return false;
	mMeshBuffer = new MeshBuffer(mesh, mWindow, mode);
	return true;
}

bool Sapphire::SMeshNode::Load(ShaderStruct * shader)
{
	if (!shader->isCompiled)
	{
		return false;
	}
	mShader = shader;
	return true;
}

void Sapphire::SMeshNode::Release()
{
	if (mMeshBuffer)
	{
		mMeshBuffer->Release();
		mMeshBuffer = NULL;
	}
	mWindow = NULL;
	mShader = NULL;

	delete this;
}

void Sapphire::SMeshNode::DrawMesh()
{
	mMeshBuffer->DrawMeshBuffer(mShader);
}

bool Sapphire::MeshBuffer::Initialize()
{
	//����VBO������
	glGenBuffers(1, &mVbo);
	//����VAO
	glGenVertexArrays(1, &mVao);
	//����EBO���� (Ԫ�ػ������  �����㻹��Ԫ��)
	glGenBuffers(1, &mEbo);
	mIsInit = true;
	return true;
}

void Sapphire::MeshBuffer::Release()
{
	delete mMesh;
	delete this;
}

void Sapphire::MeshBuffer::DrawMeshBuffer(ShaderStruct * shader)
{
	if (shader == NULL || shader->isCompiled == false)
	{
		return;
	}
	Initialize();
	//����VBO
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	switch (mMode)
	{
	case BufferMode::Static_Draw:
	{
		glBufferData(GL_ARRAY_BUFFER, mMesh->vertexBufferSize, mMesh->vertices, GL_STATIC_DRAW);
		break;
	}
	case BufferMode::Dynamic_Draw:
	{
		glBufferData(GL_ARRAY_BUFFER, mMesh->vertexBufferSize, mMesh->vertices, GL_DYNAMIC_DRAW);
		break;
	}
	case BufferMode::Stream_Draw:
	{
		glBufferData(GL_ARRAY_BUFFER, mMesh->vertexBufferSize, mMesh->vertices, GL_STREAM_DRAW);
		break;
	}
	default:
		break;
	}

	//����VAO
	{
		glBindVertexArray(mVao);
		//���ö������Խṹ
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
		//���ö����������飨index < MAX_VERTEX_ATTRBS-1��
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		//����EBO
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
			switch (mMode)
			{
			case BufferMode::Static_Draw:
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMesh->indexBufferSize, mMesh->indices, GL_STATIC_DRAW);
				break;
			}
			case BufferMode::Dynamic_Draw:
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMesh->indexBufferSize, mMesh->indices, GL_DYNAMIC_DRAW);
				break;
			}
			case BufferMode::Stream_Draw:
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMesh->indexBufferSize, mMesh->indices, GL_STREAM_DRAW);
				break;
			}
			default:
				break;
			}
		}
		glBindVertexArray(0);

	}




	glUseProgram(shader->ShaderProgram);


	glBindVertexArray(mVao);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	GLenum errorcode = glGetError();
	glBindVertexArray(0);

	Finish();
	return;

}

void Sapphire::MeshBuffer::Finish()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &mVao);
	glDeleteBuffers(1, &mVbo);
	glDeleteBuffers(1, &mEbo);
}
