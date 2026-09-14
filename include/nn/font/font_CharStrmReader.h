#pragma once

#include <nn/types.h>
#include <nn/Assert.h>

namespace nn {
namespace font {

class CharStrmReader{
    friend class Font;
public:
    CharStrmReader(const CharStrmReader& rhs):
        mCharStrm(rhs.mCharStrm),
        mReadFunc(rhs.mReadFunc)
    {
    }
    ~CharStrmReader() 
    {
    }

    void Set(const char* stream)
    {
        this->mCharStrm = stream;
    }
    void Set(const wchar_t* stream)
    {
        this->mCharStrm = stream;
    }
    const void* GetCurrentPos() const { return mCharStrm; }
    u16 Next() { return (this->*mReadFunc)(); }
private:
    typedef u16 (CharStrmReader::*ReadNextCharFunc)();

    const void* mCharStrm;
    const ReadNextCharFunc mReadFunc;

    explicit CharStrmReader(ReadNextCharFunc func):
        mCharStrm(NULL),
        mReadFunc(func)
    {
    }

    template<typename CharType>
    CharType GetChar(int offset = 0) const
    {
        const CharType* charStrm = reinterpret_cast<const CharType*>(m_CharStrm);
        return *(charStrm + offset);
    }

    template<typename CharType>
    void StepStrm(int step = 1)
    {
        const CharType*& charStrm = reinterpret_cast<const CharType*&>(m_CharStrm);
        charStrm += step;
    }

    u16 ReadNextCharUTF8();
    u16 ReadNextCharUTF16();
    u16 ReadNextCharCP1252();
    u16 ReadNextCharSJIS();
};

}
}