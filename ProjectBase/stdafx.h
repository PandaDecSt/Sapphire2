#pragma once
#include <string>
#include <assert.h>

using namespace std;



typedef unsigned int  uint32;
typedef unsigned long ulong;
typedef unsigned long long uint64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int            sint32;
typedef long long      sint64;
typedef short          sint16;
typedef char           sint8;


//! �����ֵ��ڴ�����
//  
inline void memset16bit(void * dest, const uint16 value, uint32 bytesize)
{
	uint16 * d = (uint16*)dest;

	uint32 i;

	// һ�θ���8*2=16���ֽ�
	i = bytesize >> (1 + 3);
	while (i)
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		--i;
	}

	i = (bytesize >> 1) & 7;
	while (i)
	{
		d[0] = value;
		++d;
		--i;
	}
}

inline void memset32bit(void * dest, const uint32 value, uint32 bytesize)
{
	uint32 * d = (uint32*)dest;

	uint32 i;

	i = bytesize >> (2 + 3);
	while (i)
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		i -= 1;
	}

	i = (bytesize >> 2) & 7;
	while (i)
	{
		d[0] = value;
		d += 1;
		i -= 1;
	}
}