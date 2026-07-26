#pragma once

namespace nn { 
namespace math {
namespace ARMv6{

inline QUAT* MTX34ToQUATC(QUAT* pOut, const MTX34* pMtx){
    f32 tr, s;
    s32 i, j, k;
    s32 nxt[3] = {1, 2, 0};
    f32 q[3];

    const f32 (*const m)[4] = pMtx->matrix;

    tr = m[0][0] + m[1][1] + m[2][2];
    if ( tr > 0.0f ){
        s = (f32)::std::sqrtf(tr + 1.0f);
        pOut->w = s * 0.5f;
        s = 0.5f / s;
        pOut->x = (m[2][1] - m[1][2]) * s;
        pOut->y = (m[0][2] - m[2][0]) * s;
        pOut->z = (m[1][0] - m[0][1]) * s;
    }
    else {
        i = 0;
        if (m[1][1] > m[0][0]) i = 1;
        if (m[2][2] > m[i][i]) i = 2;
        j = nxt[i];
        k = nxt[j];
        s = (f32)::std::sqrtf( (m[i][i] - (m[j][j] + m[k][k])) + 1.0f );
        q[i] = s * 0.5f;
        
        if (s!=0.0f)
            s = 0.5f / s;
        
        pOut->w = (m[k][j] - m[j][k]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;

        pOut->x = q[0];
        pOut->y = q[1];
        pOut->z = q[2];
    }
    
    return pOut;
}
inline QUAT* MTX34ToQUATC_FAST(QUAT* pOut, const MTX34* pMtx){
    f32 tr;
    s32 i;

    const f32 (*const m)[4] = pMtx->matrix;

    tr = m[0][0] + m[1][1] + m[2][2];
    if ( tr > 0.0f ){
        f32 x, y, z, w, s;
        s = (f32)::std::sqrtf(tr + 1.0f);
        w = s * 0.5f;
        s = 0.5f / s;
        x = (m[2][1] - m[1][2]) * s;
        y = (m[0][2] - m[2][0]) * s;
        z = (m[1][0] - m[0][1]) * s;
        pOut->w = w;
        pOut->x = x;
        pOut->y = y;
        pOut->z = z;
    }
    else {
        f32 x, y, z, w, s;
        i = 0;

        if (m[1][1] > m[0][0]){
            if (m[2][2] > m[1][1]) i = 2;
            else    i=1;
        }
        else{
            if (m[2][2] > m[0][0]) i = 2;
        }

        if(i==0){
            s = (f32)::std::sqrtf( (m[0][0] - (m[1][1] + m[2][2])) + 1.0f );
            x = s * 0.5f;
            
            if (s!=0.0f)
                s = 0.5f / s;
            
            w = (m[2][1] - m[1][2]) * s;
            y = (m[0][1] + m[1][0]) * s;
            z = (m[0][2] + m[2][0]) * s;

            pOut->x = x;
            pOut->w = w;
            pOut->y = y;
            pOut->z = z;


        }
        else if(i==1){
            s = (f32)::std::sqrtf( (m[1][1] - (m[2][2] + m[0][0])) + 1.0f );
            y = s * 0.5f;
            
            if (s!=0.0f)
                s = 0.5f / s;
            
            w = (m[0][2] - m[2][0]) * s;
            z = (m[1][2] + m[2][1]) * s;
            x = (m[1][0] + m[0][1]) * s;

            pOut->y = y;
            pOut->w = w;
            pOut->z = z;
            pOut->x = x;
        }
        else{
            s = (f32)::std::sqrtf((m[2][2] - (m[0][0] + m[1][1])) + 1.0f);
            z = s * 0.5f;
            
            if (s!=0.0f)
                s = 0.5f / s;
            
            w = (m[1][0] - m[0][1]) * s;
            x = (m[2][0] + m[0][2]) * s;
            y = (m[2][1] + m[1][2]) * s;

            pOut->z = z;
            pOut->w = w;
            pOut->x = x;
            pOut->y = y;

        }
    }
    
    return pOut;
}

}

inline QUAT* MTX34ToQUAT(QUAT* pOut, const MTX34* pMtx){
    #ifdef NN_BUILD_DEBUG
        return ARMv6::MTX34ToQUATC(pOut, pMtx);
    #else
        return ARMv6::MTX34ToQUATC_FAST(pOut, pMtx);
    #endif
}


}
}