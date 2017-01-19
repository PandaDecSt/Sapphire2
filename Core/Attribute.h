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
	/// Attribute used for both file serialization and network replication (default).
	static const unsigned AM_DEFAULT = 0x3;
	/// Attribute should use latest data grouping instead of delta update in network replication.
	static const unsigned AM_LATESTDATA = 0x4;
	/// Attribute should not be shown in the editor.
	static const unsigned AM_NOEDIT = 0x8;
	/// Attribute is a node ID and may need rewriting.
	static const unsigned AM_NODEID = 0x10;
	/// Attribute is a component ID and may need rewriting.
	static const unsigned AM_COMPONENTID = 0x20;
	/// Attribute is a node ID vector where first element is the amount of nodes.
	static const unsigned AM_NODEIDVECTOR = 0x40;

	class Serializable;

	/// Abstract base class for invoking attribute accessors.
	class SAPPHIRE_API AttributeAccessor : public RefCounted
	{
	public:
		/// Get the attribute.
		virtual void Get(const Serializable* ptr, Variant& dest) const = 0;
		/// Set the attribute.
		virtual void Set(Serializable* ptr, const Variant& src) = 0;
	};

	/// Description of an automatically serializable variable.
	struct AttributeInfo
	{
		/// Construct empty.
		AttributeInfo() :
			type_(VAR_NONE),
			offset_(0),
			enumNames_(0),
			mode_(AM_DEFAULT),
			ptr_(0)
		{
		}

		/// Construct offset attribute.
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

		/// Construct offset enum attribute.
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

		/// Construct accessor attribute.
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
