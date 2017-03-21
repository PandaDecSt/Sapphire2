#pragma once

#include "../Object.h"


namespace  Sapphire
{

	/// ����ļ����
	struct PackageEntry
	{
		/// ��ʼƫ�Ƶ�ַ
		unsigned offset_;
		/// �ļ���С
		unsigned size_;
		/// �ļ�У���
		unsigned checksum_;
	};

	/// ����Ŀ¼����һ���ļ���
	class SAPPHIRE_API PackageFile : public Object
	{
		SAPPHIRE_OBJECT(PackageFile, Object);

	public:
		 
		PackageFile(Context* context);
		/// ���첢��
		PackageFile(Context* context, const String& fileName, unsigned startOffset = 0);
		///  ����
		virtual ~PackageFile();

		/// �򿪰��ļ��� ���Ϊ�ɹ�true
		bool Open(const String& fileName, unsigned startOffset = 0);
		/// �����ļ��Ƿ���ڡ� �ļ�����Сд����ȡ����ƽ̨
		bool Exists(const String& fileName) const;
		/// �ҵ��������ֵ��ļ���ڡ� ���Ϊ��û�ҵ�
		const PackageEntry* GetEntry(const String& fileName) const;

		/// ���������ļ����
		const HashMap<String, PackageEntry>& GetEntries() const { return entries_; }

		/// ���ذ��ļ���
		const String& GetName() const { return fileName_; }

		/// ���ذ��ļ�����hashֵ
		StringHash GetNameHash() const { return nameHash_; }

		/// �����ļ���
		unsigned GetNumFiles() const { return entries_.Size(); }

		/// ���ذ��ļ����ܴ�С
		unsigned GetTotalSize() const { return totalSize_; }

		/// ���ذ��ļ����ݵ�У���
		unsigned GetChecksum() const { return checksum_; }

		/// �����ļ��Ƿ�ѹ��
		bool IsCompressed() const { return compressed_; }

		/// �����ڰ��е��ļ��б�
		const Vector<String> GetEntryNames() const { return entries_.Keys(); }

	private:
		/// �ļ���ڱ�
		HashMap<String, PackageEntry> entries_;
		/// �ļ���
		String fileName_;
		/// ���ļ���hash
		StringHash nameHash_;
		/// ���ļ��ܴ�С
		unsigned totalSize_;
		/// ���ļ�У���
		unsigned checksum_;
		/// ѹ����־
		bool compressed_;
	};
}