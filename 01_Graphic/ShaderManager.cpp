#include "ShaderManager.h"
#include "Log.h"
#include <sstream>
#include <iostream>
#include  <GL\glew.h>

Sapphire::ShaderManager::ShaderManager(Context* ctx) : Object(ctx)
{
}

void Sapphire::ShaderManager::Release()
{
	shaderMap.Clear();
}

Sapphire::Shader * Sapphire::ShaderManager::CreateShaderProgram(String shaderName, String vertexShaderSrc, String fragmentShaderSrc)
{
	SharedPtr<Shader> shader = DynamicCast<Shader>(context_->CreateObject(Shader::GetTypeStatic()));
	shader->ShaderName = shaderName;
	shader->VertexShaderSrc = vertexShaderSrc;
	shader->FragmentShaderSrc = fragmentShaderSrc;

	shader->ShaderProgram = glCreateProgram();
	shader->VertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
	shader->FragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

	shaderMap[shaderName] = shader;

	return shader;
}

bool Sapphire::ShaderManager::CompileAndLink(Shader* shader)
{
	const GLchar* vertexShaderSource = shader->VertexShaderSrc.CString();
	glShaderSource(shader->VertexShaderHandle, 1, &vertexShaderSource, NULL);
	glCompileShader(shader->VertexShaderHandle);
	GLint success;
	GLchar infoLog[512];
	stringstream ss;
	//��ȡ����״̬
	glGetShaderiv(shader->VertexShaderHandle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader->VertexShaderHandle, 512, NULL, infoLog);
		ss << " Shader Name:" << shader->ShaderName.CString();
		ss << "    ERROR::SHADER::VERTEX::COMPILATION_FAILED   \n" << infoLog << endl;
		logs.push_back(ss.str().c_str());
		ss.str("");
		return false;
	}

	const GLchar* fragmentShaderSource = shader->FragmentShaderSrc.CString();
	glShaderSource(shader->FragmentShaderHandle, 1, &fragmentShaderSource, NULL);
	glCompileShader(shader->FragmentShaderHandle);
	//��ȡ����״̬
	glGetShaderiv(shader->FragmentShaderHandle, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader->FragmentShaderHandle, 512, NULL, infoLog);
		ss << " Shader Name:" << shader->ShaderName.CString();
		ss << "    ERROR::SHADER::FRAGMENT::COMPILATION_FAILED    \n" << infoLog << endl;
		logs.push_back(ss.str().c_str());
		ss.str("");
		return false;
	}

	glAttachShader(shader->ShaderProgram, shader->VertexShaderHandle);
	glAttachShader(shader->ShaderProgram, shader->FragmentShaderHandle);
	glLinkProgram(shader->ShaderProgram);
	glGetProgramiv(shader->ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader->ShaderProgram, 512, NULL, infoLog);
		GLenum errorcode = glGetError();
		ss << " Shader Name:" << shader->ShaderName.CString();
		ss << "    SHADER::PROGRAME::LINK_FAILED   \n" << infoLog << "  ERROR CODE: " << errorcode << infoLog << endl;
		logs.push_back(ss.str().c_str());
		ss.str("");
		return false;
	}
	shader->isCompiled = true;
	return true;
}

Sapphire::Shader * Sapphire::ShaderManager::FindShader(String shaderName)
{
	Shader* shader = shaderMap[shaderName];
	return shader;
}


void Sapphire::ShaderManager::ReleaseShader(String shaderName)
{ 
	DeleteShader(shaderName);
}

void Sapphire::ShaderManager::DeleteShader(String shaderName)
{
	if (shaderName == "")
		return;
	HashMap<String,SharedPtr<Shader>>::Iterator it = shaderMap.Find(shaderName);
	if (it != shaderMap.End())
	{
		SharedPtr<Shader> shader = it->second_;
		if (shader->VertexShaderHandle)
		{
			glDeleteShader(shader->VertexShaderHandle);
		}
		if (shader->FragmentShaderHandle)
		{
			glDeleteShader(shader->FragmentShaderHandle);
		}
		if (shader->ShaderProgram)
		{
			glDeleteProgram(shader->ShaderProgram);
		}
		shaderMap.Erase(it);
	}
	

}

void Sapphire::ShaderManager::PrintLogs()
{
	for (int i = 0; i < logs.size(); i++)
	{
		SAPPHIRE_LOGDEBUG(logs[i].CString());
	}
}


