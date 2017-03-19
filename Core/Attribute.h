#pragma once

#include "Ptr.h"
#include "Variant.h"

namespace Sapphire
{

	//ֻ�ڱ༭������ʾ�����ԣ��������л�
	static const unsigned AM_EDIT = 0x0;
	//�����ļ����л�������
	static const unsigned AM_FILE = 0x1;
	//�������縴�Ƶ�����
	static const unsigned AM_NET = 0x2;
	/// ͬʱ�����ļ����л������縴�Ƶ����ԣ�Ĭ�ϣ�
	static const unsigned AM_DEFAULT = 0x3;
	/// ʹ�����µ���������������縴���е���������
	static const unsigned AM_LATESTDATA = 0x4;
	/// ���ڱ༭������ʾ������
	static const unsigned AM_NOEDIT = 0x8;
	/// ������һ���ڵ�ID���ҿ�����Ҫ��д
	static const unsigned AM_NODEID = 0x10;
	/// ������һ�����ID���ҿ�����Ҫ��д
	static const unsigned AM_COMPONENTID = 0x20;
	/// ������һ���ڵ�ID�������׸�Ԫ���ǽڵ���
	static const unsigned AM_NODEIDVECTOR = 0x40;

	class Serializable;

	/// ִ�����Է������ĳ������
	class SAPPHIRE_API AttributeAccessor : public RefCounted
	{
	public:
		/// ��ȡ����
		virtual void Get(const Serializable* ptr, Variant& dest) const = 0;
		/// ��������
		virtual void Set(Serializable* ptr, const Variant& src) = 0;
	};

	/// �Զ����л��ɱ����͵�������Ϣ
	struct AttributeInfo
	{
		
		AttributeInfo() :
			type_(VAR_NONE),
			offset_(0),
			enumNames_(0),
			mode_(AM_DEFAULT),
			ptr_(0)
		{
		}

		/// ����ƫ������ 
		AttributeInfo(VariantType type, const char* name, size_t offset, const Variant& defaultValue, unsigned mode) :
			type_(type),
			name_(name),
			offset_((unsigned)offset),
			enumNames_(0),
			defaultValue_(defaultValue),
			mode_(mode),
			ptr_(0)
		{
		}

		/// ����ƫ��ö������
		AttributeInfo(const char* name, size_t offset, const char** enumNames, const Variant& defaultValue, unsigned mode) :
			type_(VAR_INT),
			name_(name),
			offset_((unsigned)offset),
			enumNames_(enumNames),
			defaultValue_(defaultValue),
			mode_(mode),
			ptr_(0)
		{
		}

		/// �������������
		AttributeInfo(VariantType type, const char* name, AttributeAccessor* accessor, const Variant& defaultValue, unsigned mode) :
			type_(type),
			name_(name),
			offset_(0),
			enumNames_(0),
			accessor_(accessor),
			defaultValue_(defaultValue),
			mode_(mode),
			ptr_(0)
		{
		}

		 
		// ���������ö������
		AttributeInfo(const char* name, AttributeAccessor* accessor, const char** enumNames, const Variant& defaultValue,
			unsigned mode) :
			type_(VAR_INT),
			name_(name),
			offset_(0),
			enumNames_(enumNames),
			accessor_(accessor),
			defaultValue_(defaultValue),
			mode_(mode),
			ptr_(0)
		{
		}

		//��������
		VariantType type_;
		//������
		String name_;
		 
		// �������ʼƫ���ֽ���
		unsigned offset_;
		/// ö����
		const char** enumNames_;
		/// ����ģʽ�İ�������
		SharedPtr<AttributeAccessor> accessor_;
		// ���縴�Ƶ�Ĭ��ֵ
		Variant defaultValue_;
		/// ����ģʽ���Ƿ�ʹ�����л������縴�ƣ���һ��ʹ��
		unsigned mode_;
		// �����л�֮��ĵط�ʹ����������ָ��
		void* ptr_;
	};
}
