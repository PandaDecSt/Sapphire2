#include <iostream>
#include <stdio.h>
#include <string>
#include "Sapphire.h"
#include "Variant.h"
#include "Context.h"
#include "File.h"
#include "Log.h"
#include "Timer.h"
#include "Condition.h"
#include "DebugNew.h"
#include <vector>
#include "FileSystem.h"
#include "WorkQueue.h"
#include "Resource\ResourceCache.h"
#include  "Resource\Image.h"
#include "Test.h"
#include "CoreEvents.h"

//使用SDL文件系统
#include "SDL/include/SDL_filesystem.h"
using namespace std;

SAPPHIRE_EVENT(E_TESTEVENT, tevent)
{
	int i = 1;
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
	uint32 x = (uint32)(u >> 32);  //u高32位
	uint32 y = (uint32)u;          //u 64位
								   //y保存商 x保存余数
	for (int i = 0; i<32; ++i)
	{
		uint32 t = ((int32)x) >> 31;   //取u最高位
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
		std::cout << "创建对象" << this << std::endl;
	}
	~TestSharedPtr()
	{
		std::cout << "删除对象" << this << std::endl;
	}
};



void TestObj()
{
	SharedPtr<Context> context = SharedPtr<Context>(new Context());
	{
		
		//注册对象工厂
		context->RegisterFactory<CustomObj>();
		context->RegisterFactory<CustomEvenTObj>();
		AttributeInfo att;
		att.type_ = VariantType::VAR_INT;
		att.name_ = "width";
		att.mode_ = AM_NET;
		att.offset_ = 0;
		att.ptr_ = 0;
		context->RegisterAttribute<CustomObj>(att);
    	SharedPtr<Object> obj1 = context->CreateObject(CustomObj::GetTypeStatic());
		SharedPtr<Object> obj2 = context->CreateObject(CustomEvenTObj::GetTypeStatic());
		//事件发送
		SharedPtr<CustomObj> cobj1 = DynamicCast<CustomObj>(obj1);
		SharedPtr<CustomEvenTObj> evobj = DynamicCast<CustomEvenTObj>(obj2);
		EventHandlerImpl<CustomEvenTObj>* handler = new EventHandlerImpl<CustomEvenTObj>(evobj.Get(), &Sapphire::CustomEvenTObj::invoke);
		using namespace BeginFrame;
		cobj1->SubscribeToEvent(E_BEGINFRAME, handler);
		SharedPtr<Object> obj3 = context->CreateObject(CustomEvenTObj::GetTypeStatic());
		obj3->SendEvent(E_BEGINFRAME);
		

		AttributeInfo* atinfo = context->GetAttribute<CustomObj>("width");
		cout << atinfo->mode_ << "  name=" << atinfo->mode_ << endl;

	}
	{
		CustomObj* cobj = new CustomObj(context);
		Object* obj = cobj;
		VariantMap dataMap = obj->GetEventDataMap();
		String category = obj->GetCategory();
		cout << obj->GetType().ToString().CString() << endl;
		cout << obj->GetTypeInfo()->GetTypeName().CString() << endl;

	}

	
}


void testFileModule()
{
	Sapphire::SharedPtr<Sapphire::Context> context = SharedPtr<Context>(new Sapphire::Context());
	Sapphire::File*  file = new Sapphire::File(context);
	Sapphire::FileSystem* fs = new Sapphire::FileSystem(context);
	Sapphire::String path = fs->GetCurrentDir();
	path = fs->GetProgramDir();
	path += "test.jpg";
	bool ret = file->Open(path.CString());
	unsigned fileSize = file->GetSize();
	int pos = file->Seek((unsigned)(fileSize - sizeof(unsigned)));
	//新起始偏移地址
	unsigned i = file->ReadUInt();
	unsigned newStartOffset = fileSize - i;
	cout << "" << endl;
	delete fs;
	file->Close();
	delete file;
}





int main()
{
	TestObj();
	testFileModule();
	char* prefPath = SDL_GetPrefPath("SAPPHIRE", "temp");

	using namespace tevent;
	cout << tevent::i << endl;

	{
		Sapphire::Quaternion q1;

		q1.FromAngleAxis(90, Vector3::FORWARD);
		Vector3 V(1, 1, 0);
		Quaternion q2(0, V.x_, V.y_, V.z_);
		Quaternion res = q1*q2*q1.Inverse();
		cout << res.x_ << "  " << res.y_ << "  " << res.z_ << endl;

	}


	{
		Vector3 v1(1, 1, 0);
		Vector3 v2(-1, 1, 0);
		Quaternion q3(v1, v2);
		cout << q3.x_ << "  " << q3.y_ << "  " << q3.z_ << endl;
		Vector3 eularAngle = q3.EulerAngles();
		cout << " eularAngleX:" << eularAngle.x_ << "  eularAngleY:" << eularAngle.y_ << "  eularAngleZ:" << eularAngle.z_ << endl;
	}


	{
		Quaternion q1;
		q1.FromAngleAxis(60.0f, Vector3::FORWARD);
		Quaternion q2;
		q2.FromAngleAxis(135.0f, Vector3::FORWARD);
		Quaternion q3;
		q3 = q1.Slerp(q2, 0.5);
		cout << " q3 angle" << q3.RollAngle() << endl;
		Vector3 v1(0, 1, 0);
		Vector3 v2 = q3*v1;
		cout << v2.ToString().CString() << endl;
		Sapphire::Matrix4 mat4(q3.RotationMatrix());
		cout << mat4.ToString().CString() << endl;


	}

	{
		Vector3 v1(1, 0, 0);
		Vector3 v2(-0.7071, 0.7071, 0);
		Quaternion q(v1, v2);
		Vector3 axis;
		float angle;
		q.ToAngleAxis(angle, axis);
		Vector3 v3 = q*v1;
	}


	Sapphire::Variant v1;
	v1 = Sapphire::String("abas");
	cout << v1.ToString().CString() << endl;

	Sapphire::Variant* v;
	v = new Sapphire::Variant("测试阿萨德的洒落大理石块大声地");
	Sapphire::String str = v->GetString();
	Sapphire::VariantType vt = v->GetType();

	HashMap<Sapphire::String, int> map;
	map.Insert(Pair<String, int>("asd", 1));

	HashMap<String, int>::Iterator it = map.Find("asd");
	cout << it->second_ << endl;

	Condition* c = new Condition();
	Context* ctx = new Context();
	SharedPtr<Context> spctx = SharedPtr<Context>(ctx);
	Log* log = new Log(ctx);
	log->Open("log1.log");
	SharedPtr<Log> spLog = SharedPtr<Log>(log);

	TestSharedPtr* tsp = new TestSharedPtr();
	cout << tsp->Refs() << endl;
	SharedPtr<TestSharedPtr> sp = SharedPtr<TestSharedPtr>(tsp);

	SAPPHIRE_LOGINFO("asad");

	cout << sizeof(Sapphire::String) << v->GetString().CString() << endl;

	//测试union实现变体的原理
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
	delete c;
	char flag = 0;
	SharedPtr<Context> context = SharedPtr<Context>(new Context());
	context->RegisterSubsystem(new FileSystem(context));
	context->RegisterSubsystem(new ResourceCache(context));
	context->RegisterSubsystem(new WorkQueue(context));
	ResourceCache* pResourceCache = context->GetSubsystem<ResourceCache>();
	SharedPtr<Image> img = DynamicCast<Image>(context->CreateObject(Image::GetTypeInfoStatic()->GetType()));
	img->SetName("testImage");
	//pResourceCache->AddManualResource(NULL);
	FileSystem* pFileSys = context->GetSubsystem<FileSystem>();
	String path = pFileSys->GetProgramDir();
	AddTrailingSlash(path);
	path += "resources";
	bool ret = pResourceCache->AddResourceDir(path);
	int i = 0;
	const String name = "test.jpg";
	//异步加载方式
	bool bRet = pResourceCache->BackgroundLoadResource<Image>(name, true);
	while (i < 1000)
	{
		{
			//发送开始帧事件
			using namespace BeginFrame;

			VariantMap& eventData = pResourceCache->GetEventDataMap();
			eventData[P_FRAMENUMBER] = i;
			eventData[P_TIMESTEP] = i * 100;
			pResourceCache->SendEvent(E_BEGINFRAME, eventData);
		}
		//获取资源
		//Image* img = pResourceCache->GetResource<Image>(name, true);	
		String msg;
		PODVector<Image*> pV;
		pResourceCache->GetResources<Image>(pV);
		//同步加载
		//Image* img = pResourceCache->GetResource<Image>(name, true);
		//获取已在缓存中的资源
		Image* img = pResourceCache->GetExistingResource<Image>(name);
		if (img)
		{
			int memSize = img->GetMemoryUse();
			bRet = img->SaveTGA("1.tga");
			msg = "pV size = " + String(pV.Size());
			msg = msg + L"内存占用:" + String(memSize);
		}
		else
		{
			msg = L"加载未成功";
		}
		SAPPHIRE_LOGDEBUG(msg);
		Sleep(100);
		i++;
		
		{
			//发送结束帧
			using namespace EndFrame;
			VariantMap& eventData = pResourceCache->GetEventDataMap();
			pResourceCache->SendEvent(E_ENDFRAME, eventData);

		}
	}
	getchar();
	_CrtDumpMemoryLeaks();
	return 0;
}