// !All of this shown here is custom work done by user Luigifan27 using AI!
//
// Not much is known about the RO Service so Im just making this shit up LMAO
//
// Checkout: 
// https://www.3dbrew.org/wiki/RO_Services
// https://problemkaputt.de/gbatek-3ds-files-crr0-and-cro0-files.htm
// https://www.3dbrew.org/wiki/CRO0


#pragma once

#include <nn/ro/ro_Offset.h>
#include <cstring>
#include <nn/Assert.h>
#include <nn/util/util_FlagsEnum.h>
#include <nn/util/util_SizedEnum.h>
#include <nn/crypto/crypto_Sha256Context.h>

namespace nn{ 
namespace ro{

template <typename FirstType, int FirstSize, typename SecondType, int SecondSize>
class PairInWord{
private:
    uptr    mValue;
public:
    FirstType GetFirst() const      { return static_cast<FirstType>(GetBits<0, FirstSize>()); }
    SecondType GetSecond() const    { return static_cast<SecondType>(GetBits<FirstSize, SecondSize>()); }
private:
    template<int Offset, int Size>
    uptr GetBits() const{ return (mValue << (sizeof(uptr) * 8 - (Offset + Size))) >> (sizeof(uptr) * 8 - Size); }
};

template <class T>
struct LinkedListNode{
    T* pNext;
    T* pPrev;

    typedef LinkedListNode<T>   SelfT;

    template <class ParentT>
    void EnlistTail(ParentT* pItem){
        NN_POINTER_TASSERT_(this);
        NN_POINTER_TASSERT_(pItem);

        ParentT* pLast = this->pPrev;
        SelfT& lastNode = (pLast != NULL) ? pLast->node: *this;

        pItem->node.pPrev = pLast;
        pItem->node.pNext = NULL;

        lastNode.pNext = pItem;
        this->pPrev   = pItem;
    }

    template <class ParentT>
    void DelistItem(ParentT* pItem){
        NN_POINTER_TASSERT_(this);
        NN_POINTER_TASSERT_(pItem);

        ParentT* pNext = pItem->node.pNext;
        ParentT* pPrev = pItem->node.pPrev;
        SelfT& nextNode = (pNext != NULL) ? pNext->node: *this;
        SelfT& prevNode = (pPrev != NULL) ? pPrev->node: *this;

        prevNode.pNext = pNext;
        nextNode.pPrev = pPrev;
        pItem->node.pNext = NULL;
        pItem->node.pPrev = NULL;
    }
};

enum Section{
    SECTION_CODE,
    SECTION_RO,
    SECTION_RW,
    SECTION_ZI,

    SECTION_MAX_BITS = (1 << 7)
};

enum RelocationType{
    RELOCATION_TYPE_ARM_NONE        =  0,
    RELOCATION_TYPE_ARM_ABS32       =  2,
    RELOCATION_TYPE_ARM_REL32       =  3,
    RELOCATION_TYPE_ARM_THM_CALL    = 10,
    RELOCATION_TYPE_ARM_CALL        = 28,
    RELOCATION_TYPE_ARM_JUMP24      = 29,
    RELOCATION_TYPE_ARM_TARGET1     = 38,
    RELOCATION_TYPE_ARM_PREL31      = 42
};

typedef void (*PrologFunction)();

typedef void (*EpilogFunction)();

typedef void (*UnresolvedFunction)();

typedef PairInWord<u8, 4, u32, 28> SectionAndOffset;

struct SectionInfo{
    OffsetPointer<bit8> offset;
    size_t size;
    util::SizedEnum1<Section> section;
    s8 rev[3];
};
 
struct InternalRelocationTableEntry{
    SectionAndOffset sectionAndOffset;
    util::SizedEnum1<RelocationType> type;
    u8 refedSection;
    short rev;
    s32 param;
};

struct ExternalRelocationTableEntry{
    SectionAndOffset sectionAndOffset;
    util::SizedEnum1<RelocationType> type;
    bool isLast;
    bool isResolved;
    s8 rev;
    bit32 param;
};

struct StaticRelocationTableEntry{
    SectionAndOffset sectionAndOffset;
    util::SizedEnum1<RelocationType> type;
    bool isLast;
    bool isResolved;
    s8 rev;
    bit32 param;
};

struct SymbolImportTableEntry{
    OffsetPointer<char> symbol;
    OffsetPointer<ExternalRelocationTableEntry> relocationBegin;
};

struct IndexImportTableEntry{
    s32 index;
    OffsetPointer<ExternalRelocationTableEntry> relocationBegin;
};

struct OffsetImportTableEntry{
    SectionAndOffset sectionAndOffset;
    OffsetPointer<ExternalRelocationTableEntry> relocationBegin;
};

struct SymbolExportTableEntry{
    OffsetPointer<char> symbol;
    SectionAndOffset sectionAndOffset;
};

struct IndexExportTableEntry{
    SectionAndOffset sectionAndOffset;
};

struct OffsetExportTableEntry{
    SectionAndOffset sectionAndOffset;
    OffsetPointer<ExternalRelocationTableEntry> relocationBegin;
};

struct ObjectInfo{
    OffsetPointer<char> name;
    OffsetPointer<IndexImportTableEntry> indexImportTable;
    s32 numIndexImports;
    OffsetPointer<OffsetImportTableEntry> offsetImportTable;
    s32 numOffsetImports;
};

struct PatriciaNode{
    u16 ref;
    bit16 idxLeft;
    bit16 idxRight;
    s16 value;
};

struct Hash{
    bool operator <(const Hash& rhs) const{
        return std::memcmp(this, &rhs, sizeof(rhs)) < 0;
    }
    bit32 hashsize[crypto::Sha256Context::HASH_SIZE / sizeof(bit32)]; // 1 SHA256 Block
};

struct HashSet{
    Hash hash0;
    Hash hash1;
    Hash hash2;
    Hash hash3;
};

class ModuleHeader;

typedef LinkedListNode<ModuleHeader> ModuleHeaderListNode;

class ModuleHeader{
public:
    HashSet ModuleHash;
    bit32 signature; // "CRO0" / "CRR0"

    OffsetPointer<char> moduleName;
    ModuleHeaderListNode node;

    size_t size;
    size_t bufferSize;
    size_t fixedSize;
    s32 ret;

    SectionAndOffset control;
    SectionAndOffset prolog;
    SectionAndOffset epilog;
    SectionAndOffset unresolved;

    OffsetPointer<bit8> codeBinary;
    size_t codeBinarySize;

    OffsetPointer<bit8> heapBinary;
    size_t heapBinarySize;

    OffsetPointer<char> baseStringTable;
    size_t baseStringTableSize;

    OffsetPointer<SectionInfo>                  sectionInfo;
    s32 numSections;

    OffsetPointer<SymbolExportTableEntry> symbolExportTable;
    s32 numSymbolExports;

    OffsetPointer<IndexExportTableEntry> indexExportTable;
    s32 numIndexExports;

    OffsetPointer<char> exportStringTable;
    size_t exportStringTableSize;
        
    OffsetPointer<PatriciaNode> symbolExportDictionary;
    s32 numSymbolExportDictionaryNode;

    OffsetPointer<ObjectInfo> referenceObjects;
    s32 numObjects;

    OffsetPointer<ExternalRelocationTableEntry> externalRelocation;
    s32 numExternalRelocations;

    OffsetPointer<SymbolImportTableEntry> symbolImportTable;
    s32 numSymbolImports;

    OffsetPointer<IndexImportTableEntry> indexImportTable;
    s32 numIndexImports;

    OffsetPointer<OffsetImportTableEntry> offsetImportTable;
    s32 numOffsetImports;

    OffsetPointer<char> importStringTable;
    size_t importStringTableSize;

    OffsetPointer<OffsetExportTableEntry> offsetExportTable;
    s32 numOffsetExports;

    OffsetPointer<InternalRelocationTableEntry> internalRelocation;
    s32 numInternalRelocations;

    OffsetPointer<StaticRelocationTableEntry>   staticRelocation;
    s32 numStaticRelocations;
};

struct ModuleRegistrationListHeader;
typedef LinkedListNode<ModuleRegistrationListHeader> ModuleRegistrationListHeaderListNode;

struct Sign{
    bit32 data[256 / sizeof(bit32)];
};

struct SignKey{
    bit32 data[256 / sizeof(bit32)];
};

struct ListCert{
    bit32 uniqueIdMask;
    bit32 uniqueIdPattern;
    bit8 unkpad[0x18];

    SignKey key;
    Sign sign;
};

struct DebugInfoBody{
    OffsetPointer<wchar_t> pathOffset;
    size_t pathLength;
};

struct DebugInfoMapEntry{
    OffsetPointer<DebugInfoBody> bodyOffset;
    size_t bodySize;
};

struct DebugInfoHeader{
    OffsetPointer<DebugInfoMapEntry> tableOffset;
    s32 numTableEntry;
    OffsetPointer<DebugInfoBody> bodyOffset;
    size_t bodySize;
    bit8 unk[0x10];
};

struct ModuleRegistrationListHeader{
    bit32 signature;
    s32 unkpad;
    ModuleRegistrationListHeaderListNode node;
    OffsetPointer<DebugInfoHeader> debugInfoOffset;
    size_t debugInfoSize;
    bit8 padding_0x0010[0x08];

    ListCert cert;
    Sign sign;

    bit32 uniqueId;
    size_t size;
    bit8 unkpad2[8];

    OffsetPointer<Hash> hashOffset;
    size_t numHash;
    OffsetPointer<bit8> moduleIdOffset;
    size_t moduleIdSize;
};

}
}
