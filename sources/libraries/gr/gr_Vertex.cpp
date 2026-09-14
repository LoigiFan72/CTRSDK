// Filename: gr_Vertex.cpp
//
// Project: Horizon

#include <nn/gr/CTR/gr_Vertex.h>

namespace nn {
namespace gr {
namespace CTR {

void Vertex::LoadArray::CheckDisable()
{
    if (!IsEnable())
    {
        return;
    }

    for (u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; ++index)
    {
        if (bind[index] != -1)
        {
            return;
        }
    }

    physicalAddr = NULL;

    for (u32 index = 0; index < VERTEX_ATTRIBUTE_MAX; ++index)
    {
        byte[index] = 0;
    }
}

void Vertex::DisableAttr_(const bit32 bind_reg){    
    NN_ASSERT_(bind_reg < VERTEX_ATTRIBUTE_MAX);

    if (!m_IsEnableReg[bind_reg]) return;

    m_CmdCacheVertexNum = 0;
                
    if (m_AttrConst[bind_reg].IsEnable())
    {
        m_AttrConst[bind_reg].Disable();

        m_IsEnableReg[bind_reg] = false;
        return;
    }

    for (int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i)
    {
        if (!m_LoadArray[i].IsEnable()) continue;

        for (int j = 0; j < VERTEX_ATTRIBUTE_MAX; ++j)
        {
            if (m_LoadArray[i].bind[j] != bind_reg) continue;
                        
            m_LoadArray[i].bind[j] = -1;                       
            m_IsEnableReg[bind_reg] = false;

            m_LoadArray[i].CheckDisable();
                        
            return;
        }
    }

    NN_ASSERT_(mIsEnableReg[bind_reg] == false);
}

void Vertex::EnableAttrAsArray(const BindSymbolVSInput& symbol, const uptr physical_addr, const PicaDataVertexAttrType type )
{
    const bit32 bind_reg = symbol.start;
    const u32 byte     = PicaDataVertexAttrTypeToByteSize(type);
        NN_ASSERT_(bind_reg < VERTEX_ATTRIBUTE_MAX);

        DisableAttr_(bind_reg);

        LoadArray* array = NULL;
        for (int i = 0; i < VERTEX_ATTRIBUTE_MAX; ++i)
        {
            if (m_LoadArray[i].IsEnable() == false)
            {
                array = &m_LoadArray[i];
                break;
            }
        }
        NN_ASSERT_(array != NULL);

        m_IsEnableReg[bind_reg] = true;
        array->physicalAddr     = physical_addr;
        array->type[0]          = type;
        array->bind[0]          = bind_reg;
        array->byte[0]          = byte;

        for (int i = 1; i < VERTEX_ATTRIBUTE_MAX; ++i)
        {
            array->bind[i] = -1;
            array->byte[i] = 0;
        }

        m_CmdCacheVertexNum = 0;
    }
}
}
}