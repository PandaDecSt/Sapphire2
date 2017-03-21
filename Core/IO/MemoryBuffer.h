#pragma once 

#include "Serializer.h"
#include "Deserializer.h"


namespace Sapphire
{

	/// ������Ϊһ��Stream����д���ڴ�����
	class SAPPHIRE_API MemoryBuffer : public Deserializer, public Serializer
	{
	public:
		/// ��һ��ָ��ʹ�С����
		MemoryBuffer(void* data, unsigned size);
		/// ����һ��ֻ���Ļ�����
		MemoryBuffer(const void* data, unsigned size);
	    ///��һ��Vector����
		MemoryBuffer(PODVector<unsigned char>& data);
		///��һ��Vector����
		MemoryBuffer(const PODVector<unsigned char>& data);

		/// ���ڴ������ȡ�ֽ���������ʵ�ʶ�ȡ���ֽ���
		virtual unsigned Read(void* dest, unsigned size);
		/// �����ڴ��������ʼλ�� 
		virtual unsigned Seek(unsigned position);
		/// д�ֽڵ��ڴ�����
		virtual unsigned Write(const void* data, unsigned size);

		/// �����ڴ�����ָ��
		unsigned char* GetData() { return buffer_; }

		/// �����Ƿ�ֻ��
		bool IsReadOnly() { return readOnly_; }

	private:
		/// �ڴ�����ָ��
		unsigned char* buffer_;
		/// �Ƿ�ֻ��
		bool readOnly_;
	};
}