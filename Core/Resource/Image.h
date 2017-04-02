#pragma once


#include "ArrayPtr.h"
#include "Resource.h"

struct SDL_Surface;

namespace Sapphire
{
	static const int COLOR_LUT_SIZE = 16;

	/// ֧�ֵ�ѹ��ͼ���ʽ
	enum CompressedFormat
	{
		CF_NONE = 0,
		CF_RGBA,
		CF_DXT1,
		CF_DXT3,
		CF_DXT5,
		CF_ETC1,
		CF_PVRTC_RGB_2BPP,
		CF_PVRTC_RGBA_2BPP,
		CF_PVRTC_RGB_4BPP,
		CF_PVRTC_RGBA_4BPP,
	};

	/// ѹ��ͼ��mip����
	struct CompressedLevel
	{
		 
		CompressedLevel() :
			data_(0),
			width_(0),
			height_(0),
			depth_(0),
			blockSize_(0),
			dataSize_(0),
			rowSize_(0),
			rows_(0)
		{
		}

		/// ��ѹ��RGBA�� Ŀ�껺����������width * height * 4 bytes. ����ɹ�����true
		bool Decompress(unsigned char* dest);

		/// ѹ����ͼ������
		unsigned char* data_;
		/// ѹ����ʽ
		CompressedFormat format_;
		 
		int width_;
		 
		int height_;
	 
		int depth_;
		/// ���С
		unsigned blockSize_;
		/// �ܵĿ��С
		unsigned dataSize_;
		/// �д�С
		unsigned rowSize_;
		/// ����
		unsigned rows_;
	};

	/// %Image ��Դ.
	class SAPPHIRE_API Image : public Resource
	{
		SAPPHIRE_OBJECT(Image, Resource);

	public:
		 
		Image(Context* context);
		 
		virtual ~Image();
		/// ע����󹤳�
		static void RegisterObject(Context* context);

		/// �����м�����Դ�� 
		virtual bool BeginLoad(Deserializer& source);
		/// ����ͼ��һ�����С� (����ԭ���ĸ�ʽ��ͼƬ����Ϊpng����֧��ѹ��ͼ�����ݣ�
		virtual bool Save(Serializer& dest) const;

		/// ����2d��С����ɫ���������.�ɵ�ͼ�����ݻᱻ�ƻ������µ�ͼ�����ݻ���δ���塣
		bool SetSize(int width, int height, unsigned components);
		///  ����3d��С����Ⱥ���ɫ���������.�ɵ�ͼ�����ݻᱻ�ƻ������µ�ͼ�����ݻ���δ���塣
		bool SetSize(int width, int height, int depth, unsigned components);
		///  �����µ�ͼ������
		void SetData(const unsigned char* pixelData);
		/// ����һ��2d���ص���ɫ
		void SetPixel(int x, int y, const Color& color);
		/// ����һ��3d���ص���ɫ
		void SetPixel(int x, int y, int z, const Color& color);
		/// ����һ��2d���ص�������ɫ��R���������8λ
		void SetPixelInt(int x, int y, unsigned uintColor);
		/// ����һ��3d���ص�������ɫ��R���������8λ
		void SetPixelInt(int x, int y, int z, unsigned uintColor);
		/// ������ɫ���ұ�
		bool LoadColorLUT(Deserializer& source);
		/// ˮƽ��תͼ��
		bool FlipHorizontal();
		/// ��ֱ��תͼ��
		bool FlipVertical();
		///  ��˫�������²����ı�ͼ���С
		bool Resize(int width, int height);
		/// ��һ����ɫ���ͼ��
		void Clear(const Color& color);
		/// ��һ��������ɫ���ͼ��.R���������8λ
		void ClearInt(unsigned uintColor);
		 
		bool SaveBMP(const String& fileName) const;
		 
		bool SavePNG(const String& fileName) const;
		 
		bool SaveTGA(const String& fileName) const;
		 
		bool SaveJPG(const String& fileName, int quality) const;
		/// �������Ƿ�������������ͼ��ֻ���DDS
		bool IsCubemap() const { return cubemap_; }
		/// �������Ƿ�����volume texture�� ֻ���DDS
		bool IsArray() const { return array_; }
		/// �������Ƿ���sRGB��ɫ�ռ䣬ֻ���DDS
		bool IsSRGB() const { return sRGB_; }

		/// ����һ��2d������ɫ
		Color GetPixel(int x, int y) const;
		/// ����һ��2d������ɫ
		Color GetPixel(int x, int y, int z) const;
		/// ����һ������2d������ɫ  R���������8λ
		unsigned GetPixelInt(int x, int y) const;
		/// ����һ������3d������ɫ  R���������8λ
		unsigned GetPixelInt(int x, int y, int z) const;
		/// ����һ��˫���Բ�����2d������ɫ�� X��Y�ķ�Χ0-1
		Color GetPixelBilinear(float x, float y) const;
		/// ����һ�������Բ�����3d������ɫ�� X��Y�ķ�Χ0-1
		Color GetPixelTrilinear(float x, float y, float z) const;

		int GetWidth() const { return width_; }

		int GetHeight() const { return height_; }

		int GetDepth() const { return depth_; }

		/// ������ɫ������
		unsigned GetComponents() const { return components_; }

		/// ������������ָ��
		unsigned char* GetData() const { return data_; }

		/// �����Ƿ�ѹ��
		bool IsCompressed() const { return compressedFormat_ != CF_NONE; }

		/// ����ѹ����ʽ 
		CompressedFormat GetCompressedFormat() const { return compressedFormat_; }

		/// ����ѹ��mip ������
		unsigned GetNumCompressedLevels() const { return numCompressedLevels_; }

		/// ������һ��˫�����˲���mip����
		SharedPtr<Image> GetNextLevel() const;
		/// ������������ͼ������һ���ٽ����ֵ�ͼ��
		SharedPtr<Image> GetNextSibling() const { return nextSibling_; }
		/// ����ͼ��ת����4����RGBA.
		SharedPtr<Image> ConvertToRGBA() const;
		/// ����һ��ѹ����mip����
		CompressedLevel GetCompressedLevel(unsigned index) const;
		/// ��ȡ��һ��ͼ���ж�����������ͼ�����ʧ�ܷ���null.��֧��3dͼ�񡣱����ֶ��ͷ���ͼ��
		Image* GetSubimage(const IntRect& rect) const;
		/// ��һ��ͼ�񷵻�SDL surface�����ʧ�ܷ���null�� ֻ֧��RGBͼ�� ָ�����򽫷��ز���ͼ�񣬱����ֶ��ͷ��ڴ�
		SDL_Surface* GetSDLSurface(const IntRect& rect = IntRect::ZERO) const;
		/// Ԥ�ȼ���mip���� �����첽�������
		void PrecalculateLevels();

	private:
		/// ��stb_image ����ͼ��
		static unsigned char* GetImageData(Deserializer& source, int& width, int& height, unsigned& components);
		/// �ͷ�һ��ͼ���ļ�����������
		static void FreeImageData(unsigned char* pixelData);

		int width_;

		int height_;

		int depth_;
		/// ��ɫ������
		unsigned components_;
		/// ѹ��mip������
		unsigned numCompressedLevels_;
		/// �����dds��������״̬
		bool cubemap_;
		/// ��������״̬ DDS
		bool array_;
		/// ������sRGB
		bool sRGB_;
		/// ѹ����ʽ
		CompressedFormat compressedFormat_;
		/// ��������
		SharedArrayPtr<unsigned char> data_;
		/// Ԥ����mip����ͼ��
		SharedPtr<Image> nextLevel_;
		/// ��һ���������ݻ�����������ͼ��
		SharedPtr<Image> nextSibling_;
	};
}