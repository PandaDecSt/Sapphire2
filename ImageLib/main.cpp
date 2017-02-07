#include <iostream>
#include <Windows.h>
#include <io.h>
#include "ximage.h"
#include "ImageLoader.h"
#include "ImageWriter.h"
#include "Image.h"


using namespace std;

typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned short WORD;

typedef struct {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BMPFILEHEADER_T;

typedef struct {
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BMPINFOHEADER_T;

void savebmp(uint8 * pdata, char * bmp_file, int width, int height)
{      //�ֱ�Ϊrgb���ݣ�Ҫ�����bmp�ļ�����ͼƬ����  
	int size = width*height * 3 * sizeof(char); // ÿ�����ص�3���ֽ�  
												// λͼ��һ���֣��ļ���Ϣ  
	BMPFILEHEADER_T bfh;
	bfh.bfType = (WORD)0x4d42;  //bm  
	bfh.bfSize = size  // data size  
		+ sizeof(BMPFILEHEADER_T) // first section size  
		+ sizeof(BMPINFOHEADER_T) // second section size  
		;
	bfh.bfReserved1 = 0; // reserved  
	bfh.bfReserved2 = 0; // reserved  
	bfh.bfOffBits = sizeof(BMPFILEHEADER_T) + sizeof(BMPINFOHEADER_T);//���������ݵ�λ��  

																	  // λͼ�ڶ����֣�������Ϣ  
	BMPINFOHEADER_T bih;
	bih.biSize = sizeof(BMPINFOHEADER_T);
	bih.biWidth = width;
	bih.biHeight = -height;//BMPͼƬ�����һ���㿪ʼɨ�裬��ʾʱͼƬ�ǵ��ŵģ�������-height������ͼƬ������  
	bih.biPlanes = 1;//Ϊ1�����ø�  
	bih.biBitCount = 24;
	bih.biCompression = 0;//��ѹ��  
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 2835;//����ÿ��  
	bih.biYPelsPerMeter = 2835;
	bih.biClrUsed = 0;//���ù�����ɫ��24λ��Ϊ0  
	bih.biClrImportant = 0;//ÿ�����ض���Ҫ  
	FILE * fp = fopen(bmp_file, "wb");
	if (!fp) return;

	fwrite(&bfh, 8, 1, fp);//����linux��4�ֽڶ��룬����Ϣͷ��СΪ54�ֽڣ���һ����14�ֽڣ��ڶ�����40�ֽڣ����ԻὫ��һ���ֲ���Ϊ16�Լ���ֱ����sizeof����ͼƬʱ�ͻ�����premature end-of-file encountered����  
	fwrite(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, fp);
	fwrite(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, fp);
	fwrite(&bih, sizeof(BMPINFOHEADER_T), 1, fp);
	fwrite(pdata, size, 1, fp);
	fclose(fp);
}

int main()
{


	wstring file = L"111.jpg";
	if (_waccess(file.c_str(), 0) != -1)
	{
		CxImage image;
		image.Load(file.c_str());
		long size = 0;
		uint8* buf = NULL;
		long size_2 = image.GetSize();
		//bool ret = image.Encode(buf, size, CXIMAGE_FORMAT_BMP);
		buf = image.GetBits();  //��ȡ�����ڴ�����
		ulong w = image.GetWidth();
		ulong h = image.GetHeight();
		ulong bpp = image.GetBpp();
		CxImage image2;
		bool ret = image2.CreateFromArray(buf, w, h, 24, w * 3, false);
		image2.GetBits();
		cout << size_2 << endl;
	     
	 
		//image.Resample(800, 600, 1, &dst);
		image2.Save(L"222.bmp", CXIMAGE_FORMAT_BMP);
	}

	//}
	//cout << sizeof(char) << endl;

	//Image* img = ImageLoader::LoadImageW(L"111.jpg", EIF_JPEG);
	//ImageWriter::ImageWriterW(img, L"333.jpg", EIF_JPEG);
	//delete img;


	return 0;
}