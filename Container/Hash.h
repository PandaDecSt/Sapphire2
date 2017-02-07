#pragma once

#include <cstddef>

namespace Sapphire
{

	/// Pointer hash function.
	//hash����ָ��    ��ϣֵ = �����ַ/�������ʹ�С
	template <class T> unsigned MakeHash(T* value)
	{
		return (unsigned)((size_t)value / sizeof(T));
	}

	/// Const pointer hash function.
	template <class T> unsigned MakeHash(const T* value)
	{
		return (unsigned)((size_t)value / sizeof(T));
	}

	/// Generic hash function.
	// ͨ��hash����
	template <class T> unsigned MakeHash(const T& value)
	{
		return value.ToHash();
	}

	/// Void pointer hash function.
	// void����hashָ��
	template <> inline unsigned MakeHash(void* value)
	{
		return (unsigned)(size_t)value;
	}

	/// Const void pointer hash function.
	template <> inline unsigned MakeHash(const void* value)
	{
		return (unsigned)(size_t)value;
	}

	/// Long long hash function.
	//  long long ��ϣ����
	//  ȡ��32λ
	template <> inline unsigned MakeHash(const long long& value)
	{
		return (unsigned)((value >> 32) | (value & 0xffffffff));
	}

	/// Unsigned long long hash function.
	//  unsigned long long ��ϣ����
	//  ȡ��32λ
	template <> inline unsigned MakeHash(const unsigned long long& value)
	{
		return (unsigned)((value >> 32) | (value & 0xffffffff));
	}

	/// Int hash function.
	//  int ��ϣ����
	template <> inline unsigned MakeHash(const int& value)
	{
		return (unsigned)value;
	}

	/// Unsigned hash function.
	//  unsigned ��ϣ����
	template <> inline unsigned MakeHash(const unsigned& value)
	{
		return value;
	}

	/// Short hash function.
	// short ��ϣ����
	template <> inline unsigned MakeHash(const short& value)
	{
		return (unsigned)value;
	}

	/// Unsigned short hash function.
	//  unsigned short ��ϣ����
	template <> inline unsigned MakeHash(const unsigned short& value)
	{
		return value;
	}

	/// Char hash function.
	//  char ��ϣ����
	template <> inline unsigned MakeHash(const char& value)
	{
		return (unsigned)value;
	}

	/// Unsigned char hash function.
	//  unsigned char ��ϣ����
	template <> inline unsigned MakeHash(const unsigned char& value)
	{
		return value;
	}
}