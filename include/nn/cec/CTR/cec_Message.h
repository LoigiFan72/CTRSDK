#pragma once

#include <nn/fnd.h>
#include <nn/cec/CTR/cec_Types.h>

namespace nn{
namespace cec{
namespace CTR{

struct CecMessageHeader;

#define CEC_EXHEADER_NUM_MAX    (16)
#define CEC_EXHEADER_SIZE_MAX   (8*1024)

struct CecMessageExHeader{
    u32  exHeaderType;
    u32  exHeaderLen;
    u8*  exHeaderData;
};

#define LOAD_FLAG_POINTER   1
#define LOAD_FLAG_MALLOC    0

class Message{
private:
    struct CecMessageHeader m_cec_mh;
    struct CecMessageExHeader m_cec_mhex[CEC_EXHEADER_NUM_MAX] __attribute__((aligned(4)));
    s8  numOfExHeader;
    u8  m_flag_pointer;
    u8  m_flag_input;
    s8 __PADDING__;
    u8* m_messBody;
    u32 m_messBodyLen;
    u8* m_pMessBody;
    u8*     m_pHash;
    u32     m_hashSize;
    u8  m_hmacKey[32];

    u32 calcCecMessSize();
    Result      Init_Message();
    Result      SetExHeaderWithoutCalc(MessageExHeaderType exhType, size_t exhLen, const void* exhBody);
public:
    Message();
    ~Message();

    Result NewMessage(TitleId cecTitleId, u32 groupId, MessageTypeFlag messageTypeFlag, SendMode sendMode, u8 sendCount, u8 propagationCount, const void* icon, size_t iconSize,
        const wchar_t* infoTextData, size_t infoTextSize);

    Result  SetCecTitleId(TitleId cecTitleId);
    TitleId GetCecTitleId() const{ return m_cec_mh.cecTitleId; }
    u32     GetGroupID() const { return m_cec_mh.groupId; }
    u32     GetSessionID() const{ return m_cec_mh.sessionId; }
    u32     GetMessSize() const { return m_cec_mh.messSize; }
    u32     GetHeaderSize() const{ return m_cec_mh.headerSize; }
    
    Result  SetBodySize(u32 bodySize);
    u32     GetBodySize() const { return m_cec_mh.bodySize; }

    u32     GetMessageVersion() const { return m_cec_mh.messVersion; }
    MessageTypeFlag GetMessageTypeFlag() const{ return m_cec_mh.messageTypeFlag;}

    SendMode GetSendMode() const{ return m_cec_mh.sendMode; }

    u64  GetSenderID() const{ return m_cec_mh.senderId; }

    nn::fnd::DateTimeParameters GetSendDate() const{ return m_cec_mh.sendDate; }
    nn::fnd::DateTimeParameters GetRecvDate() const{ return m_cec_mh.recvDate; }
    nn::fnd::DateTimeParameters GetCreateDate() const { return m_cec_mh.createDate; }

    u8      GetSendCount() const{ return m_cec_mh.sendCount;}
    u8      GetPropagationCount() const{ return m_cec_mh.propagationCount; }
    u8      GetFlag_Unread() const{ return m_cec_mh.flagUnread; }
    u8      GetFlag_New() const{ return m_cec_mh.flagNew; }
    bit16   GetTag() const{ return m_cec_mh.tag;}

    Result        SetExHeader(MessageExHeaderType exhType, size_t exhLen, const void* exhBody);
    Result        GetExHeader(MessageExHeaderType exhType, size_t* exhLen, void** exhBody) const;

    inline Result SetIcon(void* iconData, size_t iconSize){ return SetExHeader(2, iconSize, iconData); }
    inline Result GetIcon(void** iconData, size_t* iconSize) const{ return GetExHeader(2, iconSize, iconData); }

    inline Result SetInfoText(const wchar_t* infoTextData, size_t infoTextSize){ return SetExHeader(4 , infoTextSize, infoTextData);}
    Result        GetInfoText(const wchar_t** infoTextData, size_t* infoTextSize) const;

    Result SetMessageBody(const void* dataBody, size_t size);
    u32    GetMessageBody(void* dataBody, size_t size) const;

    Result  InputMessage(const void* mess, size_t size);
    u32     MakeMessageBinary(void* messData) const;
    void    OutputMessageHeader(void* pHeaderBuf) const;    
};



}
}
}