#pragma once

#include "Image.h"

namespace Sapphire
{
	/// ��ѹDXT��RGBA
	SAPPHIRE_API void
		DecompressImageDXT(unsigned char* dest, const void* blocks, int width, int height, int depth, CompressedFormat format);
	/// ��ѹETC1��RGBA
	SAPPHIRE_API void DecompressImageETC(unsigned char* dest, const void* blocks, int width, int height);
	/// ��ѹPVRTC��RGBA
	SAPPHIRE_API void DecompressImagePVRTC(unsigned char* dest, const void* blocks, int width, int height, CompressedFormat format);
	/// ��ֱ��תһ��ѹ����
	SAPPHIRE_API void FlipBlockVertical(unsigned char* dest, unsigned char* src, CompressedFormat format);
	/// ˮƽ��תһ��ѹ����
	SAPPHIRE_API void FlipBlockHorizontal(unsigned char* dest, unsigned char* src, CompressedFormat format);
}