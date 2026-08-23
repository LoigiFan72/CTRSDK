// Filename: fs_UserFileSystem.cpp
//
// Project: Horizon

#include <nn/fs/CTR/MPCore/fs_UserFileSystem.h>
#include <nn/fs/fs_IpcFileSystem.h>
#include <nn/fs/fs_IpcFile.h>
#include <nn/fs/fs_IpcDirectory.h>
#include <nn/fs/CTR/MPCore/fs_FileSystemBase.h>
#include <nn/fs/fs_Types.h>
#include <nn/fslow/fslow_QueryOptimalBucketCount.h>

#include <nn/CTR.h>
#include <nn/err.h>
#include <nn/fnd.h>
#include <nn/os.h>
#include <nn/util/util_Result.h>
#include <nn/cfg/CTR/cfg_DebugParam.h>

#include "fs_AccessAnalysis.h"
#include "fs_ArchiveHandle.impl.h"
#include "fs_RomFsArchive.impl.h"

namespace nn{
namespace fs{
    
nn::Handle gFileServerHandle = INVALID_HANDLE_VALUE;
nn::os::CriticalSection gMountLock = nn::WithInitialize();

namespace CTR{
namespace MPCore{
namespace detail{

Handle& GetFileServerHandle(){ return fs::gFileServerHandle; }

namespace{
    fnd::UnitHeapBase sArchiveHeap = *(fnd::UnitHeapBase*)0;

    const wchar_t* GetArchivePath(const wchar_t* path){
        while (true){
            if (*path++ == L':'){
                return path;
            }
        }
    }
}

/* =============== */
/* =File Archives= */
/* =============== */

class FileServerArchive : public IArchive, private nn::os::HandleObject{
private:
    typedef nn::fs::ipc::FileSystem IpcObject;

    IpcObject GetIpcObject() const{
        if (!IsInitialized()){
            NN_ERR_THROW_FATAL_ALL(fs::ResultNotInitialized());
        }
        return IpcObject(GetHandle());
    }

    bool IsInitialized() const { return IsValid(); }

    class File : public IFile, public os::HandleObject{
    private:
        typedef nn::fs::ipc::File IpcObject;

        IpcObject GetIpcObject() const{
            if (!IsInitialized()){
                NN_ERR_THROW_FATAL_ALL(ResultNotInitialized());
            }
            return IpcObject(GetHandle());
        }

        bool IsInitialized() const { return IsValid(); }
    public:

        File(Handle handle) { SetHandle(handle); }

        virtual Result TryRead(s32* pOut, s64 offset, void* buffer, size_t size){ return GetIpcObject().Read(pOut, offset, buffer, size); }
        virtual Result TryWrite(s32* pOut, s64 offset, const void* buffer, size_t size, bool flush){ return GetIpcObject().Write(pOut, offset, buffer, size, nn::fs::WriteOption(flush, flush)); }
        virtual Result TryGetSize(s64* pOut) const{ return GetIpcObject().GetSize(pOut); }
        virtual Result TrySetSize(s64 size){ return GetIpcObject().SetSize(size); }
        virtual Result TryFlush(){ s32 n; return GetIpcObject().Write(&n, 0, &n, 0, nn::fs::WriteOption(true, true)); }
        virtual Result TrySetPriority(s32 priority){return GetIpcObject().SetPriority(priority);}
        virtual Result TryGetPriority(s32* pOut) const{return GetIpcObject().GetPriority(pOut);}
        virtual Result DuplicateHandle(Handle* pOut, s64 offset, s64 length){ return GetIpcObject().OpenSubFile(pOut, offset, length); }
        virtual Result OpenLinkHandle(Handle* pOut){ return GetIpcObject().OpenLinkFile(pOut); }
        virtual Handle GetFileHandle() { return this->GetHandle(); }
        virtual void DetachFileHandle() { this->ClearHandle(); }
        virtual void Close();
        virtual ~File(){}
    };

    class Directory : public IDirectory, private nn::os::HandleObject{
    private:

        typedef nn::fs::ipc::Directory IpcObject;

        IpcObject GetIpcObject() const{
            if (!IsInitialized()){
                NN_ERR_THROW_FATAL_ALL(ResultNotInitialized());
            }
            return IpcObject(GetHandle());
        }

        bool IsInitialized() const { return IsValid(); }

    public:

        Directory(Handle handle) { SetHandle(handle); }

        virtual Result TryRead(s32* pNumEntriesOut, DirectoryEntry pEntries[], s32 numEntries){ return GetIpcObject().Read(pNumEntriesOut, pEntries, numEntries); }
        virtual void Close();
        virtual Result TrySetPriority(s32 priority){return GetIpcObject().SetPriority(priority);}
        virtual Result TryGetPriority(s32* pOut) const{return GetIpcObject().GetPriority(pOut);}
    };

    friend class File;
    friend class Directory;

private:
    static nn::util::aligned_storage<sizeof(FileServerArchive::File) * 32, 8>::type sFileServerFileBuffer;
    static nn::util::aligned_storage<sizeof(FileServerArchive::Directory) * 16, 8>::type sFileServerDirectoryBuffer;
    static nn::fnd::ThreadSafeUnitHeap sArchiveHeap;
    static nn::fnd::ThreadSafeUnitHeap sFileHeap;
    static nn::fnd::ThreadSafeUnitHeap sDirectoryHeap;

    ArchiveHandle mArchiveHandle;
public:
    const ArchiveHandle& GetArchiveHandle() const { return mArchiveHandle; }

    FileServerArchive() {}
    FileServerArchive(Handle handle, ArchiveHandle archiveHandle): 
        mArchiveHandle(archiveHandle){
        this->SetHandle(handle);
    }
    void Initialize(Handle handle, ArchiveHandle archiveHandle){
        this->SetHandle(handle);
        mArchiveHandle = archiveHandle;
    }

    static IFile* OpenDirect(Handle handle);

    static Result Create(IArchive** pOut, Handle handle, ArchiveHandle archiveHandle){
        *pOut = new (sArchiveHeap.Allocate()) FileServerArchive(handle, archiveHandle);
        if (!*pOut)
        {
            return nn::fs::ResultOutOfMemory();
        }
        return ResultSuccess();
    }

    virtual Result OpenFile(IFile** pOut, const Path& path, bit32 mode);
    virtual Result OpenDirectory(IDirectory** pOut, const Path& path);
    virtual Result DeleteFile(const Path& path){
        if (path.GetDataSize() > 512){
            return fs::ResultPathTooLong();
        }

        return GetIpcObject().DeleteFile(fs::Transaction(),this->mArchiveHandle, path.GetPathType(), reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize());
    }
    virtual Result RenameFile(const Path& currentPath, const Path& newPath){
        if (currentPath.GetDataSize() > 512 || newPath.GetDataSize() > 512){
            return ResultPathTooLong();
        }

        return GetIpcObject().RenameFile(fs::Transaction(),this->mArchiveHandle, currentPath.GetPathType(), reinterpret_cast<const bit8*>(currentPath.GetDataBuffer()), currentPath.GetDataSize(),this->mArchiveHandle, newPath.GetPathType(), reinterpret_cast<const bit8*>(newPath.GetDataBuffer()), newPath.GetDataSize());
    }

    virtual Result DeleteDirectory(const Path& path){
        if (path.GetDataSize() > 512){
            return ResultPathTooLong();
        }

        return GetIpcObject().DeleteDirectory(fs::Transaction(),this->mArchiveHandle, path.GetPathType(), reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize());
    }

    virtual Result DeleteDirectoryRecursively(const Path& path){
        if (path.GetDataSize() > 512){
            return ResultPathTooLong();
        }

        return GetIpcObject().DeleteDirectoryRecursively(fs::Transaction(),this->mArchiveHandle, path.GetPathType(), reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize());
    }

    virtual Result CreateFile(const Path& path, s64 size){
        if (path.GetDataSize() > 512){
            return ResultPathTooLong();
        }

        return GetIpcObject().CreateFile(fs::Transaction(),this->mArchiveHandle, path.GetPathType(), reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize(),nn::fs::Attributes(), size);
    }

    virtual Result CreateDirectory(const Path& path){
        if (path.GetDataSize() > 512){
            return ResultPathTooLong();
        }

        return GetIpcObject().CreateDirectory(fs::Transaction(),this->mArchiveHandle, path.GetPathType(), reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize(),fs::Attributes());
    }

    virtual Result RenameDirectory(const Path& currentPath, const Path& newPath){
        if (currentPath.GetDataSize() > 512 || newPath.GetDataSize() > 512){
            return ResultPathTooLong();
        }

        return GetIpcObject().RenameDirectory(fs::Transaction(),this->mArchiveHandle, currentPath.GetPathType(), reinterpret_cast<const bit8*>(currentPath.GetDataBuffer()), currentPath.GetDataSize(),this->mArchiveHandle, newPath.GetPathType(), reinterpret_cast<const bit8*>(newPath.GetDataBuffer()), newPath.GetDataSize());
    }

    virtual Result SetArchivePriority(s32 priority){
        return GetIpcObject().SetArchivePriority(this->mArchiveHandle, priority);
    }

    virtual Result GetArchivePriority(s32* pOut){
        return GetIpcObject().GetArchivePriority(pOut, this->mArchiveHandle);
    }

    virtual Result GetFreeBytes(s64* pOut){
        return GetIpcObject().GetFreeBytes(pOut, this->mArchiveHandle);
    }

    void Finalize(){
        if (mArchiveHandle){
            GetIpcObject().CloseArchive(this->mArchiveHandle);
            mArchiveHandle = 0;
        }
        this->ClearHandle();
    }

    virtual ~FileServerArchive(){
        this->Finalize();
    }

    virtual void DeleteObject(){
        this->~FileServerArchive();
        sArchiveHeap.Free(this);
    }
};

nn::util::aligned_storage<sizeof(FileServerArchive) * 16, 8>::type sFileServerArchiveBuffer;
nn::util::aligned_storage<sizeof(FileServerArchive::File) * 32, 8>::type FileServerArchive::sFileServerFileBuffer;
nn::util::aligned_storage<sizeof(FileServerArchive::Directory) * 16, 8>::type FileServerArchive::sFileServerDirectoryBuffer;
nn::fnd::ThreadSafeUnitHeap FileServerArchive::sArchiveHeap(sizeof(FileServerArchive), reinterpret_cast<uptr>(&sFileServerArchiveBuffer), sizeof(sFileServerArchiveBuffer));
nn::fnd::ThreadSafeUnitHeap FileServerArchive::sFileHeap(sizeof(FileServerArchive::File), reinterpret_cast<uptr>(&sFileServerFileBuffer), sizeof(sFileServerFileBuffer));
nn::fnd::ThreadSafeUnitHeap FileServerArchive::sDirectoryHeap(sizeof(FileServerArchive::Directory), reinterpret_cast<uptr>(&sFileServerDirectoryBuffer), sizeof(sFileServerDirectoryBuffer));

IFile* FileServerArchive::OpenDirect(Handle handle){
    return new (sFileHeap.Allocate()) File(handle);
}

void FileServerArchive::File::Close(){
    if (this->IsInitialized()){
        GetIpcObject().Close();
    }
    this->~File();
    sFileHeap.Free(this);
}

void FileServerArchive::Directory::Close(){
    if (IsInitialized()){
        GetIpcObject().Close();
    }
    this->~Directory();
    sDirectoryHeap.Free(this);
}

Result FileServerArchive::OpenFile(IFile** pOut, const Path& path, bit32 mode){
    if (path.GetDataSize() > 512){
        return ResultPathTooLong();
    }

    Handle handle;
    NN_UTIL_RETURN_IF_FAILED(GetIpcObject().OpenFile(&handle, nn::fs::Transaction(),this->mArchiveHandle, path.GetPathType(), reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize(),mode, fs::Attributes()));
    *pOut = new (sFileHeap.Allocate()) File(handle);
    if (!(*pOut)){
        nn::svc::CloseHandle(handle);
        return fs::ResultOutOfMemory();
    }
    return ResultSuccess();
}

Result FileServerArchive::OpenDirectory(IDirectory** pOut, const Path& path){
    if (path.GetDataSize() > 512){
        return fs::ResultPathTooLong();
    }

    Handle handle;
    NN_UTIL_RETURN_IF_FAILED(GetIpcObject().OpenDirectory(&handle,this->mArchiveHandle, path.GetPathType(), reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize()));
    *pOut = new (sDirectoryHeap.Allocate()) Directory(handle);
    if (!(*pOut)){
        nn::svc::CloseHandle(handle);
        return nn::fs::ResultOutOfMemory();
    }
    return ResultSuccess();
}

class ContentRomFsArchive : public RomFsArchive{
private:
    static void* AllocateBuffer();
    static nn::fnd::ThreadSafeUnitHeap* sArchiveHeap;

private:
    virtual Result OpenDirect(IFile** pOut, Handle handle){
        IFile* p = FileServerArchive::OpenDirect(handle);
        if (!p){
            nn::svc::CloseHandle(handle);
            return nn::fs::ResultOutOfMemory();
        }

        *pOut = p;

        return ResultSuccess();
    }
public:
    Result Initialize(size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache, const nn::fs::CTR::ProgramDataPath& contentPath){
        Handle handle;{
            Path path;
            Path filePath = Path::Make(&contentPath);
            NN_UTIL_RETURN_IF_FAILED(GetFileServer().OpenFileDirectly(&handle,fs::Transaction(),3,
                    path.GetPathType(),reinterpret_cast<const bit8*>(path.GetDataBuffer()), path.GetDataSize(),filePath.GetPathType(),
                    reinterpret_cast<const bit8*>(filePath.GetDataBuffer()), filePath.GetDataSize(),OPEN_MODE_READ,fs::Attributes()));
        }

        IFile* p = NULL;
        NN_UTIL_RETURN_IF_FAILED(OpenDirect(&p, handle));

        NN_UTIL_RETURN_IF_FAILED_1(this->RomFsArchive::Initialize(p, maxFile, maxDirectory, workingMemory, workingMemorySize, useCache),p->Close());

        return ResultSuccess();
    }

    static Result Create(ContentRomFsArchive** pOut, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache, const nn::fs::CTR::ProgramDataPath& contentPath){
        ContentRomFsArchive* p = new (AllocateBuffer()) ContentRomFsArchive();
        if (!p){
            return nn::fs::ResultOutOfMemory();
        }
        Result result = p->Initialize(maxFile, maxDirectory, workingMemory, workingMemorySize, useCache, contentPath);
        if (result.IsFailure()){
            p->DeleteObject();
            return result;
        }
        *pOut = p;
        return ResultSuccess();
    }

    Result Initialize(const nn::fs::DataContentArchivePath& contentArchivePath, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
        Handle handle;{
            TitleDataSpecifier titleSpec = TitleDataSpecifier::Make(contentArchivePath.mediaType, contentArchivePath.titleId);
            Path path = Path::Make(&titleSpec);
            TitleDataPath titleData = TitleDataPath::MakeContentDataPath(contentArchivePath.contentIdx, ContentPath::MakeRomfsPath());
            Path filePath = Path::Make(&titleData);
            NN_UTIL_RETURN_IF_FAILED(GetFileServer().OpenFileDirectly(&handle,nn::fs::Transaction(),0x2345678a,path.GetPathType(),path.GetDataBuffer(), path.GetDataSize(),
                    filePath.GetPathType(),filePath.GetDataBuffer(), filePath.GetDataSize(),nn::fs::OPEN_MODE_READ,nn::fs::Attributes())
            );
        }
    }

    static Result Create(ContentRomFsArchive** pOut, const nn::fs::DataContentArchivePath& contentArchivePath, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
        ContentRomFsArchive* p = new (AllocateBuffer()) ContentRomFsArchive();
        if (!p){
            return nn::fs::ResultOutOfMemory();
        }
        Result result = p->Initialize(contentArchivePath, maxFile, maxDirectory, workingMemory, workingMemorySize, useCache);
        if (result.IsFailure()){
            p->DeleteObject();
            return result;
        }
        *pOut = p;
        return ResultSuccess();
    }

    virtual void DeleteObject(){
        this->~ContentRomFsArchive();
        sArchiveHeap->Free(this);
    }

    virtual ~ContentRomFsArchive(){}
};

nn::fnd::ThreadSafeUnitHeap* ContentRomFsArchive::sArchiveHeap = 0;

void* ContentRomFsArchive::AllocateBuffer(){
    nn::os::CriticalSection::ScopedLock lk(gMountLock);

    static nn::util::aligned_storage<sizeof(ContentRomFsArchive) * 16, 8>::type sContentRomFsArchiveBuffer;
    static nn::fnd::ThreadSafeUnitHeap heap(sizeof(ContentRomFsArchive), reinterpret_cast<uptr>(&sContentRomFsArchiveBuffer), sizeof(sContentRomFsArchiveBuffer));
    sArchiveHeap = &heap;
    return heap.Allocate();
}

static bool sIsEmulateEndurance = false;
static bool sIsLatencyEmulationEnable = false;
static s64 sConstantWait = 0;

typedef void (*File)(void*);

inline void InitializeGlobal(Handle h){
    GetFileServer().mSession = h;
    GetFileServer().InitializeWithSdkVersion(0x40205c8);
}

Result OpenSpecialArchiveRaw(IArchive** pOut, bit32 archiveKind){
    Path path;
    bit64 lowHandle;
    NN_UTIL_RETURN_IF_FAILED_0(GetFileServer().OpenArchive(&lowHandle, archiveKind, path.GetPathType(), path.GetDataBuffer(), path.GetDataSize()));
    NN_UTIL_RETURN_IF_FAILED_1(FileServerArchive::Create(pOut, GetFileServerHandle(), lowHandle), GetFileServer().CloseArchive(lowHandle));
    return ResultSuccess();
}

Result OpenRom(IArchive** pOut, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
    ContentRomFsArchive* p = 0;
    NN_UTIL_RETURN_IF_FAILED(ContentRomFsArchive::Create(&p, maxFile, maxDirectory, workingMemory, workingMemorySize, useCache, nn::fs::CTR::ProgramDataPath::MakeRomFsDefaultPath()));
    *pOut = p;
    return ResultSuccess();
}

Result UserFileSystem::Initialize(nn::Handle handle){
    InitializeGlobal(handle);
    return ResultSuccess();
}

Result UserFileSystem::TryCreateFile(const wchar_t *pathName, s64 size){
    IArchive* archive = FindArchive(pathName);
    if (!archive){
        return ResultArchiveNotFound();
    }
    NN_FS_ANALYSIS_LOG_RETURN(archive->CreateFile(Path(GetArchivePath(pathName)), size), "API=CreateFile,Path=%ls,Size=%lld", pathName, size);
}

Result UserFileSystem::TryCreateDirectory(const wchar_t *pathName){
    IArchive* archive = FindArchive(pathName);
    if (!archive){
        return ResultArchiveNotFound();
    }
    NN_FS_ANALYSIS_LOG_RETURN(archive->CreateDirectory(Path(GetArchivePath(pathName))),"API=CreateDirectory,Path=%ls", pathName);
}

Result UserFileSystem::TryOpenFile(void** pOut,const wchar_t* pathName,bit32 mode){
    IArchive* archive = FindArchive(pathName);
    if (!archive){
        return ResultArchiveNotFound();
    }

    NN_FS_ANALYSIS_LOG_INIT_TICK();
    IFile* p;
    nn::Result res = archive->OpenFile(&p, GetArchivePath(pathName), mode);
    if (res.IsSuccess()){
        *pOut = p;
    }

    NN_FS_ANALYSIS_LOG_RETURN_WITHOUT_TICK(res,"API=OpenFile,Path=%ls,mode=%c%c%c,Handle=%08X",pathName,
        (mode & OPEN_MODE_READ) ? 'R' : '_',(mode & OPEN_MODE_WRITE) ? 'W' : '_',(mode & OPEN_MODE_CREATE) ? 'C' : '_',p);
}

void UserFileSystem::CloseFile(void* p){
    if (p){
        NN_FS_ANALYSIS_LOG_NORETURN(static_cast<IFile*>(p)->Close(),"API=CloseFile,Handle=%08X",p);
    }
}

Result UserFileSystem::TryReadFile(s32* pOut, void* p, s64 offset, void* buffer, size_t size){
    if ((p == NULL) || (buffer == NULL)){
        return ResultInvalidArgument();
    }

    LatencyEmulation(true);
    NN_FS_ANALYSIS_LOG_RETURN(static_cast<IFile*>(p)->TryRead(pOut, offset, buffer, size),"API=ReadFile,Handle=%08X,Offset=%lld,Dst=0x%08X,Size=%u",
        p,offset,buffer,size
    );
}

Result UserFileSystem::TryWriteFile(s32* pOut, void* p, s64 offset, const void* buffer, size_t size, bool flush){
    if ((pOut == NULL) || (p == NULL) || (buffer == NULL)){
        return ResultInvalidArgument();
    }

    LatencyEmulation(false);
    NN_FS_ANALYSIS_LOG_RETURN(static_cast<IFile*>(p)->TryWrite(pOut, offset, buffer, size, flush),"API=WriteFile,Handle=%08X,Offset=%lld,Src=0x%08X,Size=%u,Flush=%d",
        p,offset,buffer,size,flush
    );
}

Result UserFileSystem::TryGetFileSize(s64* pOut, void* p){
    if ((pOut == NULL) || (p == NULL)){
        return ResultInvalidArgument();
    }

    NN_FS_ANALYSIS_LOG_RETURN(static_cast<const IFile*>(p)->TryGetSize(pOut),"API=GetFileSize,Handle=%08X", p);
}

Result UserFileSystem::TrySetFileSize(void* p, s64 size){
    if (p == NULL){
        return ResultInvalidArgument();
    }

    NN_FS_ANALYSIS_LOG_RETURN(static_cast<IFile*>(p)->TrySetSize(size),"API=SetFileSize,Handle=%08X,Size=%lld",
        p,size
    );
}

Result UserFileSystem::TryFlush(void* p){
    if (p == NULL){
        return ResultInvalidArgument();
    }

    NN_FS_ANALYSIS_LOG_RETURN(static_cast<IFile*>(p)->TryFlush(),"API=Flush,Handle=%08X", p);
}

void LatencyEmulation( bool isRead){
    u16 latencyTime = 0;
    u16 latencyParameter = (isRead)? (100) : (380);
    if(latencyParameter && sIsLatencyEmulationEnable){
        if(detail::sIsEmulateEndurance){
            s64 sysTick = nn::svc::GetSystemTick();
            if(sysTick & 0x10){
                latencyTime = latencyParameter / (static_cast<u16>(sysTick & 0xF) + 1);
            }
        }
        latencyTime += static_cast<u16>(sConstantWait);
        if(latencyTime > 0){
            nn::os::Thread::Sleep(nn::fnd::TimeSpan::FromMilliSeconds(latencyTime));
        }
    }
}

} // CTR
} // MPCore
} // detail

void InitializeLatencyEmulation(){
    CTR::MPCore::detail::sConstantWait = cfg::CTR::GetFsLatencyEmulationParam() * 10;
    if(cfg::CTR::IsDebugMode()){
        CTR::MPCore::detail::sIsLatencyEmulationEnable = true;
    }

    if((CTR::MPCore::detail::sIsLatencyEmulationEnable) || (CTR::MPCore::detail::sConstantWait != 0)){
        CTR::MPCore::detail::sIsEmulateEndurance = true;
    }
}

using namespace CTR::MPCore::detail;

namespace{
    IArchive* gSaveDataArchive = 0;
}

Result MountSaveData(const char* archive) {
    IArchive* p;
    Result result;

    if (gSaveDataArchive != 0) {
        return Result(0xc92044e7);
    }

    NN_FS_ANALYSIS_LOG_INIT_TICK();

    result = CTR::MPCore::detail::OpenSpecialArchiveRaw(&p, 4);
    if (result.IsSuccess()){
        result = CTR::MPCore::detail::RegisterArchive(archive,p,false,false);
        if(result.IsFailure()){
            p->DeleteObject();
        } 
        else{
            gSaveDataArchive = p;
        }
    }
    return result;
}

Result FormatSaveData(size_t maxFiles, size_t maxDirectories, bool isDuplicateAll, size_t sizeBlock){
    u32 countBucketDirectory = nn::fslow::QueryOptimalBucketCount(maxDirectories);
    u32 countBucketFile = nn::fslow::QueryOptimalBucketCount(maxFiles);
    Path dummyPath;
    NN_FS_ANALYSIS_LOG_RETURN(
        GetFileServer().FormatSaveData(4,dummyPath.GetPathType(),dummyPath.GetDataBuffer(),dummyPath.GetDataSize(),sizeBlock,maxDirectories,maxFiles,countBucketDirectory,countBucketFile,isDuplicateAll),
        "API=FormatSaveData,MaxFile=%u,MaxDir=%u,Dup=%d", maxFiles, maxDirectories, isDuplicateAll);
}

Result FormatSaveData(size_t maxFiles, size_t maxDirectories, bool isDuplicated){
    return FormatSaveData(maxFiles, maxDirectories, isDuplicated, 512);
}

Result CommitSaveData(const char* path){
    FileServerArchive* archive = reinterpret_cast<FileServerArchive *>(FindArchive(path));
    if (!archive){
        return ResultArchiveNotFound();
    }

    ArchiveHandle archiveHandle = archive->GetArchiveHandle();
    char dummy1;
    char dummy2;

    NN_FS_ANALYSIS_LOG_RETURN(GetFileServer().ControlArchive(archiveHandle, 0, &dummy1, 1, &dummy2, 1),"API=CommitSaveData,Path=%s", path);
}

Result MountRom(const char *archiveName,size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
    NN_FS_ANALYSIS_LOG_INIT_TICK();
    IArchive* p;
    NN_ERR_THROW_FATAL_ALL(OpenRom(&p, maxFile, maxDirectory, workingMemory, workingMemorySize, useCache));
    Result r = RegisterArchive(archiveName, p, false, false);
    if(r.IsFailure()){
        p->DeleteObject();
        NN_ERR_THROW_FATAL_ALL(r);
    }
    NN_FS_ANALYSIS_LOG_RETURN_WITHOUT_TICK(ResultSuccess(),"API=MountRom,Path=%s,MaxFile=%u,MaxDir=%u,UseCache=%d", archiveName, maxFile, maxDirectory, useCache);
}

Result MountRom(size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
    MountRom("rom:", maxFile, maxDirectory, workingMemory, workingMemorySize, useCache);
}

Result Unmount(const char* archiveName){
    NN_FS_ANALYSIS_LOG_INIT_TICK();
    Result res;
    bool isAlias;
    if (IArchive* p = UnregisterArchive(&isAlias, MakeArchiveName(archiveName))){
        if (!isAlias){
            p->DeleteObject();

            if (p == gSaveDataArchive){
                gSaveDataArchive = 0;
            }
        }

        res = ResultSuccess();
    }

    else{
        res = ResultArchiveNotFound();
    }
    NN_FS_ANALYSIS_LOG_RETURN_WITHOUT_TICK(res,"API=Unmount,Path=%s", archiveName);
}

s32 GetRomRequiredMemorySize(size_t maxFile, size_t maxDirectory, bool useCache){
    ProgramDataPath path;
    GetRomRequiredMemorySizeImpl(maxFile, maxDirectory, useCache, &path);
}

s32 GetRomRequiredMemorySizeImpl(size_t maxFile, size_t maxDirectory, bool useCache, ProgramDataPath* contentPath){
    Handle handle;
    {
        Path path;
        Path filePath = Path::Make(&contentPath);
        NN_ERR_THROW_FATAL_ALL(GetFileServer().OpenFileDirectly(&handle,fs::Transaction(),3,path.GetPathType(),path.GetDataBuffer(), path.GetDataSize(),filePath.GetPathType(),filePath.GetDataBuffer(), 
        filePath.GetDataSize(),OPEN_MODE_READ,fs::Attributes()));
    }

    IFile* pFile = FileServerArchive::OpenDirect(handle);
    if (!pFile){
        nn::svc::CloseHandle(handle);
        NN_ERR_THROW_FATAL_ALL(fs::ResultUnknownError());
    }

    s32 ret = RomFsArchive::GetRequiredWorkingMemorySize(pFile, maxFile, maxDirectory, useCache);
    if(ret <= 0){
        nn::svc::CloseHandle(handle);
        NN_ERR_THROW_FATAL_ALL(fs::ResultUnknownError());
    }

    pFile->Close();

    return ret;
}

Result MountSpecialArchive(const char* archiveName, bit32 archiveKind){
    IArchive* p;
    NN_UTIL_RETURN_IF_FAILED(OpenSpecialArchiveRaw(&p, archiveKind));
    NN_UTIL_RETURN_IF_FAILED_1(RegisterArchive(archiveName, p, false, false),p->DeleteObject());
    return ResultSuccess();
}

Result OpenDataContent(IArchive** pOut, const nn::fs::DataContentArchivePath& contentArchivePath, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
    ContentRomFsArchive* p = 0;
    NN_UTIL_RETURN_IF_FAILED(ContentRomFsArchive::Create(&p, contentArchivePath, maxFile, maxDirectory, workingMemory, workingMemorySize, useCache));
    *pOut = p;
    return ResultSuccess();
}

Result MountContent(const char* archiveName, MediaType mediaType, TitleId titleId, ContentIdx contentIndex, size_t maxFile, size_t maxDirectory, void* workingMemory, size_t workingMemorySize, bool useCache){
    nn::fs::DataContentArchivePath path;
    path.mediaType = mediaType;
    path.titleId = titleId;
    path.contentIdx = contentIndex;
    IArchive* p;
    NN_UTIL_RETURN_IF_FAILED(OpenDataContent(&p, path, maxFile, maxDirectory, workingMemory, workingMemorySize, useCache));
    NN_UTIL_RETURN_IF_FAILED_1(RegisterArchive(archiveName, p, nn::CTR::IsAddOnContents(titleId)),p->DeleteObject());
    return ResultSuccess();
}

Result OpenSharedExtSaveData(IArchive** pOut, const nn::fs::ExtSaveDataArchivePath& extSaveDataArchivePath){
    Path path = Path::Make(&extSaveDataArchivePath);
    bit64 lowHandle;
    NN_UTIL_RETURN_IF_FAILED_0(GetFileServer().OpenArchive(&lowHandle, 0x200000007LL, path.GetPathType(), path.GetDataBuffer(), path.GetDataSize()));
    NN_UTIL_RETURN_IF_FAILED_1(FileServerArchive::Create(pOut, gFileServerHandle, lowHandle), GetFileServer().CloseArchive(lowHandle));
    return ResultSuccess();
}

Result MountSharedExtSaveData(const char* archiveName, bit32 id){
    nn::fs::ExtSaveDataArchivePath path = nn::fs::ExtSaveDataArchivePath::Make(nn::fs::MEDIA_TYPE_NAND, static_cast<nn::fs::ExtSaveDataId>(id));
    IArchive* p;
    NN_UTIL_RETURN_IF_FAILED(OpenSharedExtSaveData(&p, path));
    NN_UTIL_RETURN_IF_FAILED_1(RegisterArchive(archiveName, p),p->DeleteObject());
    return ResultSuccess();
}

Result MountSdmc(const char* archiveName){
    NN_FS_ANALYSIS_LOG_RETURN(MountSpecialArchive(archiveName, 9),"API=MountSdmc,Path=%s", archiveName);
}

}
}