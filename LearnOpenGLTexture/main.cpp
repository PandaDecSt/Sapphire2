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
#include "Quad.h"
#include "Context.h"
#include "Resource\Image.h"
#include "FileSystem.h"
#include "Resource\ResourceCache.h"
#include "Material.h"
#include "Texture2D.h"

#pragma comment(lib,"opengl32.lib")

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void render(GLFWwindow* window);

GLFWwindow*  init();

Sapphire::ShaderManager* shaderMgr;
Sapphire::IMaterial* material;
Sapphire::Geometry*  tgeometry;


int main(char* argc[], int argv)
{

	GLFWwindow* window = init();

	// ���崰�ڴ�С
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	string vs_source = readTextFile("vs.glsl");
	string ps_source = readTextFile("ps.glsl");
	string vs2_source = readTextFile("colorVs.glsl");
	string ps2_source = readTextFile("colorPs.glsl");	
	string vs3_source = readTextFile("textureVs.glsl");
	string ps3_source = readTextFile("texturePs.glsl");

	shaderMgr = new Sapphire::ShaderManager();
	shaderMgr->CreateShaderProgram("lineShader", vs_source.c_str(), ps_source.c_str());
	Sapphire::ShaderStruct* shader = shaderMgr->FindShader("lineShader");
	shaderMgr->CompileAndLink(shader);
	shaderMgr->CreateShaderProgram("QuadShader", vs2_source.c_str(), ps2_source.c_str());
	shaderMgr->PrintLogs();
	Sapphire::ShaderStruct* shader2 = shaderMgr->FindShader("QuadShader");
	shaderMgr->CompileAndLink(shader2);
	shaderMgr->PrintLogs();
	shaderMgr->CreateShaderProgram("TQuadShader", vs3_source.c_str(), ps3_source.c_str());
	shaderMgr->PrintLogs();
	Sapphire::ShaderStruct* shader3 = shaderMgr->FindShader("TQuadShader");
	shaderMgr->CompileAndLink(shader3);

	Sapphire::SharedPtr<Sapphire::Context> context = Sapphire::SharedPtr<Sapphire::Context>(new Sapphire::Context());
	context->RegisterSubsystem(new Sapphire::FileSystem(context));
	context->RegisterSubsystem(new Sapphire::ResourceCache(context));
	Sapphire::ResourceCache* pResourceCache = context->GetSubsystem<Sapphire::ResourceCache>();
	Sapphire::SharedPtr<Sapphire::Image> img = Sapphire::DynamicCast<Sapphire::Image>(context->CreateObject(Sapphire::Image::GetTypeInfoStatic()->GetType()));
	img->SetName("testImage");
	Sapphire::FileSystem* pFileSys = context->GetSubsystem<Sapphire::FileSystem>();
	Sapphire::String path = pFileSys->GetProgramDir();
	AddTrailingSlash(path);
	path += "resources";
	bool ret = pResourceCache->AddResourceDir(path);
	const Sapphire::String image_name = "test.jpg";
	Sapphire::Image* image = pResourceCache->GetResource<Sapphire::Image>(image_name);
	material = new Sapphire::Material();
	material->SetFillMode(Sapphire::FillMode::FILL_SOLID);
	Sapphire::ITexture2D* tex2d = new Sapphire::Texture2D();
	tex2d->SetData(image);
	tex2d->SetSize(image->GetWidth(), image->GetHeight(), 24);
	material->SetTexture("tex1",tex2d);
	material->SetVertexType(Sapphire::EVT_TCOORD);
	material->SetShader("TQuadShader", shader3);

	Sapphire::VertexTcoord leftTop(Sapphire::Vector3(-0.3, 0.4, 0), Sapphire::Color::BLUE, Sapphire::Vector3(0,0,0), Sapphire::Vector2(0,0));
	Sapphire::VertexTcoord rightTop(Sapphire::Vector3(0.3, 0.4, 0), Sapphire::Color::RED, Sapphire::Vector3(0, 0, 0), Sapphire::Vector2(1, 0));
	Sapphire::VertexTcoord leftBottom(Sapphire::Vector3(-0.3, -0.4, 0), Sapphire::Color::GREEN, Sapphire::Vector3(0, 0, 0), Sapphire::Vector2(0, 1));
	Sapphire::VertexTcoord rightBottom(Sapphire::Vector3(0.3, -0.4, 0), Sapphire::Color::BLACK, Sapphire::Vector3(0, 0, 0), Sapphire::Vector2(1, 1));
	Sapphire::TexturedQuad quad(leftTop, rightTop, leftBottom, rightBottom);
	tgeometry = quad.toGeometry();

	while (!glfwWindowShouldClose(window))
	{
		render(window);
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

void render2(GLFWwindow * window)
{

}

void render(GLFWwindow * window)
{
	
	

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
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	/*Sapphire::MeshStruct* mesh = new Sapphire::MeshStruct();
	mesh->vertices = vertices;
	mesh->vertexBufferSize = sizeof(vertices);
	mesh->indices = indices;
	mesh->indexBufferSize = sizeof(indices);
	mesh->offset = 0;
	mesh->attribCount = 3;
	mesh->index = 0;
	Sapphire::SMeshNode*  smesh = new Sapphire::SMeshNode();
	Sapphire::ShaderStruct* shader = shaderMgr->FindShader("QuadShader");
	smesh->Load(shader);
	smesh->Load(mesh);
	
	smesh->DrawMesh();*/

	/*Sapphire::Line3d lines(Sapphire::Vector3(0.5, -0.8, 1.0), Sapphire::Vector3(-0.5, -0.8, 1.0));
	Sapphire::Line3d lines2(Sapphire::Vector3(-0.2, -0.8, 1.0), Sapphire::Vector3(0.7, 0.8, 1.0));
	lines.setColor(Sapphire::Color(1.0, 0.0, 0.0, 1.0));
	lines2.setColor(Sapphire::Color(1.0, 1.0, 0.0, 1.0));*/
	Sapphire::IVideoDriver* vd = new Sapphire::OpenGLVideoDriver(window, shaderMgr);
	/*vd->drawLine(lines,"lineShader");
	vd->drawLine(lines2,"lineShader");*/
	
	{
		//Sapphire::VertexColor leftTop(Sapphire::Vector3(-0.3, 0.4, 0), Sapphire::Color::BLUE);
		//Sapphire::VertexColor rightTop(Sapphire::Vector3(0.3, 0.4, 0), Sapphire::Color::RED);
		//Sapphire::VertexColor leftBottom(Sapphire::Vector3(-0.3, -0.4, 0), Sapphire::Color::GREEN);
		//Sapphire::VertexColor rightBottom(Sapphire::Vector3(0.3, -0.4, 0), Sapphire::Color::BLACK);
		//UINT vertexColorSize = sizeof(leftTop);
		//UINT vertexSize = sizeof(Sapphire::Vertex);
		//UINT tstSize = sizeof(Sapphire::Vector3) + sizeof(Sapphire::Color) + sizeof(Sapphire::EVertexType);
		//Sapphire::ColorQuad quad(leftTop, rightTop, leftBottom, rightBottom);
		//byte* seek = (byte*)&leftTop;
		////ǰ4�ֽ���������ռ�ã�������ǳ�Ա����
		//seek += 4;
		//Sapphire::Vector3* v = (Sapphire::Vector3*)seek;
		//seek += sizeof(Sapphire::Vector3)+4;
		//Sapphire::Color* color = (Sapphire::Color*)seek;
		//Sapphire::Geometry*  geometry = quad.toGeometry();
		//vd->drawGeometry(geometry, "QuadShader");
		//delete geometry;
	}

	{
		
		tgeometry->setMaterial(material);
		vd->drawGeometry(tgeometry, "TQuadShader");
	}
	
	vd->release();
	delete vd;

}


//������ʱGLFW�Ļص���Ӧ
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}