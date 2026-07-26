// Filename: nstd_Printf.cpp
//
// Project: Horizon

#include <nn/nstd/nstd_Printf.h>
#include <string>

namespace {
template <typename Type>
struct TVSNPrintfImpl{
    typedef typename Type::char_type CharT;
    struct dst_string{
        size_t len;  // remaining capacity
        CharT* cur;  // current write cursor
        CharT* base; // start of destination
    };

    static void string_put_char (dst_string* p, CharT c){
        if (p->len) {
            *p->cur = c;
            --p->len;
        }
        ++p->cur;
    }
    static void string_fill_char (dst_string* p, CharT c, int n){
        if (n > 0) {
                size_t len;
                size_t i;

                len = p->len;
                if (p->len > n){
                        len = n;
                }
                for (i = 0; i < len; ++i) {
                        p->cur[i] = c;
                    }
                p->len -= len;
                p->cur += n;
            }
        }
    static void string_put_string (dst_string* p, const CharT* s, int n){
        if (n > 0) {
            size_t len;
            size_t i;

            len = p->len;
            if (p->len > n){
                len = n;
            }
            for (i = 0; i < len; ++i){
                 p->cur[i] = s[i];
            }
            p->len -= len;
            p->cur += n;
        }
    }
    static s32 TVSNPrintf (char* dst, size_t len, const CharT* fmt, va_list vlist){
        dst_string str NN_UNUSED;
        str.len  = len;
        str.cur  = dst;
        str.base = dst;

        return 0;
    }
    static s32 TVSNPrintf (wchar_t* dst, size_t len, const CharT* fmt, va_list vlist){
        dst_string str NN_UNUSED;
        str.len  = len;
        str.cur  = dst;
        str.base = dst;

        return 0;
    }
};
} // namespace

namespace nn {
namespace nstd {

s32 TVSNPrintf (char* dst, size_t len, const char* fmt, va_list vlist){
    return TVSNPrintfImpl<std::char_traits<char> >::TVSNPrintf (dst, len, fmt, vlist);
}

s32 TSNPrintf (char* dst, size_t len, const char* fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = TVSNPrintf (dst, len, fmt, vlist);
    va_end (vlist);

    return ret;
}

s32 TSPrintf (char* dst, const char* fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = TVSNPrintf (dst, 0x7FFFFFFF, fmt, vlist);
    va_end (vlist);

    return ret;
}

/* wchar_t's */

s32 TVSNWPrintf(wchar_t* dst, size_t len, const wchar_t* fmt, va_list vlist){
    return TVSNPrintfImpl<std::char_traits<wchar_t> >::TVSNPrintf (dst, len, fmt, vlist);
}

s32 TSNWPrintf(wchar_t *dst, size_t len, const wchar_t *fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = TVSNPrintfImpl<std::char_traits<wchar_t> >::TVSNPrintf(dst,len,fmt,vlist);
    va_end (vlist);

    return ret;
}

s32 TSWPrintf (wchar_t* dst, const wchar_t* fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = TVSNWPrintf(dst, 0x7FFFFFFF, fmt, vlist);
    va_end (vlist);

    return ret;
}

}
}


extern "C" {

s32 nnnstdTVSNPrintf (char* dst, size_t len, const char* fmt, va_list vlist){
        return nn::nstd::TVSNPrintf (dst, len, fmt, vlist);
}

s32 nnnstdTSNPrintf (char* dst, size_t len, const char* fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = nn::nstd::TSNPrintf (dst, len, fmt, vlist);
    va_end (vlist);

    return ret;
}

s32 nnnstdTSPrintf (char* dst, const char* fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = nn::nstd::TSPrintf (dst, fmt, vlist);
    va_end (vlist);

    return ret;
}

/* wchar_t */

s32 nnnstdTVSNWPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, va_list vlist){
        return nn::nstd::TVSNWPrintf (dst, len, fmt, vlist);
}

s32 nnnstdTSNWPrintf (wchar_t* dst, size_t len, const wchar_t* fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = nn::nstd::TSNWPrintf (dst, len, fmt, vlist);
    va_end (vlist);

    return ret;
}

s32 nnnstdTSWPrintf (wchar_t* dst, const wchar_t* fmt, ...){
    va_list vlist;
    s32     ret;

    va_start (vlist, fmt);
    ret = nn::nstd::TSWPrintf(dst, fmt, vlist);
    va_end (vlist);

    return ret;
}
}