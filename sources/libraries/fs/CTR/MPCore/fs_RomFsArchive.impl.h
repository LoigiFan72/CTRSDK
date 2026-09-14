#pragma once

#include <nn/fs/CTR/MPCore/fs_UserArchive.h>
#include <nn/os/os_CriticalSection.h>
#include <nn/err.h>
#include <nn/fs/fs_Api.h>
#include <nn/util/util_TypeTraits.h>
#include <cstdlib>
#include <nn/dbm/dbm_Parameters.h>
#include <nn/dbm/dbm_HierarchicalRomFileTableTemplate.h>
#include <nn/dbm/dbm_HierarchicalRomFileTableTemplate.impl.h>
#include <nn/util/util_Result.h>

namespace nn{
namespace fs{
namespace CTR{
namespace MPCore{
namespace detail{

class RomFsArchive : public IArchive{
public:
    typedef nn::fs::CTR::MPCore::Path Path;
private:
    class RomFsStorage{
    private:
        bit8* m_Buffer;
        size_t m_Size;
        RomFsArchive* m_pParent;
        u32 m_Offset;
    public:
        RomFsStorage() {}

        RomFsStorage(void* buffer, size_t size): 
            m_Buffer(static_cast<bit8*>(buffer)), 
            m_Size(size), 
            m_pParent(0) 
        {
        }

        RomFsStorage(size_t size, RomFsArchive* parent, u32 offset): 
            m_Buffer(0), 
            m_Size(size), 
            m_pParent(parent), 
            m_Offset(offset) 
        {
        }

        Result ReadBytes(s64 offset, void* buffer, size_t size)
        {
            NN_TASSERT_(buffer != NULL);
            NN_TASSERT_(m_Size > 0);
            NN_TASSERT_(offset + size <= m_Size);
            if (m_Buffer)
            {
                std::memcpy(buffer, this->m_Buffer + static_cast<u32>(offset), size);
            }
            else if (m_pParent)
            {
                s32 n;
                NN_UTIL_RETURN_IF_FAILED(m_pParent->GetBaseFile()->TryRead(&n, m_Offset + static_cast<u32>(offset), buffer, size));
            }
            return ResultSuccess();
        }
    };

    typedef nn::dbm::HierarchicalRomFileTableTemplate<RomFsStorage, RomFsStorage, RomFsStorage, RomFsStorage> RomFileTable;
    s32 m_ArchivePriority;
    IFile* m_pBaseFile;
    IFile* m_pPriorFiles[5];
    RomFileTable m_RomFileTable;
    RomFsStorage m_StorageDirectoryBucket;
    RomFsStorage m_StorageDirectoryEntry;
    RomFsStorage m_StorageFileBucket;
    RomFsStorage m_StorageFileEntry;
    u32 m_EntrySize;
    int rev;
    fnd::UnitHeapTemplate<nn::os::LockPolicy::NoLock> m_FileHeap;
    fnd::UnitHeapTemplate<nn::os::LockPolicy::NoLock> m_DirectoryHeap;
public:
    class File : public IFile{
    private:
        RomFsArchive* m_Parent;
        s64 m_Head;
        s64 m_Tail;
        s32 m_Priority;

        s64 GetSize() const { return m_Tail - m_Head; }
    public:
        File(RomFsArchive* parent, s64 head, s64 tail): 
            m_Parent(parent), 
            m_Head(head), 
            m_Tail(tail), 
            m_Priority(parent->m_ArchivePriority)
        {
        }

        virtual Result TryRead (s32* pOut, s64 offset, void* buffer, size_t size)
        {
            NN_TASSERT_(offset >= 0);
            NN_TASSERT_(GetSize() >= offset);

            if (offset + size > this->GetSize())
            {
                size = this->GetSize() - offset;
            }
            return m_Parent->GetBaseFile(this->m_Priority)->TryRead(pOut, offset + this->m_Head, buffer, size);
        }
        virtual Result TryWrite (s32* pOut, s64 offset, const void* buffer, size_t, bool flush){ return ResultUnsupportedOperation();}
        virtual Result TryGetSize (s64* pOut) const{ *pOut = this->GetSize(); return ResultSuccess(); }
        virtual Result TrySetSize (s64 size){ return ResultUnsupportedOperation();  }
        virtual Result TryFlush (){ return ResultUnsupportedOperation(); }
        virtual Result TrySetPriority(s32 priority)
        {
            NN_UTIL_RETURN_IF_FAILED(m_Parent->OpenLinkFileIfNecessary(priority));
            m_Priority = priority;

            return ResultSuccess();
        }
        virtual Result TryGetPriority(s32* pOut) const
        {             
            if (pOut)
            {
                *pOut = m_Priority;
            }
            return ResultSuccess();
        }
        virtual Result DuplicateHandle (Handle* pOut, s64 offset, s64 length){ return m_Parent->DuplicateHandle(pOut, offset + m_Head, length); }
        virtual Result OpenLinkHandle(Handle* pOut){ return m_Parent->OpenLinkHandle(pOut); } 
        virtual void Close ()
        {
            this->~File();
            this->m_Parent->m_FileHeap.Free(this);
        }
        virtual ~File (){ }
    };

    class Directory : public IDirectory
    {
    private:
        RomFsArchive* m_Parent;
        RomFileTable::FindPosition m_FindPosition;
        s32 m_FileId;
        s32 m_Priority;
    public:
        Directory(RomFsArchive* parent, RomFileTable::FindPosition* pfp): 
            m_Parent(parent), 
            m_FindPosition(*pfp), 
            m_Priority(parent->m_ArchivePriority)
        {
        }
        virtual Result TryRead(s32* pOut, DirectoryEntry pEntries[], s32 numEntries)
        {
            s32 i = 0;
            for (; i < numEntries; ++i)
            {
                Result result = this->m_Parent->m_RomFileTable.FindNextDirectory(pEntries[i].entryName, &this->m_FindPosition);
                if (result <= dbm::ResultFindFinished())
                {
                    break;
                }
                pEntries[i].attributes.isDirectory = true;
                pEntries[i].shortName.valid = false;
            }
            for (; i < numEntries; ++i)
            {
                RomFileTable::StoragePosition filePosition = m_FindPosition.nextPositionFile;
                Result result = this->m_Parent->m_RomFileTable.FindNextFile(pEntries[i].entryName, &this->m_FindPosition);
                if (result <= dbm::ResultFindFinished())
                {
                    break;
                }
                
                RomFileTable::FileInfo fi;
                NN_UTIL_RETURN_IF_FAILED(this->mParent->mRomFileTable.OpenFile(&fi, this->m_Parent->m_RomFileTable.PositionToFileId(filePosition)));
                pEntries[i].entrySize = fi.size.Get();
                pEntries[i].attributes.isDirectory = false;
                pEntries[i].shortName.valid = false;
            }
            *pOut = i;
            return ResultSuccess();
        }
        virtual void Close ()
        {
            this->~Directory();
            m_Parent->m_DirectoryHeap.Free(this);
        }
        virtual Result TrySetPriority(s32 priority){ return ResultUnsupportedOperation(); }
        virtual Result TryGetPriority(s32* pOut) const{ return ResultUnsupportedOperation(); }
        virtual ~Directory () {}
    };

    friend class File;
    friend class Directory;
private:
    static s32 GetPriorityIndex(s32 priority)
    {
        if (priority < 0)
        {
            if (1 <= priority)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (0 < priority)
        {
            if (priority <= 2)
            {
                return 2;
            }
            else
            {
                return 3;
            }
        }
        else
        {
            return 4;
        }
    }

    Result OpenLinkFileIfNecessary(s32 priority)
    {
        if (!m_pPriorFiles[GetPriorityIndex(priority)])
        {
            Handle handle;
            NN_UTIL_RETURN_IF_FAILED(OpenLinkHandle(&handle));

            IFile* p = NULL;
            NN_UTIL_RETURN_IF_FAILED(OpenDirect(&p, handle));
            NN_UTIL_RETURN_IF_FAILED_1(p->TrySetPriority(priority),p->Close());

            m_pPriorFiles[GetPriorityIndex(priority)] = p;
        }

        return ResultSuccess();
    }
public:
    IFile* GetBaseFile(s32 priority)
    {
        IFile* pBaseFile = m_pPriorFiles[GetPriorityIndex(priority)];
        NN_NULL_TASSERT_(pBaseFile);

        return pBaseFile;
    }
    IFile* GetBaseFile()
    {
        return GetBaseFile(m_ArchivePriority);
    }

    static s32 GetMetaDataRequiredSize(IFile* file)
    {
        s32 n;
        dbm::detail::RomFileSystemInformation header;
        if (file->TryRead(&n, 0, &header, sizeof(dbm::detail::RomFileSystemInformation)).IsFailure())
        {
            return -1;
        }

        if (n != sizeof(dbm::detail::RomFileSystemInformation))
        {
            return -1;
        }

        s32 ret = 0;
        ret += header.sizeBucketDirectory + header.sizeDirectoryEntry + header.sizeBucketFile + header.sizeFileEntry;
        return ret;
    }

    static size_t GetFileDirectoryRequiredSize(size_t maxFile, size_t maxDirectory)
    {
        return sizeof(File) * maxFile + sizeof(Directory) * maxDirectory;
    }

    static s32 GetRequiredWorkingMemorySize(IFile* file, size_t maxFile, size_t maxDirectory, bool useCache)
    {
        s32 ret = 0;

        if (useCache)
        {
            s32 meta = GetMetaDataRequiredSize(file);
            if (meta < 0)
            {
                return -1;
            }
            ret += meta;
        }

        ret += GetFileDirectoryRequiredSize(maxFile, maxDirectory);
        return ret;
    }


protected:
    RomFsArchive(): 
        m_ArchivePriority(2),   
        m_pBaseFile(NULL)
    {
        for (s32 i = 0; i < 5; ++i)
        {
            m_pPriorFiles[i] = NULL;
        }
    }

public:
    Result Initialize(IFile* baseFile, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache)
    {
        bit8* buf = static_cast<bit8*>(workingMemory);
        s32 n;
        s32 readCount = 0;
        nn::dbm::detail::RomFileSystemInformation header;

        NN_UTIL_RETURN_IF_FAILED(baseFile->TryRead(&n, 0, &header, sizeof(nn::dbm::detail::RomFileSystemInformation)));
        readCount += n;
        if (n != sizeof(nn::dbm::detail::RomFileSystemInformation))
        {
            return fs::ResultNotFormatted();
        }

        if (GetRequiredWorkingMemorySize(baseFile, maxFile, maxDirectory, useCache) > workingMemorySize)
        {
            return fs::ResultOutOfMemory();
        }

        if (useCache)
        {
            bit8* BufDirectoryBucket;
            bit8* BufDirectoryEntry;
            bit8* BufFileBucket;
            bit8* BufFileEntry;

            NN_UTIL_RETURN_IF_FAILED(baseFile->TryRead(&n, readCount, buf, header.sizeBucketDirectory));
            readCount += n;
            BufDirectoryBucket = buf;
            buf += header.sizeBucketDirectory;
            NN_TASSERT_(n == header.sizeBucketDirectory);

            NN_UTIL_RETURN_IF_FAILED(baseFile->TryRead(&n, readCount, buf, header.sizeDirectoryEntry));
            readCount += n;
            BufDirectoryEntry = buf;
            buf += header.sizeDirectoryEntry;
            NN_TASSERT_(n == header.sizeDirectoryEntry);

            NN_UTIL_RETURN_IF_FAILED(baseFile->TryRead(&n, readCount, buf, header.sizeBucketFile));
            readCount += n;
            BufFileBucket = buf;
            buf += header.sizeBucketFile;
            NN_TASSERT_(n == header.sizeBucketFile);

            NN_UTIL_RETURN_IF_FAILED(baseFile->TryRead(&n, readCount, buf, header.sizeFileEntry));
            readCount += n;
            BufFileEntry = buf;
            buf += header.sizeFileEntry;
            NN_TASSERT_(n == header.sizeFileEntry);

            m_StorageDirectoryBucket = RomFsStorage(BufDirectoryBucket, header.sizeBucketDirectory);
            m_StorageDirectoryEntry = RomFsStorage(BufDirectoryEntry, header.sizeDirectoryEntry);
            m_StorageFileBucket = RomFsStorage(BufFileBucket, header.sizeBucketFile);
            m_StorageFileEntry = RomFsStorage(BufFileEntry, header.sizeFileEntry);
        }
        else
        {
            m_StorageDirectoryBucket = RomFsStorage(header.sizeBucketDirectory, this, header.offsetBucketDirectory);
            m_StorageDirectoryEntry = RomFsStorage(header.sizeDirectoryEntry, this, header.offsetDirectoryEntry);
            m_StorageFileBucket = RomFsStorage(header.sizeBucketFile, this, header.offsetBucketFile);
            m_StorageFileEntry = RomFsStorage(header.sizeFileEntry, this, header.offsetFileEntry);
        }

        NN_ERR_THROW_FATAL_ALL(this->m_RomFileTable.Initialize(&this->m_StorageDirectoryBucket,0,header.sizeBucketDirectory, &this->m_StorageDirectoryEntry,0,header.sizeDirectoryEntry,
                     &this->m_StorageFileBucket,0,header.sizeBucketFile,&this->m_StorageFileEntry,0,header.sizeFileEntry));

        s32 sizeFileBuffer      = sizeof(File) * maxFile;
        s32 sizeDirectoryBuffer = sizeof(Directory) * maxDirectory;

        this->m_FileHeap.Initialize(sizeof(File), reinterpret_cast<uptr>(buf), sizeFileBuffer);
        this->m_DirectoryHeap.Initialize(sizeof(Directory), reinterpret_cast<uptr>(buf + sizeFileBuffer), sizeDirectoryBuffer);

        m_EntrySize = header.offsetFileBody;

        NN_ERR_THROW_FATAL_ALL(GetFileServer().GetPriority(&this->m_ArchivePriority));
        m_pBaseFile = this->m_pPriorFiles[GetPriorityIndex(this->m_ArchivePriority)] = baseFile;

        return ResultSuccess();
    }

    virtual Result OpenFile (IFile** pOut, const Path& path, bit32)
    {
        RomFileTable::FileInfo fi;
        NN_UTIL_RETURN_IF_FAILED(this->m_RomFileTable.OpenFile(&fi, path.GetWStringRaw()));
        *pOut = new (this->m_FileHeap.Allocate()) File(this, this->m_EntrySize + fi.offset.Get(), this->m_EntrySize + fi.offset.Get() + fi.size.Get() );
        if(!(*pOut))
        {
            return ResultOutOfMemory();
        }

        NN_UTIL_RETURN_IF_FAILED_2((*pOut)->TrySetPriority(this->m_ArchivePriority),(*pOut)->Close(),*pOut = NULL);

        return ResultSuccess();
    }
    virtual Result OpenDirectory (IDirectory** pOut, const Path& path)
    {
        RomFileTable::FindPosition fp;
        NN_UTIL_RETURN_IF_FAILED(this->m_RomFileTable.FindOpen(&fp, path.GetWStringRaw()));
        *pOut = new (this->m_DirectoryHeap.Allocate())Directory(this, &fp);
        if(!(*pOut))
        {
            return ResultOutOfMemory();
        }

        return ResultSuccess();
    }
    virtual Result DeleteFile (const Path&){ ResultUnsupportedOperation(); }
    virtual Result RenameFile (const Path&, const Path&){ ResultUnsupportedOperation(); }
    virtual Result DeleteDirectory (const Path&){ ResultUnsupportedOperation(); }
    virtual Result DeleteDirectoryRecursively (const Path&){ ResultUnsupportedOperation(); }
    virtual Result CreateFile (const Path&, s64){ ResultUnsupportedOperation();}
    virtual Result CreateDirectory (const Path&){ ResultUnsupportedOperation(); }
    virtual Result RenameDirectory (const Path&, const Path&){ ResultUnsupportedOperation(); }
    virtual Result SetArchivePriority(s32 priority)
    {
        NN_UTIL_RETURN_IF_FAILED(OpenLinkFileIfNecessary(priority));

        m_ArchivePriority = priority;

        return ResultSuccess();
    }

    virtual Result GetArchivePriority(s32* pOut)
    {
        *pOut = m_ArchivePriority;
        return ResultSuccess();
    }

    virtual ~RomFsArchive()
    {
        m_pBaseFile = NULL;
        for (s32 i = 0; i < 5; ++i)
        {
            if (m_pPriorFiles[i]){
                m_pPriorFiles[i]->Close();
            }
        }
    }
    virtual Result OpenDirect(IFile** pOut, Handle handle) = 0;
    virtual Result DuplicateHandle(nn::Handle* pOut, s64 offset, s64 length){ return this->GetBaseFile()->DuplicateHandle(pOut, offset, length); }
    virtual Result OpenLinkHandle(nn::Handle* pOut){ return this->m_pBaseFile->OpenLinkHandle(pOut); }
};

}
}
}
}
}