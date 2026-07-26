// Filename: cx_StreamingUncompression.cpp
//
// Project: Horizon

#include <nn/cx/cx_StreamingUncompression.h>
#include <nn/Assert.h>

namespace nn{
namespace cx{
namespace{

inline u32 ReadHeader(u8* headerSize, s32 *destCount, const u8* srcp, u32 srcSize, s32 forceDestSize){
    NN_NULL_TASSERT_(headerSize);
    NN_NULL_TASSERT_(destCount);

    u32 readLen = 0;
    while (*headerSize > 0){
        --(*headerSize);
        if (*headerSize <= 3){
            *destCount |= (*srcp << ((3 - *headerSize) * 8));
        }
        else if (*headerSize <= 6){
            *destCount |= (*srcp << ((6 - *headerSize) * 8));
        }
        ++srcp;
        ++readLen;
        if (*headerSize == 4 && *destCount > 0){
            *headerSize = 0;
        }
        if (--srcSize == 0 && *headerSize > 0){
            return readLen;
        }
    }

    if ((forceDestSize > 0) && (forceDestSize < *destCount)){
        *destCount = forceDestSize;
    }
    return readLen;
}

} // namespace

void InitUncompContextLZ( UncompContextLZ *context, void* dest ){
    NN_NULL_TASSERT_(context);
    context->destp = reinterpret_cast<u8*>(dest);
    context->destCount = 0;
    context->flags = 0;
    context->flagIndex = 0;
    context->length = 0;
    context->lengthFlg = 3;
    context->headerSize = 8;
    context->exFormat = 0;
    context->forceDestCount = 0;
}

s32 ReadUncompLZ(UncompContextLZ *context, const void* data, u32 len){
    NN_NULL_TASSERT_(context);
    NN_NULL_TASSERT_(data);

    const u8* srcp = (const u8*)data;
    s32 offset;

    if (context->headerSize > 0){
        u32 read_len;

        if (context->headerSize == 8){
            if ((*srcp & 0xF0) != 0x10){
                return -1;
            }

            context->exFormat = (u8)(*srcp & 0x0F);
            if ((context->exFormat != 0x0) && (context->exFormat != 0x1)){
                return -1;
            }
        }
        read_len = ReadHeader(&context->headerSize, &context->destCount, srcp, len, context->forceDestCount);
        srcp += read_len;
        len-= read_len;
        if (len == 0){
            return (context->headerSize == 0)? context->destCount : -1;
        }
    }

    while (context->destCount > 0){
        while (context->flagIndex > 0){
            if (len == 0){
                return context->destCount;
            }

            if (!(context->flags & 0x80)){
                *context->destp++ = *srcp++;
                context->destCount--;
                len--;
            }
            else{

                while (context->lengthFlg > 0){
                    --context->lengthFlg;
                    if (!context->exFormat){
                        context->length  = *srcp++;
                        context->length += (3 << 4);
                        context->lengthFlg = 0;
                    }
                    else{
                        switch ( context->lengthFlg ){
                        case 2:{
                                context->length = *srcp++;
                                if ((context->length >> 4) == 1){
                                    context->length =  (context->length & 0x0F) << 16;
                                    context->length += ((0xFF + 0xF + 3) << 4);
                                }
                                else if ( (context->length >> 4) == 0 ){
                                    context->length =  (context->length & 0x0F) << 8;
                                    context->length += ((0xF + 2) << 4);
                                    context->lengthFlg = 1;
                                }
                                else{
                                    context->length += (1 << 4);
                                    context->lengthFlg = 0;
                                }
                            }
                            break;
                        case 1:{
                                context->length += (*srcp++ << 8);
                            }
                            break;
                        case 0:{
                                context->length += *srcp++;
                            }
                            break;
                        }
                    }
                    if(--len == 0){
                        return context->destCount;
                    }
                }

                offset = (context->length & 0xF) << 8;
                context->length = context->length >> 4;
                offset = (offset | *srcp++) + 1;
                len--;
                context->lengthFlg = 3;

                if (context->length > context->destCount){
                    if (context->forceDestCount == 0){
                        return -4;
                    }
                    context->length = context->destCount;
                }
                while (context->length > 0){
                    *context->destp = context->destp[-offset];
                    context->destp++;
                    context->destCount--;
                    context->length--;
                }
            }

            if (context->destCount == 0){
                goto out;
            }
            context->flags <<= 1;
            context->flagIndex--;
        }

        if (len == 0){
            return context->destCount;
        }

        context->flags = *srcp++;
        context->flagIndex = 8;
        len--;
    }

out:
    if ((context->forceDestCount == 0) && (len > 32)){
        return -3;
    }
    return 0;
}

}
}