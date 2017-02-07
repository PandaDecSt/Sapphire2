#version 330 core

layout (location = 0) in vec3 position;   //������λ�� ��λ��0��
in vec4 color;                            //��������ɫ

out vec4 vertexColor;                     //�����

void main()
{
   gl_Position = vec4(position.xyz,1.0);
   vertexColor = color;
}