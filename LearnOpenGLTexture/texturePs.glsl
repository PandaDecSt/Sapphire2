#version 420 core  //����汾 openGL 3.3 core profile
 #pragma optimize (off)
 //�����������Ҫ��vertex shader�������һ��
in vec4 vertexColor;
in vec2 tcoord;
//������Զ���
out vec4 color;

uniform sampler2D texture1;
 
void main()
{
    //color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	color = texture(texture1, tcoord);
}