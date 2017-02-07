#pragma once
#include "stdafx.h"


//! һ���������ɫ��ʽ��ö��
enum ECOLOR_FORMAT
{
	//! 16λɫ�ʸ�ʽ�������������
	/** ���������Ϊ��������������Ƶ��������ѡ
	ÿ����ɫ������5λ��alpha��Ϣ�ɵ���λ��� */
	ECF_A1R5G5B5 = 0,

	//! ��׼��16λ��ɫ��ʽ
	ECF_R5G6B5,

	//! ��׼��24λ��ɫ��ʽ��û��alphaͨ������������8λ
	ECF_R8G8B8,

	//! 32λ��ɫ��ʽ,��������8λ
	ECF_R8G8B8A8,

	//! Ĭ�ϵ�32λ��ɫ��ʽ��ÿ��������8λ
	ECF_A8R8G8B8,

	/** �����ʽ�����и�ʽֻ��������ȾĿ������ */

	//! 16λ�����ʽ��ʹ��16λ�ĺ�ɫͨ��
	ECF_R16F,

	//! 32λ�����ʽ����16λ��ɫͨ����16λ��ɫͨ��
	ECF_G16R16F,

	//! 64λ�����ʽ����16λ�죬�̣�����alphaͨ��
	ECF_A16B16G16R16F,

	//! 32λ�����ʽ����32λ�ĺ�ɫͨ��
	ECF_R32F,

	//! 64λ�����ʽ����32λ�ĺ�ɫ����ɫͨ��
	ECF_G32R32F,

	//! 128λ�����ʽ����32λ�ĺ�������͸����ʽ
	ECF_A32B32G32R32F,


	ECF_UNKNOWN
};


//! ����һ��16λ��A1R5G5B5��ɫ
inline uint16 RGBA16(uint32 r, uint32 g, uint32 b, uint32 a = 0xFF)
{
	return (uint16)((a & 0x80) << 8 |
		(r & 0xF8) << 7 |
		(g & 0xF8) << 2 |
		(b & 0xF8) >> 3);
}


//! ����һ��16λ��A1R5G5B5��ɫ 
inline uint16 RGB16(uint32 r, uint32 g, uint32 b)
{
	return RGBA16(r, g, b);
}


//!  ����һ��16λ��A1R5G5B5��ɫ ,����16λ����ֵ
inline uint16 RGB16from16(uint16 r, uint16 g, uint16 b)
{
	return (0x8000 |
		(r & 0x1F) << 10 |
		(g & 0x1F) << 5 |
		(b & 0x1F));
}


//! ת��һ��32λ(X8R8G8B8)��ɫ��16λ��A1R5G5B5
inline uint16 X8R8G8B8toA1R5G5B5(uint32 color)
{
	return (uint16)(0x8000 |
		(color & 0x00F80000) >> 9 |
		(color & 0x0000F800) >> 6 |
		(color & 0x000000F8) >> 3);
}


//! ת��һ��32λ(A8R8G8B8)��ɫ��16λ��A1R5G5B5
inline uint16 A8R8G8B8toA1R5G5B5(uint32 color)
{
	return (uint16)((color & 0x80000000) >> 16 |
		(color & 0x00F80000) >> 9 |
		(color & 0x0000F800) >> 6 |
		(color & 0x000000F8) >> 3);
}


//! ת��һ��32λ(A8R8G8B8) ��ɫ��16λ��R5G6B5
inline uint16 A8R8G8B8toR5G6B5(uint32 color)
{
	return (uint16)((color & 0x00F80000) >> 8 |
		(color & 0x0000FC00) >> 5 |
		(color & 0x000000F8) >> 3);
}

//! ת��һ��32λ(A8R8G8B8) ��ɫ��32λ��R8G8B8A8
inline uint16 A8R8G8B8toR8G8B8A8(uint32 color)
{
	return (uint32)((color & 0x00FF0000) << 8 |
		(color & 0x0000FF00) << 8 |
		(color & 0x000000FF) << 8 |
		(color & 0xFF000000) >> 24
		);
}


//! ��A1R5G5B5ת����A8R8G8B8
inline uint32 A1R5G5B5toA8R8G8B8(uint16 color)
{
	return (((-((sint32)color & 0x00008000) >> (sint32)31) & 0xFF000000) |
		((color & 0x00007C00) << 9) | ((color & 0x00007000) << 4) |
		((color & 0x000003E0) << 6) | ((color & 0x00000380) << 1) |
		((color & 0x0000001F) << 3) | ((color & 0x0000001C) >> 2)
		);
}


//! ����R5G6B5��A8R8G8B8����ɫ
inline uint32 R5G6B5toA8R8G8B8(uint16 color)
{
	return 0xFF000000 |
		((color & 0xF800) << 8) |
		((color & 0x07E0) << 5) |
		((color & 0x001F) << 3);
}


//! ���ش�A1R5G5B5��R5G6B5����ɫ 
inline uint16 R5G6B5toA1R5G5B5(uint16 color)
{
	return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
}


//! ����A1R5G5B5��R5G6B5����ɫ  
inline uint16 A1R5G5B5toR5G6B5(uint16 color)
{
	return (((color & 0x7FE0) << 1) | (color & 0x1F));
}



//! ����A1R5G5B5��alpha����

inline uint32 getAlpha(uint16 color)
{
	return ((color >> 15) & 0x1);
}


//! ����A1R5G5B5�ĺ�ɫ����
/** Shift left by 3 to get 8 bit value. */
inline uint32 getRed(uint16 color)
{
	return ((color >> 10) & 0x1F);
}


//! ����A1R5G5B5����ɫ����
/** Shift left by 3 to get 8 bit value. */
inline uint32 getGreen(uint16 color)
{
	return ((color >> 5) & 0x1F);
}


//! ����A1R5G5B5����ɫ����
/** Shift left by 3 to get 8 bit value. */
inline uint32 getBlue(uint16 color)
{
	return (color & 0x1F);
}


//! ����16λA1R5G5B5��ƽ��ֵ
inline sint32 getAverage(sint32 color)
{
	return ((getRed(color) << 3) + (getGreen(color) << 3) + (getBlue(color) << 3)) / 3;
}

////32λRGBA
typedef uint32 RGBA;
////32λARGB
typedef uint32 ARGB;
////32λABGR
typedef uint32 ABGR;
////32λBGRA
typedef uint32 BGRA;



// 1 - λalpha���
inline uint16 PixelBlend16(const uint16 c2, const uint16 c1)
{
	uint16 mask = ((c1 & 0x8000) >> 15) + 0x7fff;
	return (c2 & mask) | (c1 & ~mask);
}

// 1 - λalpha��� 16Bit SIMD
inline uint32 PixelBlend16_simd(const uint32 c2, const uint32 c1)
{
	uint32 mask = ((c1 & 0x80008000) >> 15) + 0x7fff7fff;
	return (c2 & mask) | (c1 & ~mask);
}

//32λ����Alpha���
/*!
Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
*/
inline uint32 PixelBlend32(const uint32 c2, const uint32 c1)
{
	// alpha test
	uint32 alpha = c1 & 0xFF000000;

	if (0 == alpha)
		return c2;

	if (0xFF000000 == alpha)
	{
		return c1;
	}

	alpha >>= 24;

	// ���alpha��λ�� ���(alpha > 127) alpha += 1
	alpha += (alpha >> 7);

	uint32 srcRB = c1 & 0x00FF00FF;  //Դ������
	uint32 srcXG = c1 & 0x0000FF00;   //Դ����

	uint32 dstRB = c2 & 0x00FF00FF;  //Դ������
	uint32 dstXG = c2 & 0x0000FF00;  //Դ����


	uint32 rb = srcRB - dstRB;    
	uint32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return (c1 & 0xFF000000) | rb | xg;
}

inline void convert_A8R8G8B8toR8G8B8(const void* sP, sint32 sN, void* dP)
{
	uint8* sB = (uint8*)sP;
	uint8* dB = (uint8*)dP;

	for (sint32 x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}

class Color
{
public:

	
	
	Color() :r(1.0f), g(1.0f), b(1.0f), a(1.0f)
	{
	}
	Color(const Color& other) :r(other.r), g(other.g), b(other.b), a(other.a)
	{
	}
	Color(float _r,float _g, float _b, float _a) :a(_a), r(_r), g(_g), b(_b)
	{

	}
	Color(uint32 alpha, uint32 red, uint32 green, uint32 blue)
	{
		setAlpha(alpha);
		setRed(red);
		setGreen(green);
		setBlue(blue);
	}
	Color(uint32 argb)
	{
		uint32 val32 = argb;

		// ת����32λģʽ
		// (ARGB = 8888)

		// Alpha
		a = ((val32 >> 24) & 0xFF) / 255.0f;

		// Red
		r = ((val32 >> 16) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 8) & 0xFF) / 255.0f;

		// Blue
		b = (val32 & 0xFF) / 255.0f;
	}

	~Color()
	{}

	Color& operator=(const Color& other)
	{
		this->r = other.r;
		this->g = other.g;
		this->b = other.b;
		this->a = other.a;
		return *this;
	}

	Color operator+(const Color& other)
	{
		return Color(a + other.a, r + other.r, g + other.g, b + other.b);
	}

	Color operator*(const Color& other)
	{
		return Color(a * other.a, r * other.r, g * other.g, b * other.b);
	}

	Color operator*(const float scalar)
	{
		return Color(a * scalar, r * scalar, g * scalar, b * scalar);
	}

	Color operator/(const float scalar)
	{
		assert(scalar != 0.0f);
		return Color(a / scalar, r / scalar, g / scalar, b / scalar);
	}

	Color operator/(const Color& other)
	{
		return Color(a / other.a, r / other.r, g / other.g, b / other.b);
	}

	Color operator-(const Color& other)
	{
		return Color(a - other.a, r - other.r, g - other.g, b - other.b);
	}

	bool  operator==(const Color& other)
	{
		return a == other.a && r == other.r && g == other.g && b == other.b;
	}

	bool  operator!=(const Color& other)
	{
		return a != other.a || r != other.r || g != other.g || b != other.b;
	}

	void setAlpha(uint32 alpha)
	{
		a = static_cast<float>(alpha) / 255;
	};

	void setRed(uint32 red)
	{
		r = static_cast<float>(red) / 255;
	};

	void setGreen(uint32 green)
	{
		g = static_cast<float>(green) / 255;
	}

	void setBlue(uint32 blue)
	{
		b = static_cast<float>(blue) / 255;
	};
	
	uint32 getAlpha() const
	{
		return static_cast<uint32>(a * 255);
	}

	uint32 getRed() const
	{
		return static_cast<uint32>(r * 255);
	}

	uint32 getGreen() const
	{
		return static_cast<uint32>(g * 255);
	}

	uint32 getBlue() const
	{
		return static_cast<uint32>(b * 255);
	}

	uint32 getAverage()
	{
		return (getRed() + getGreen() + getBlue()) / 3;
	}

	//! ת����OpenGL��ɫ��ʽ
	/** ��ARGB��RGBA
	*/
	void toOpenGLColor(uint8* dest) const
	{
		*dest = (uint8)getRed();
		*++dest = (uint8)getGreen();
		*++dest = (uint8)getBlue();
		*++dest = (uint8)getAlpha();
	}

	void saturate(float min, float max);


	/** ����RGBA
	*/
	RGBA getAsRGBA(void) const;

	/** ����ARGB
	*/
	ARGB getAsARGB(void) const;

	/**����BGRA
	*/
	BGRA getAsBGRA(void) const;

	/** ����ABGR */
	ABGR getAsABGR(void) const;

	/** ���� RGBA.
	*/
	void setAsRGBA(const RGBA val);

	/** ���� ARGB.
	*/
	void setAsARGB(const ARGB val);

	/**���� BGRA.
	*/
	void setAsBGRA(const BGRA val);

	/** ���� ABGR.
	*/
	void setAsABGR(const ABGR val);


	void setData(const void *data, ECOLOR_FORMAT format)
	{
		uint32 color;
		switch (format)
		{
		case ECF_A1R5G5B5:
			color = A1R5G5B5toA8R8G8B8(*(uint16*)data);
			break;
		case ECF_R5G6B5:
			color = R5G6B5toA8R8G8B8(*(uint16*)data);
			break;
		case ECF_A8R8G8B8:
			color = *(uint32*)data;
			break;
		case ECF_R8G8B8:
		{
			uint8* p = (uint8*)data;
			//set(255, p[0], p[1], p[2]);
			color = ((255 & 0xff) << 24) | ((p[0] & 0xff) << 16) | ((p[1] & 0xff) << 8) | (p[2] & 0xff);
		}
		break;
		default:
			color = 0xffffffff;
			break;
		}
		setAsARGB(color);
	}

	//! ��ָ������ɫ��ʽд��һ����ɫ����
	/** \param data: Ŀ�����ɫ���ݡ���������㹻�������һ�����ָ����ʽ����ɫ����
	\param format: ����Ҫ��ʲô��ɫ����
	*/
	void getData(void *data, ECOLOR_FORMAT format)
	{
		uint32 color = getAsARGB();
		switch (format)
		{
		case ECF_A1R5G5B5:
		{
			uint16 * dest = (uint16*)data;
			*dest = A8R8G8B8toA1R5G5B5(color);
		}
		break;

		case ECF_R5G6B5:
		{
			uint16 * dest = (uint16*)data;
			*dest = A8R8G8B8toR5G6B5(color);
		}
		break;

		case ECF_R8G8B8:
		{
			uint8* dest = (uint8*)data;
			dest[0] = (uint8)getRed();
			dest[1] = (uint8)getGreen();
			dest[2] = (uint8)getBlue();
		}
		break;

		case ECF_A8R8G8B8:
		{
			uint32 * dest = (uint32*)data;
			*dest = color;
		}
		break;

		default:
			break;
		}
	}

	float r;
	float g;
	float b;
	float a;

	//���峣����ɫ
	static const Color ZERO;
	static const Color Black;   //��   
	static const Color White;    //��
	static const Color Red;      //��
	static const Color Green;     //��
	static const Color Blue;        //��

private:

	

};