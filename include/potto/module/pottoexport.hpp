#ifndef POTTOEXPORT_H_
#define POTTOEXPORT_H_
#pragma once

#include <mutex>

#include <potto/module/pottodef.hpp>
#include <potto/module/pottomodule.hpp>
#include <potto/pottointerface.hpp>
#include <potto/pottoptr.hpp>

Potto::POTTO_ERROR ModuleCanUnloadNow() {
  return (Potto::PottoModule::GetInstance().GetLockCount() == 0 ? Potto::POTTO_E_OK
                                                                : Potto::POTTO_E_FAIL);
}

Potto::POTTO_ERROR ModuleGetClassObject(const Potto::PottoUuid& clsid, const Potto::PottoUuid& iid,
                                        void** ppv) {
  auto it = Potto::PottoModule::GetInstance().GetClassEntryMap().find(clsid);
  if (it == Potto::PottoModule::GetInstance().GetClassEntryMap().end() || nullptr == it->second)
    return Potto::POTTO_E_CLASSNOTAVAILABLE;

  Potto::ClassEntry* pEntry = it->second;

  Potto::POTTO_ERROR error = Potto::POTTO_E_NOINTERFACE;
  if (nullptr != pEntry->pfnClassFactoryCreator) {
    if (nullptr == pEntry->pCachedIUnknown) {
      {
        std::lock_guard<std::mutex> lock(Potto::PottoModule::GetInstance().GetModuleMutex());

        if (nullptr == pEntry->pCachedIUnknown) {
          Potto::IUnknown* pUnknown = nullptr;
          error = pEntry->pfnClassFactoryCreator(Potto::IID_IUnknown, (void**)&pUnknown);
          if (Potto::POTTO_E_OK == error || nullptr != pUnknown)
            pEntry->pCachedIUnknown = pUnknown;
        }
      }

      if (pEntry->pCachedIUnknown && Potto::IsEqualUuid(Potto::IID_IClassFactory, iid)) {
        Potto::PottoPtr<Potto::IClassFactory> pClassFactory;
        error = pEntry->pCachedIUnknown->QueryInterface(Potto::IID_IClassFactory,
                                                        (void**)&pClassFactory);
        if (Potto::POTTO_E_OK == error && pClassFactory)
          pClassFactory->SetCreatorFunction(pEntry->pfnObjectCreator);
      }
    }

    if (nullptr != pEntry->pCachedIUnknown)
      error = pEntry->pCachedIUnknown->QueryInterface(iid, ppv);
  }

  return error;
}

Potto::POTTO_ERROR RegisterModule(Potto::PottoUuid& moduleId, Potto::ClassInfoList& classInfoList) {
  moduleId = Potto::PottoModule::GetInstance().GetId();

  for (auto it = Potto::PottoModule::GetInstance().GetClassEntryMap().begin();
       it != Potto::PottoModule::GetInstance().GetClassEntryMap().end(); it++) {
    classInfoList.push_back(Potto::ClassInfo{it->first, it->second->className});
  }

  return Potto::POTTO_E_OK;
}

#endif // POTTOEXPORT_H_
