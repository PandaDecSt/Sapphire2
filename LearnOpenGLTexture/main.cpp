//#define GLEW_STATIC  //ʹ��GLEW�ľ�̬��

#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <fstream>
#include <sstream>
#include "StringUtil.h"
#include "IOUtil.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "Line.h"
#include "VideoDriver.h"
#include "Vertex.h"

#pragma comment(lib,"opengl32.lib")

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void render();

GLFWwindow*  init();


int main(char* argc[], int argv)
{

	GLFWwindow* window = init();

	// ���崰�ڴ�С
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);



	while (!glfwWindowShouldClose(window))
	{
		render();
		glfwSwapBuffers(window);
		//��飬����¼�������(����������ƶ�)���ú��ʵ���Ӧ����
		glfwPollEvents();
		// ����������
	}
	//����GLFW�����GLFW����Դ
	glfwTerminate();
	return 0;
}

GLFWwindow* init()
{
	//glfwInit����ʼ��GLFW��Ȼ�����ǿ���ʹ��glfwWindowHint������GLFW��
	glfwInit();
	//glfwWindowHint�ĵ�һ�������������ǣ������ĸ�ѡ���Щѡ�����GLFW_ǰ׺���ڶ���������һ������������������Ϊѡ�������õ�ֵ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //����GLFW����ʹ�õ�OpenGL�汾��3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //����ͬ������GLFW������ϣ����ȷ��ʹ��core-profile
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//GLFW�������ں���
	GLFWwindow * window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	//�������Ǵ��ڵĻ�����glfwMakeContextCurrent������������ǵ�ǰ�̵߳�������
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	// ������Ҫ�Ļص�����
	glfwSetKeyCallback(window, key_callback);

	//��ʼ��GLEW
	//�ڳ�ʼ��GLEWǰ���ǰ�glewExperimental��������ΪGL_TRUE��(glew 1.13�����İ汾)
	//����glewExperimentalΪtrue���Ա�֤GLEWʹ�ø�����ִ�����������OpenGL���ܡ�
	//�������ô���ã����ͻ�ʹ��Ĭ�ϵ�GL_FALSE��������ʹ��core profile��ʱ�п��ܷ������⡣
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //glewInit()����Opengl��������֮��
	{
		//��ʼ��ʧ��
		std::cout << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
	return window;
}

void render()
{
	string vs_source = readTextFile("vs.glsl");
	string ps_source = readTextFile("ps.glsl");

	ShaderManager* shaderMgr = new ShaderManager();
	shaderMgr->CreateShaderProgram("shader1", vs_source.c_str(), ps_source.c_str());
	ShaderStruct* shader = shaderMgr->FindShader("shader1");
	shaderMgr->CompileAndLink(shader);
	shaderMgr->PrintLogs();

	//���㼯��
	/*
	GLfloat vertices[] = {

	-0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
	0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f,
	0.5f,  -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
	};
	*/
	GLfloat vertices[] = {

		0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // ���Ͻ�
		0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // ���½�
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, // ���½�
		-0.5f, 0.5f, 0.0f,   0.0f, 1.0f, 1.0f,// ���Ͻ�
	};
	GLuint indices[] = { // ��ʼ��0!

		0, 1, 3, // ��һ��������
		1, 2, 3  // �ڶ���������
	};

	MeshStruct* mesh = new MeshStruct();
	mesh->vertices = vertices;
	mesh->vertexBufferSize = sizeof(vertices);
	mesh->indices = indices;
	mesh->indexBufferSize = sizeof(indices);
	mesh->offset = 0;
	mesh->attribCount = 3;
	mesh->index = 0;
	SMeshNode*  smesh = new SMeshNode();
	smesh->Load(shader);
	smesh->Load(mesh);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	smesh->DrawMesh();

	Sapphire::Line3d lines(Sapphire::Vector3(0.5, -0.8, 1.0), Sapphire::Vector3(-0.5, -0.8, 1.0));
	Sapphire::Line3d lines2(Sapphire::Vector3(-0.2, -0.8, 1.0), Sapphire::Vector3(0.7, 0.8, 1.0));
	lines.setColor(Sapphire::Color(1.0, 0.0, 0.0, 1.0));
	lines2.setColor(Sapphire::Color(1.0, 1.0, 0.0, 1.0));
	OpenGLVideoDriver* vd = new OpenGLVideoDriver();
	vd->Load(shader);
	vd->drawLine(lines);
	vd->drawLine(lines2);
	delete vd;

}


//������ʱGLFW�Ļص���Ӧ
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}