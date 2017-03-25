#pragma once

#include "Sapphire.h"

namespace Sapphire
{

	class Deserializer;
	class Serializer;
	class VectorBuffer;

	/// ���ڸ����������С���㲢�ҷ���������LZ4 ѹ�������С
	SAPPHIRE_API unsigned EstimateCompressBound(unsigned srcSize);
	/// ��LZ4ѹ���㷨ѹ�����ݲ��ҷ���ѹ�����ݵĴ�С.����ҪĿ�껺����EstimateCompressBound()�����Ĵ�С
	SAPPHIRE_API unsigned CompressData(void* dest, const void* src, unsigned srcSize);
	/// ��LZ4�㷨��ѹ�����ݡ� δѹ�����ݵĴ�С����֪���������Ѿ���ѹ�����ݴ�С
	SAPPHIRE_API unsigned DecompressData(void* dest, const void* src, unsigned destSize);
	/// ѹ��һ����(��Դ��Ŀ��)���ɹ�����true
	SAPPHIRE_API bool CompressStream(Serializer& dest, Deserializer& src);
	/// ��ѹ��һ������Ŀ�������ɹ�����true
	SAPPHIRE_API bool DecompressStream(Serializer& dest, Deserializer& src);
	/// ��LZ4�㷨ѹ��һ���ɱ仺����������ѹ�����������
	SAPPHIRE_API VectorBuffer CompressVectorBuffer(VectorBuffer& src);
	/// ��ѹ��һ���ɱ仺����
	SAPPHIRE_API VectorBuffer DecompressVectorBuffer(VectorBuffer& src);


}