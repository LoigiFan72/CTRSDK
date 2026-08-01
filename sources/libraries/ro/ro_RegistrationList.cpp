// Filename: ro_RegistrationList.cpp
//
// Project: Horizon

#include <nn/ro.h>
#include <nn/ro/ro_ObjectFile.h>
#include <nn/ro/ro_DynamicLoader.h>

namespace nn{
namespace ro{

Result RegistrationList::Unregister(){
    Result res;
    ModuleRegistrationListHeader* p = reinterpret_cast<ModuleRegistrationListHeader*>(this);
    
    ModuleRegistrationListHeader* pOther = (p->node.pPrev != NULL) ? p->node.pPrev: p->node.pNext;

    res = detail::DynamicLoader::UnregisterList(PSEUDO_HANDLE_CURRENT_PROCESS, GetHead());

    if(res.IsSuccess()){
        detail::UpdateRegistrationListNode(reinterpret_cast<RegistrationList*>(pOther));
    }

    return res;
}

}
}