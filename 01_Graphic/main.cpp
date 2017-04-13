//#define GLEW_STATIC  //ʹ��GLEW�ľ�̬��

#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <fstream>
#include <sstream>

#pragma comment(lib,"opengl32.lib")

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main(char* argc[], int argv)
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
		return -1;
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
		return -1;
	}


	// ����ʱ�̴�С
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	std::ifstream in("vertex.glsl");
	std::ostringstream tmp;
	tmp << in.rdbuf();
	std::string str = tmp.str();
	in.close();


	const GLchar* vs_source = str.c_str();
	in.open("fragment.glsl");
	tmp.str("");  //���sstream��clear()û����
	tmp << in.rdbuf();
	str = tmp.str();
	in.close();
	const GLchar* fs_source = str.c_str();

	//���嶥����ɫ�����
	GLuint vertexShader;
	//����shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//���shaderԴ����
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	//��ȡ����״̬
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return 0;
	}

	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs_source, NULL);
	glCompileShader(fragmentShader);

	//��ȡ����״̬
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return 0;
	}

	//��ɫ���������shader program object���Ƕ����ɫ��������ӵİ汾��
	//���Ҫʹ�øղű������ɫ�����Ǳ������������Ϊһ����ɫ���������Ȼ����Ⱦ�����ʱ�򼤻������ɫ�����򡣼����˵���ɫ���������ɫ�����ڵ�����Ⱦ����ʱ�ſ��á�
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		GLenum errorcode = glGetError();
		std::cout << "SHADER::PROGRAME::LINK_FAILED\n" << infoLog << "  ERROR CODE: " << errorcode << std::endl;
		return 0;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//���㼯��
	GLfloat vertices[] = {

		0.5f, 0.5f, 0.0f,   // ���Ͻ�
		0.5f, -0.5f, 0.0f,  // ���½�
		-0.5f, -0.5f, 0.0f, // ���½�
		-0.5f, 0.5f, 0.0f   // ���Ͻ�
	};
	GLuint indices[] = { // ��ʼ��0!

		0, 1, 3, // ��һ��������
		1, 2, 3  // �ڶ���������
	};

	//vbo������
	GLuint VBO;
	//����������
	glGenBuffers(1, &VBO);
	//���������������
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	//��VAO
	glBindVertexArray(VAO);

	//�󶨻�����
	//OpenGL�кܶ໺��������ͣ�GL_ARRAY_BUFFER������һ�����㻺�����Ļ������͡�
	//OpenGL��������ͬʱ�󶨶�����壬ֻҪ�����ǲ�ͬ�Ļ�������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Ϊ��������������
	//�κλ��庯������GL_ARRAY_BUFFERĿ���ϣ������������õ�ǰ�󶨵Ļ��壨VBO����
	//Ȼ�����ǿ��Ե���glBufferData�����������֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
	//glBufferData��һ���������û��������ݸ��Ƶ���ǰ�󶨻���ĺ�����
	//���ĵ�һ������������ϣ�������ݸ��Ƶ�����Ļ������ͣ�
	//���㻺�����ǰ�󶨵�GL_ARRAY_BUFFERĿ���ϡ�
	//�ڶ�������ָ������ϣ�����ݸ���������ݵĴ�С���ֽڣ�����һ���򵥵�sizeof������������ݾ��С�
	//����������������ϣ�����͵���ʵ���ݡ�
	/*
	���ĸ�����ָ��������ϣ���Կ���ι�����������ݡ���������ʽ��

	GL_STATIC_DRAW�����ݲ���򼸺�����ı䡣
	GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
	GL_STREAM_DRAW������ÿ�λ���ʱ����ı䡣
	�����ε�λ�����ݲ���ı䣬ÿ����Ⱦ����ʱ������ԭ����������ʹ�õ����������GL_STATIC_DRAW����������磬һ�������е����ݽ�Ƶ�����ı䣬��ôʹ�õ����;���GL_DYNAMIC_DRAW��GL_STREAM_DRAW����������ȷ��ͼ�ο������ݷ��ڸ���д����ڴ沿�֡�
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//��������location��Ϊ���Ĳ���
	glEnableVertexAttribArray(0);

	// 1. ���ö�������ָ��,����������
	//��һ������ָ������Ҫ������һ����������,�ڶ�����ɫ����ʹ��layout(location = 0)�����˶������ԡ���λ�ã�position����location
	//�ڶ�������ָ���������ԵĴ�С������������vec3���ͣ�����3����ֵ��ɡ�
	//����������ָ�����ݵ����ͣ�������GL_FLOAT
	//���ĸ��������������Ƿ�ϣ�����ݱ���׼���������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������ - 1����1֮�䡣
	//�������������������stride���������������������Ķ�������֮�����ж��� (Ҫע�������������֪����������ǽ������еģ���������������֮��û�п�϶������Ҳ��������Ϊ0����OpenGL�������岽���Ƕ��٣�ֻ�е���ֵ�ǽ�������ʱ�ſ��ã�)
	//���һ�������йŹֵ�GLvoid*��ǿ������ת���������ǵ�λ�������ڻ�������ʼλ�õ�ƫ����������λ������������Ŀ�ʼ������������0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);


	/*
	ÿ���������Դ�VBO������ڴ��л���������ݣ�������ȡ���ݵ��Ǹ�VBO��
	���ǵ�����glVetexAttribPointer��ʱ������󶨵�GL_ARRAY_BUFFER���Ǹ�VBO��
	�����ڵ���glVertexAttribPointer֮ǰ����VBO����������0�������ӵ������Ķ������ݡ�
	*/

	/*

	OpenGL core-profile��Ҫ������ʹ��VAO������������֪�������ǵĶ���������Щʲô��������ǰ�VAOʧ�ܣ�OpenGL��ܾ������κζ�����
	һ������������󴢴���������ݣ�

	����glEnableVertexAttribArray��glDisableVertexAttribArray�ġ�
	ʹ��glVertexAttribPointer�Ķ����������á�
	ʹ��glVertexAttribPointer���еĶ��㻺������붥����������

	*/
	// ��Ⱦ
	// �����ɫ������
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//��glUseProgram���������´����ĳ��������Ϊ���Ĳ������������ܼ�������������
	//������glUseProgram��������֮���ÿ����ɫ������Ⱦ���������õ����������󣨵�Ȼ������Щ���ӵ���ɫ�����ˡ�
	glUseProgram(shaderProgram);
	//����������
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//������������
	GLuint EBO;
	glGenBuffers(1, &EBO);
	//��VBO���ƣ����ǰ�EBOȻ����glBufferData���������Ƶ������ͬ������VBO���ƣ����ǻ����Щ�������÷��ڰ󶨺ͽ��������֮�䣬������ǰѻ�������Ͷ���ΪGL_ELEMENT_ARRAY_BUFFER��
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//�߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	////��һ������ָ�������ǻ��Ƶ�ģʽ�������glDrawArrays��һ��
	//�ڶ������������Ǵ�����ƶ���Ĵ�����������6��˵�������ܹ������6������
	//���������������������ͣ�������GL_UNSIGNED_INT��
	//���һ�����������ǿ���ָ��EBO�е�ƫ����(���ߴ���һ���������飬������ֻ�ǵ��㲻����ʹ��������������ʱ��)
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glfwSwapBuffers(window);
	//�����
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{

		//��飬����¼�������(����������ƶ�)���ú��ʵ���Ӧ����
		glfwPollEvents();
		// ����������

	}
	glDeleteBuffers(1, &VBO);

	//����GLFW�����GLFW����Դ
	glfwTerminate();
	return 0;
}

//������ʱGLFW�Ļص���Ӧ
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}