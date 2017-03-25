#pragma once


namespace Sapphire
{
	class VertexBuffer;
	class IndexBuffer;
	//! ö��ͼԪ����
	enum EPRIMITIVE_TYPE
	{
		//��������
		EPT_POINTS = 0,

		//���ж������ӳ�һ�������߶�
		EPT_LINE_STRIP,

		//!ͬ�ϣ�������λ���
		EPT_LINE_LOOP,

		//! ÿ������������ΪN/2���߶�
		EPT_LINES,

		//!  ��ǰ����������ÿ�����㹹��һ����������N-2
		EPT_TRIANGLE_STRIP,

		//!  ��ǰ����������ÿ�����㹹��һ����������N-2
		//! ���е������ζ����Ƶ�һ������
		EPT_TRIANGLE_FAN,

		//! �������ж��㵽ÿһ��������
		EPT_TRIANGLES,

		//! ÿ�ĸ����㹹��һ���ı���
		EPT_QUADS,

		//! ����һ��LINE_LOOP,�����
		EPT_POLYGON,

		//! ��GPU��һ��������չ��һ���ı��εĹ�����
		EPT_POINT_SPRITES
	};


	class Geometry
	{
		

	public:

		Geometry()
		{
			mVertexBuffer = 0;
			mIndexBuffer = 0;
			mType = EPT_TRIANGLES;
		};
		Geometry(VertexBuffer* pVb, IndexBuffer* pIb, EPRIMITIVE_TYPE type);
		~Geometry();
		void draw();
		void setVertexBuffer(VertexBuffer* pBuf);
		VertexBuffer* getVertexBuffer();
		void setIndexBuffer(IndexBuffer* pBuf);
		IndexBuffer*  getIndexBuffer();
		EPRIMITIVE_TYPE  getType();
		void          setType(EPRIMITIVE_TYPE type);
		void release();


	private:

		VertexBuffer*  mVertexBuffer;
		IndexBuffer*   mIndexBuffer;
		EPRIMITIVE_TYPE  mType;
		
		
	};
}