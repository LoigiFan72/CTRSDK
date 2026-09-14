#pragma once

#include <nn/dbm/dbm_KeyValueRomStorageTemplate.h>
#include <nn/fnd/fnd_LinkedList.h>

namespace nn{
namespace dbm{

template <typename DirectoryBucketStorage_,typename DirectoryEntryStorage_,typename FileBucketStorage_,typename FileEntryStorage_>
class HierarchicalRomFileTableTemplate
{
private:
    typedef DirectoryBucketStorage_ DirectoryBucketStorage;
    typedef DirectoryEntryStorage_ DirectoryEntryStorage;
    typedef FileBucketStorage_ FileBucketStorage;
    typedef FileEntryStorage_ FileEntryStorage;
public:
    typedef u32 StoragePosition;

    struct FindPosition
    {
        StoragePosition nextPositionDirectory;
        StoragePosition nextPositionFile;
    };

    struct DirectoryEntry
    {
        StoragePosition posNext;
        StoragePosition posDirectory;
        StoragePosition posFile;
    };

    typedef detail::RomFileInfo FileInfo;

    struct FileEntry
    {
        StoragePosition  posNext;
        FileInfo info;
    };
private:
    static const u32 MAX_KEY_LENGTH = RomPathTool::MAX_PATH_LENGTH;

    template <class BucketStorage_,class EntryStorage_,class Key_,class Key2_,class Value_>
    class EntryMapTable  : public KeyValueRomStorageTemplate<BucketStorage_,EntryStorage_,Key_,Value_,MAX_KEY_LENGTH>
    {
    public:
        typedef Key_ Key;
        typedef Key2_ Key2;
        typedef Value_ Value;
        typedef u32 Position;
        typedef KeyValueRomStorageTemplate <BucketStorage_,EntryStorage_,Key_,Value_,MAX_KEY_LENGTH>BaseClass;
    
    public:
        inline Result Get(Position* pOutPosition, Value* pValue, const Key2& key) const
        {
            return BaseClass::GetInternal(pOutPosition,pValue,key.key,key.Hash(),key.name.path,key.name.length * sizeof(RomPathChar));
        }

        inline Result GetByPosition(Key* pKey, Value* pValue, void* pExtraKey, size_t* pExtraSize, Position pos) const
        {
            return BaseClass::GetByPosition(pKey, pValue, pExtraKey, pExtraSize, pos);
        }
    };

    struct RomEntryKey
    {
        StoragePosition parentDir;

        inline bool IsEqual(const RomEntryKey& rKey, const void* pExtraKey1, size_t extraSize1, const void* pExtraKey2, size_t extraSize2) const 
        {
            if (this->parentDir != rKey.parentDir)
            {
                return false;
            }

            if (extraSize1 != extraSize2)
            {
                return false;
            }

            return RomPathTool::IsEqualPath(reinterpret_cast<const RomPathChar *>(pExtraKey1), reinterpret_cast<const RomPathChar *>(pExtraKey2), (extraSize1 / sizeof(RomPathChar)));
        }
    };

    struct EntryKey
    {
        RomEntryKey key;
        RomPathTool::RomEntryName name;

        inline u32 Hash() const
        {
            u32 v = 123456789;
            v ^= key.parentDir;
            const RomPathChar* pName = name.path;
            const RomPathChar* pNameEnd = pName + name.length;
            while (pName < pNameEnd)
            {
                v = ((v >> 5)|(v << (32-5))) ^ (*pName);
                pName ++;
            }
            return v;
        }
    };

    typedef EntryMapTable<DirectoryBucketStorage, DirectoryEntryStorage, RomEntryKey, EntryKey, DirectoryEntry> DirectoryEntryMapTable;
    typedef EntryMapTable<FileBucketStorage, FileEntryStorage, RomEntryKey, EntryKey, FileEntry> FileEntryMapTable;

    DirectoryEntryMapTable mTableDirectory;
    FileEntryMapTable mTableFile;
public:
    static inline RomFileId PositionToFileId(StoragePosition pos)
    {
        return static_cast<RomFileId>(pos);
    }

    static inline StoragePosition FileIdToPosition(RomFileId fileId)
    {
        return static_cast<StoragePosition>(fileId);
    }

    HierarchicalRomFileTableTemplate();
    Result Initialize(DirectoryBucketStorage* pDirectoryBucket,s64 offsetDirectoryBucket,u32 sizeDirectoryBucket, DirectoryEntryStorage* pDirectoryEntry,s64 offsetDirectoryEntry,
               u32 sizeDirectoryEntry,FileBucketStorage* pFileBucket,s64 offsetFileBucket,u32 sizeFileBucket, FileEntryStorage* pFileEntry,s64 offsetFileEntry,u32 sizeFileEntry);
    void Finalize(void);
    Result OpenFile(FileInfo* pFileInfo, const RomPathChar* pFullPath) const;
    Result OpenFile(FileInfo* pFileInfo, RomFileId fileId) const;
    Result OpenFile(FileInfo* pFileInfo, const EntryKey& fileKey) const;

    Result FindOpen(FindPosition* pfi, const RomPathChar* pFullPath) const;
    Result FindOpen(FindPosition* pfi, const EntryKey& dirKey) const;

    Result FindNextDirectory(RomPathChar* pDirectoryName, FindPosition* pfi) const;
    Result FindNextFile(RomPathChar* pFileName, FindPosition* pfi) const;
    Result GetGrandparent(StoragePosition* pPosition,EntryKey* pDirKey,DirectoryEntry* pDirEntry,StoragePosition pos,RomPathTool::RomEntryName name,const RomPathChar* pFullPath) const;

    Result FindParentDirRecursive(StoragePosition* pParentPosition,EntryKey* pParentDirKey,DirectoryEntry* pParentDirEntry,RomPathTool::PathParser& parser,const RomPathChar* pFullPath) const;
    Result FindPathRecursive(EntryKey* pParentDirKey,DirectoryEntry* pParentDirEntry,EntryKey* pKey,bool bFindDir,const RomPathChar* pFullPath) const;
    Result FindFileRecursive(EntryKey* pParentDirKey,DirectoryEntry* pParentDirEntry,EntryKey* pFileKey,const RomPathChar* pFullPath) const;
    Result FindDirectoryRecursive(EntryKey* pParentDirKey,DirectoryEntry* pParentDirEntry,EntryKey* pDirKey,const RomPathChar* pFullPath) const;

    Result GetDirectoryEntry(StoragePosition* pPosition, DirectoryEntry* pEntry, const EntryKey& key) const;

    Result GetFileEntry(StoragePosition* pPosition, FileEntry* pEntry, const EntryKey& key) const;
    Result GetFileEntry(FileEntry* pEntry, RomFileId fileId) const;
};

}
}