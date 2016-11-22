#ifndef POTTOEXPORT_H_
#define POTTOEXPORT_H_
#pragma once

#include <mutex>
#include <potto/pottoptr.h>
#include <potto/pottointerface.h>
#include <potto/internal/pottodef.h>
#include <potto/internal/pottomodule.h>

#ifdef _WIN32
#ifdef _WIN64
#pragma comment(linker, "/EXPORT:ModuleCanUnloadNow=?ModuleCanUnloadNow@@YAHXZ,PRIVATE")
#pragma comment(linker, "/EXPORT:ModuleGetClassObject=?ModuleGetClassObject@@YAHAEBV_PottoUuid@Potto@@0PEAPEAX@Z,PRIVATE")
#pragma comment(linker, "/EXPORT:RegisterModule=?RegisterModule@@YAHAEAV_PottoUuid@Potto@@AEAV?$vector@VClassInfo@Potto@@V?$allocator@VClassInfo@Potto@@@std@@@std@@@Z,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:ModuleCanUnloadNow=?ModuleCanUnloadNow@@YAHXZ,PRIVATE")
#pragma comment(linker, "/EXPORT:ModuleGetClassObject=?ModuleGetClassObject@@YAHABV_PottoUuid@Potto@@0PAPAX@Z,PRIVATE")
#pragma comment(linker, "/EXPORT:RegisterModule=?RegisterModule@@YAHAAV_PottoUuid@Potto@@AAV?$vector@VClassInfo@Potto@@V?$allocator@VClassInfo@Potto@@@std@@@std@@@Z,PRIVATE")
#endif
#endif

POTTO_ERROR ModuleCanUnloadNow()
{
	return (Potto::PottoModule::GetInstance().GetLockCount() == 0 ? POTTO_E_OK : POTTO_E_FAIL);
}

POTTO_ERROR ModuleGetClassObject(const Potto::PottoUuid& clsid, const Potto::PottoUuid& iid, void** ppv)
{
	auto it = Potto::PottoModule::GetInstance().GetClassEntryMap().find(clsid);
	if (it == Potto::PottoModule::GetInstance().GetClassEntryMap().end() && nullptr != it->second)
		return POTTO_E_CLASSNOTAVAILABLE;

	Potto::ClassEntry* pEntry = it->second;

	POTTO_ERROR error = POTTO_E_NOINTERFACE;
	if (nullptr != pEntry->pfnClassFactoryCreator)
	{
		if (nullptr == pEntry->pCachedIUnknown)
		{
			{
				std::lock_guard<std::mutex> lock(Potto::PottoModule::GetInstance().GetModuleMutex());

				if (nullptr == pEntry->pCachedIUnknown)
				{
					Potto::IUnknown* pUnknown = nullptr;
					error = pEntry->pfnClassFactoryCreator(Potto::IID_IUnknown, (void**)&pUnknown);
					if (POTTO_E_OK == error || nullptr != pUnknown)
						pEntry->pCachedIUnknown = pUnknown;
				}
			}

			if (pEntry->pCachedIUnknown && Potto::IsEqualUuid(Potto::IID_IClassFactory, iid))
			{
				Potto::PottoPtr<Potto::IClassFactory> pClassFactory;
				error = pEntry->pCachedIUnknown->QueryInterface(Potto::IID_IClassFactory, (void**)&pClassFactory);
				if (POTTO_E_OK == error && pClassFactory)
					pClassFactory->SetCreatorFunction(pEntry->pfnObjectCreator);
			}
		}
		
		if (nullptr != pEntry->pCachedIUnknown)
			error = pEntry->pCachedIUnknown->QueryInterface(iid, ppv);
	}

	return error;
}

POTTO_ERROR RegisterModule(Potto::PottoUuid& moduleId, Potto::ClassInfoList& classInfoList)
{
	moduleId = Potto::PottoModule::GetInstance().GetId();

	for (auto it = Potto::PottoModule::GetInstance().GetClassEntryMap().begin();
		it != Potto::PottoModule::GetInstance().GetClassEntryMap().end(); it++)
	{
		classInfoList.push_back(Potto::ClassInfo
		{
			it->first,
			it->second->className
		});
	}

	return POTTO_E_OK;
}

#endif // POTTOEXPORT_H_