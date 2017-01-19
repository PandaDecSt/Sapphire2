#include <iostream>
#include <string>
#include "Sapphire.h"
#include "Variant.h"
#include "Context.h"
#include "File.h"
#include "Log.h"
#include "Timer.h"
#include "Condition.h"

using namespace std;

int test()
{
	cout << "test" << endl;
	return 5;
}


struct TestS
{
	union
	{
		int int1;
		float float1;
		bool b1;
		void* ptr_1;

	};

	union
	{
		int int2;
		float float2;
		bool b1;
		void* ptr_2;
	};
	union
	{
		int int3;
		float float3;
		bool b3;
		void* ptr_3;
	};
	union
	{
		int int4;
		float float4;
		bool b4;
		void* ptr_4;
	};
};

struct TestClass
{
	int a;
	float b;
	float c;
};

#define uint32 unsigned int
#define int32  int
#define uint64 unsigned long long

uint32 div(uint64 u, uint32 z) //return u/z 
{
	uint32 x = (uint32)(u >> 32);  //u��32λ
	uint32 y = (uint32)u;          //u 64λ
	//y������ x��������
	for (int i = 0; i<32; ++i)
	{
		uint32 t = ((int32)x) >> 31;   //ȡu���λ
		x = (x << 1) | (y >> 31);
		y = y << 1;
		if ((x | t) >= z)
		{
			x -= z;
			++y;
		}
	}
	return y;
}



using namespace std;
using namespace Sapphire;


class TestSharedPtr : public RefCounted
{
public:
	TestSharedPtr()
	{
		AddRef();
		std::cout << "��������" << this << std::endl;
	}
	~TestSharedPtr()
	{
		std::cout << "ɾ������" << this << std::endl;
	}
};
 

int main()
{
	typedef int(*func)(void);
	func f = test;

	Sapphire::Variant* v;
	v = new Sapphire::Variant("���԰����µ��������ʯ�������");
	Sapphire::String str = v->GetString();
	Sapphire::VariantType vt = v->GetType();
	
	HashMap<Sapphire::String, int> map;
	map.Insert(Pair<String, int>("asd", 1));

	HashMap<String,int>::Iterator it = map.Find("asd");
	cout << it->second_ << endl;

	Condition* c = new Condition();
	Context* ctx = new Context();
	Log* log = new Log(ctx);
	log->Open("log1.log");

	TestSharedPtr* tsp = new TestSharedPtr();
	cout << tsp->Refs() << endl;
	tsp->ReleaseRef();

	SAPPHIRE_LOGINFO("asad");
	

	cout << sizeof(Sapphire::String) << v->GetString().CString() << endl;

	//����unionʵ�ֱ����ԭ��
	TestClass* t1 = new TestClass();
	t1->a = 5;
	t1->b = 12.5;
	t1->c = 2491.2546718;

	TestS* ts = reinterpret_cast<TestS*>(t1);
	int s = sizeof(unsigned long long);
	cout << s << endl;
	;
	uint64 u1 = 65536;
	uint32 u2 = 16;
	//
	cout << div(u1, u2) << endl;
	
	delete v;

	getchar();
	return f();
}