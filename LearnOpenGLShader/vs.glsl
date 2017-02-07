#version 330 core   //����汾 openGL 3.3 core profile

//���붥������: ���岼��(��������index)  ����  �������
layout (location=0) in vec3 position;
layout (location=1) in vec3 vColor;

out vec3 vertexColor;

void main()
{
    //gl_PositionԤ���������
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
	vertexColor = vColor;
}