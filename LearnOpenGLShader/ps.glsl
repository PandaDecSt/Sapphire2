#version 330 core  //����汾 openGL 3.3 core profile
 
 //�����������Ҫ��vertex shader�������һ��
in vec3 vertexColor;
//������Զ���
out vec4 color;
 
void main()
{
    //color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	color = vec4(vertexColor.r,vertexColor.g,vertexColor.b,1.0);
}