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
        bit8* mBuffer;
        size_t mSize;
        RomFsArchive* mpParent;
        u32 mOffset;
    public:
        RomFsStorage() {}

        RomFsStorage(void* buffer, size_t size): 
            mBuffer(static_cast<bit8*>(buffer)), 
            mSize(size), 
            mpParent(0) 
        {}

        RomFsStorage(size_t size, RomFsArchive* parent, u32 offset): 
            mBuffer(0), 
            mSize(size), 
            mpParent(parent), 
            mOffset(offset) 
        {}

        Result ReadBytes(s64 offset, void* buffer, size_t size){
            NN_TASSERT_(buffer != NULL);
            NN_TASSERT_(this->mSize > 0);
            NN_TASSERT_(offset + size <= this->mSize);
            if (mBuffer){
                std::memcpy(buffer, this->mBuffer + static_cast<u32>(offset), size);
            }
            else if (mpParent){
                s32 n;
                NN_UTIL_RETURN_IF_FAILED(this->mpParent->GetBaseFile()->TryRead(&n, this->mOffset + static_cast<u32>(offset), buffer, size));
            }
            return ResultSuccess();
        }
    };

    typedef nn::dbm::HierarchicalRomFileTableTemplate<RomFsStorage, RomFsStorage, RomFsStorage, RomFsStorage> RomFileTable;
    s32 mArchivePriority;
    IFile* mpBaseFile;
    IFile* mpPriorFiles[5];
    RomFileTable mRomFileTable;
    RomFsStorage mStorageDirectoryBucket;
    RomFsStorage mStorageDirectoryEntry;
    RomFsStorage mStorageFileBucket;
    RomFsStorage mStorageFileEntry;
    u32 mEntrySize;
    int rev;
    fnd::UnitHeapTemplate<nn::os::LockPolicy::NoLock> mFileHeap;
    fnd::UnitHeapTemplate<nn::os::LockPolicy::NoLock> mDirectoryHeap;
public:
    class File : public IFile{
    private:
        RomFsArchive* mParent;
        s64 mHead;
        s64 mTail;
        s32 mPriority;

        s64 GetSize() const { return mTail - mHead; }
    public:
        File(RomFsArchive* parent, s64 head, s64 tail): 
            mParent(parent), 
            mHead(head), 
            mTail(tail), 
            mPriority(parent->mArchivePriority)
        {}

        virtual Result TryRead (s32* pOut, s64 offset, void* buffer, size_t size){
            NN_TASSERT_(offset >= 0);
            NN_TASSERT_(GetSize() >= offset);

            if (offset + size > this->GetSize()){
                size = this->GetSize() - offset;
            }
            return mParent->GetBaseFile(this->mPriority)->TryRead(pOut, offset + this->mHead, buffer, size);
        }
        virtual Result TryWrite (s32* pOut, s64 offset, const void* buffer, size_t, bool flush){ return ResultUnsupportedOperation();}
        virtual Result TryGetSize (s64* pOut) const{ *pOut = this->GetSize(); return ResultSuccess(); }
        virtual Result TrySetSize (s64 size){ return ResultUnsupportedOperation();  }
        virtual Result TryFlush (){ return ResultUnsupportedOperation(); }
        virtual Result TrySetPriority(s32 priority){
            NN_UTIL_RETURN_IF_FAILED(this->mParent->OpenLinkFileIfNecessary(priority));
            mPriority = priority;

            return ResultSuccess();
        }
        virtual Result TryGetPriority(s32* pOut) const{             
            if (pOut){
                *pOut = mPriority;
            }
            return ResultSuccess();
        }
        virtual Result DuplicateHandle (Handle* pOut, s64 offset, s64 length){ return this->mParent->DuplicateHandle(pOut, offset + this->mHead, length); }
        virtual Result OpenLinkHandle(Handle* pOut){ return this->mParent->OpenLinkHandle(pOut); }
        virtual void Close (){
            this->~File();
            this->mParent->mFileHeap.Free(this);
        }
        virtual ~File () {}
    };

    class Directory : public IDirectory{
    private:
        RomFsArchive* mParent;
        RomFileTable::FindPosition mFindPosition;
        s32 mFileId;
        s32 mPriority;
    public:
        Directory(RomFsArchive* parent, RomFileTable::FindPosition* pfp): 
            mParent(parent), 
            mFindPosition(*pfp), 
            mPriority(parent->mArchivePriority)
        {}
        virtual Result TryRead(s32* pOut, DirectoryEntry pEntries[], s32 numEntries){
            s32 i = 0;
            for (; i < numEntries; ++i){
                Result result = this->mParent->mRomFileTable.FindNextDirectory(pEntries[i].entryName, &this->mFindPosition);
                if (result <= dbm::ResultFindFinished()){
                    break;
                }
                pEntries[i].attributes.mIsDirectory = true;
                pEntries[i].shortName.valid = false;
            }
            for (; i < numEntries; ++i){
                RomFileTable::StoragePosition filePosition = mFindPosition.nextPositionFile;
                Result result = this->mParent->mRomFileTable.FindNextFile(pEntries[i].entryName, &this->mFindPosition);
                if (result <= dbm::ResultFindFinished()){
                    break;
                }
                
                RomFileTable::FileInfo fi;
                NN_UTIL_RETURN_IF_FAILED(this->mParent->mRomFileTable.OpenFile(&fi, this->mParent->mRomFileTable.PositionToFileId(filePosition)));
                pEntries[i].entrySize = fi.size.Get();
                pEntries[i].attributes.mIsDirectory = false;
                pEntries[i].shortName.valid = false;
            }
            *pOut = i;
            return ResultSuccess();
        }
        virtual void Close (){
            this->~Directory();
            mParent->mDirectoryHeap.Free(this);
        }
        virtual Result TrySetPriority(s32 priority){ return ResultUnsupportedOperation(); }
        virtual Result TryGetPriority(s32* pOut) const{ return ResultUnsupportedOperation(); }
        virtual ~Directory () {}
    };

    friend class File;
    friend class Directory;
private:
    static s32 GetPriorityIndex(s32 priority){
        if (priority < 0){
            if (1 <= priority){
                return 1;
            }
            else{
                return 0;
            }
        }
        else if (0 < priority){
            if (priority <= 2){
                return 2;
            }
            else{
                return 3;
            }
        }
        else{
            return 4;
        }
    }

    Result OpenLinkFileIfNecessary(s32 priority){
        if (!mpPriorFiles[GetPriorityIndex(priority)]){
            Handle handle;
            NN_UTIL_RETURN_IF_FAILED(OpenLinkHandle(&handle));

            IFile* p = NULL;
            NN_UTIL_RETURN_IF_FAILED(OpenDirect(&p, handle));
            NN_UTIL_RETURN_IF_FAILED_1(p->TrySetPriority(priority),p->Close());

            mpPriorFiles[GetPriorityIndex(priority)] = p;
        }

        return ResultSuccess();
    }
public:
    IFile* GetBaseFile(s32 priority){
        IFile* pBaseFile = mpPriorFiles[GetPriorityIndex(priority)];
        NN_NULL_TASSERT_(pBaseFile);

        return pBaseFile;
    }
    IFile* GetBaseFile(){
        return GetBaseFile(mArchivePriority);
    }

    static s32 GetMetaDataRequiredSize(IFile* file){
        s32 n;
        dbm::detail::RomFileSystemInformation header;
        if (file->TryRead(&n, 0, &header, sizeof(dbm::detail::RomFileSystemInformation)).IsFailure()){
            return -1;
        }

        if (n != sizeof(dbm::detail::RomFileSystemInformation)){
            return -1;
        }

        s32 ret = 0;
        ret += header.sizeBucketDirectory + header.sizeDirectoryEntry + header.sizeBucketFile + header.sizeFileEntry;
        return ret;
    }

    static size_t GetFileDirectoryRequiredSize(size_t maxFile, size_t maxDirectory){
        return sizeof(File) * maxFile + sizeof(Directory) * maxDirectory;
    }

    static s32 GetRequiredWorkingMemorySize(IFile* file, size_t maxFile, size_t maxDirectory, bool useCache){
        s32 ret = 0;

        if (useCache){
            s32 meta = GetMetaDataRequiredSize(file);
            if (meta < 0){
                return -1;
            }
            ret += meta;
        }

        ret += GetFileDirectoryRequiredSize(maxFile, maxDirectory);
        return ret;
    }


protected:
    RomFsArchive(): 
        mArchivePriority(2),   
        mpBaseFile(NULL){
        for (s32 i = 0; i < 5; ++i){
            mpPriorFiles[i] = NULL;
        }
    }

public:
    Result Initialize(IFile* baseFile, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
        bit8* buf = static_cast<bit8*>(workingMemory);
        s32 n;
        s32 readCount = 0;
        nn::dbm::detail::RomFileSystemInformation header;

        NN_UTIL_RETURN_IF_FAILED(baseFile->TryRead(&n, 0, &header, sizeof(nn::dbm::detail::RomFileSystemInformation)));
        readCount += n;
        if (n != sizeof(nn::dbm::detail::RomFileSystemInformation)){
            return fs::ResultNotFormatted();
        }

        if (GetRequiredWorkingMemorySize(baseFile, maxFile, maxDirectory, useCache) > workingMemorySize){
            return fs::ResultOutOfMemory();
        }

        if (useCache){
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

            mStorageDirectoryBucket = RomFsStorage(BufDirectoryBucket, header.sizeBucketDirectory);
            mStorageDirectoryEntry = RomFsStorage(BufDirectoryEntry, header.sizeDirectoryEntry);
            mStorageFileBucket = RomFsStorage(BufFileBucket, header.sizeBucketFile);
            mStorageFileEntry = RomFsStorage(BufFileEntry, header.sizeFileEntry);
        }
        else{
            mStorageDirectoryBucket = RomFsStorage(header.sizeBucketDirectory, this, header.offsetBucketDirectory);
            mStorageDirectoryEntry = RomFsStorage(header.sizeDirectoryEntry, this, header.offsetDirectoryEntry);
            mStorageFileBucket = RomFsStorage(header.sizeBucketFile, this, header.offsetBucketFile);
            mStorageFileEntry = RomFsStorage(header.sizeFileEntry, this, header.offsetFileEntry);
        }

        NN_ERR_THROW_FATAL_ALL(this->mRomFileTable.Initialize(&this->mStorageDirectoryBucket,0,header.sizeBucketDirectory, &this->mStorageDirectoryEntry,0,header.sizeDirectoryEntry,
                     &this->mStorageFileBucket,0,header.sizeBucketFile,&this->mStorageFileEntry,0,header.sizeFileEntry));

        s32 sizeFileBuffer      = sizeof(File) * maxFile;
        s32 sizeDirectoryBuffer = sizeof(Directory) * maxDirectory;

        this->mFileHeap.Initialize(sizeof(File), reinterpret_cast<uptr>(buf), sizeFileBuffer);
        this->mDirectoryHeap.Initialize(sizeof(Directory), reinterpret_cast<uptr>(buf + sizeFileBuffer), sizeDirectoryBuffer);

        mEntrySize = header.offsetFileBody;

        NN_ERR_THROW_FATAL_ALL(GetFileServer().GetPriority(&this->mArchivePriority));
        mpBaseFile = this->mpPriorFiles[GetPriorityIndex(this->mArchivePriority)] = baseFile;

        return ResultSuccess();
    }

    virtual Result OpenFile (IFile** pOut, const Path& path, bit32){
        RomFileTable::FileInfo fi;
        NN_UTIL_RETURN_IF_FAILED(this->mRomFileTable.OpenFile(&fi, path.GetWStringRaw()));
        *pOut = new (this->mFileHeap.Allocate()) File(this, this->mEntrySize + fi.offset.Get(), this->mEntrySize + fi.offset.Get() + fi.size.Get() );
        if(!(*pOut)){
            return ResultOutOfMemory();
        }

        NN_UTIL_RETURN_IF_FAILED_2((*pOut)->TrySetPriority(this->mArchivePriority),(*pOut)->Close(),*pOut = NULL);

        return ResultSuccess();
    }
    virtual Result OpenDirectory (IDirectory** pOut, const Path& path){
        RomFileTable::FindPosition fp;
        NN_UTIL_RETURN_IF_FAILED(this->mRomFileTable.FindOpen(&fp, path.GetWStringRaw()));
        *pOut = new (this->mDirectoryHeap.Allocate())Directory(this, &fp);
        if(!(*pOut)){
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
    virtual Result SetArchivePriority(s32 priority){
        NN_UTIL_RETURN_IF_FAILED(OpenLinkFileIfNecessary(priority));

        mArchivePriority = priority;

        return ResultSuccess();
    }
    virtual Result GetArchivePriority(s32* pOut){
        *pOut = mArchivePriority;
        return ResultSuccess();
    }
    virtual ~RomFsArchive(){
        mpBaseFile = NULL;
        for (s32 i = 0; i < 5; ++i){
            if (mpPriorFiles[i]){
                mpPriorFiles[i]->Close();
            }
        }
    }
    virtual Result OpenDirect(IFile** pOut, Handle handle) = 0;
    virtual Result DuplicateHandle(nn::Handle* pOut, s64 offset, s64 length){ return this->GetBaseFile()->DuplicateHandle(pOut, offset, length); }
    virtual Result OpenLinkHandle(nn::Handle* pOut){ return this->mpBaseFile->OpenLinkHandle(pOut); }
};

}
}
}
}
}