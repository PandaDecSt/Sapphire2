#pragma once

#include "stdafx.h"
#include <string>
#include <unordered_map>
#include <GL\glew.h>
#include "Shader.h"

namespace Sapphire
{
	class SAPPHIRE_API ShaderManager : public Object
	{
		SAPPHIRE_OBJECT(ShaderManager, Object);

	public:

		ShaderManager(Context* ctx);
		virtual ~ShaderManager() {};

		void Release();

		Shader*  CreateShaderProgram(String shaderName, String vertexShaderSrc, String fragmentShaderSrc);
		bool     CompileAndLink(Shader* shader);
		Shader*  FindShader(String shaderName);
		void  ReleaseShader(String shaderName);
		void  PrintLogs();

	private:

		HashMap<String, SharedPtr<Shader>> shaderMap;
		vector<String>                 logs;

		void  DeleteShader(String shaderName);




	};
}