#include <windows.h>
#include "PottoModuleEntryManager.h"

namespace Potto
{
	PottoModuleEntryManager& PottoModuleEntryManager::GetInstance()
	{
		// Singleton instance.
		static PottoModuleEntryManager s_instance;
		return s_instance;
	}

	POTTO_ERROR Potto::PottoModuleEntryManager::LoadModule(const std::string& moduleName, ModuleEntryPtr& pModule)
	{
		HMODULE hMod = nullptr;
		{
			// Find or load the module
			std::lock_guard<std::mutex> lock(m_mtxForLoadModule);
			hMod = ::GetModuleHandleA(moduleName.c_str());
			if (nullptr == hMod)
				hMod = ::LoadLibraryA(moduleName.c_str());
		}

		// Module was not found
		if (nullptr == hMod)
			return POTTO_E_MODULENOTFOUND;

		// Get all required function pointers
		TypeModuleGetClassObject pfnModuleGetClassObject = (TypeModuleGetClassObject)
			::GetProcAddress(hMod, ModuleGetClassObjectName);
		if (!pfnModuleGetClassObject)
		{
			::FreeLibrary(hMod);
			return POTTO_E_EXPORTNOTFOUND;
		}

		TypeModuleCanUnloadNow pfnModuleCanUnloadNow = (TypeModuleCanUnloadNow)
			::GetProcAddress(hMod, ModuleCanUnloadNowName);
		if (!pfnModuleCanUnloadNow)
		{
			::FreeLibrary(hMod);
			return POTTO_E_EXPORTNOTFOUND;
		}

		TypeRegisterModule pfnRegisterModule = (TypeRegisterModule)
			::GetProcAddress(hMod, RegisterModuleName);
		if (!pfnRegisterModule)
		{
			::FreeLibrary(hMod);
			return POTTO_E_EXPORTNOTFOUND;
		}

		// Build Module Entry
		ModuleEntryPtr pNewModule = std::make_shared<ModuleEntry>();
		pNewModule->hMod = (int)hMod;
		pNewModule->pfnModuleGetClassObject = pfnModuleGetClassObject;
		pNewModule->pfnModuleCanUnloadNow = pfnModuleCanUnloadNow;
		pNewModule->pfnRegisterModule = pfnRegisterModule;

		// Save the module into the module entry list
		{
			std::lock_guard<std::mutex> lock(m_mtxForModuleEntryList);
			m_moduleEntryList.push_back(pNewModule);
		}

		// Register Module Entry in lookup map
		POTTO_ERROR error = RegisterModuleEntry(pNewModule);
		if (POTTO_E_OK == error)
			pModule = pNewModule;

		return error;
	}

	POTTO_ERROR Potto::PottoModuleEntryManager::UnloadModule(const ModuleEntryPtr& pModule)
	{
		// If the module is valid and can be unloaded now then unload it
		if (pModule && POTTO_E_OK == pModule->pfnModuleCanUnloadNow())
		{
			if (::FreeLibrary((HMODULE)(pModule->hMod)))
				return POTTO_E_OK;
		}

		// Failed to unload the module
		return POTTO_E_FAIL;
	}

	POTTO_ERROR Potto::PottoModuleEntryManager::GetModuleByClassId(const PottoUuid& id, ModuleEntryPtr& pModule) const
	{
		// Find the module from the lookup map by class id 
		auto it = m_classLookupMap.end();
		{
			std::lock_guard<std::mutex> lock(m_mtxForClassLookupMap);
			it = m_classLookupMap.find(id);
		}

		// If the module was found then return it
		if (it != m_classLookupMap.end())
		{
			auto p = it->second.lock();
			if (p)
			{
				pModule = p;
				return POTTO_E_OK;
			}
		}

		return POTTO_E_CLASSNOTAVAILABLE;
	}

	PottoModuleEntryManager::PottoModuleEntryManager()
	{
	}

	PottoModuleEntryManager::~PottoModuleEntryManager()
	{
		// Clean the lookup map
		{
			std::lock_guard<std::mutex> lock(m_mtxForClassLookupMap);
			m_classLookupMap.clear();
		}

		// Clean all the module entries
		{
			std::lock_guard<std::mutex> lock(m_mtxForModuleEntryList);
			for (auto it = m_moduleEntryList.rbegin(); it != m_moduleEntryList.rend(); it++)
			{
				UnloadModule(*it);
			}
			m_moduleEntryList.clear();
		}
	}

	POTTO_ERROR PottoModuleEntryManager::RegisterModuleEntry(const ModuleEntryPtr& pModule)
	{
		// If it is valid module entry then register it
		if (pModule)
		{
			// Get module id and all the class info in this module
			PottoUuid moduleId;
			ClassInfoList classInfoList;
			POTTO_ERROR error = pModule->pfnRegisterModule(moduleId, classInfoList);

			if (POTTO_E_OK != error)
				return error;
			
			pModule->moduleId = moduleId;

			// Insert item into the lookup table
			for (const ClassInfo& classInfo : classInfoList)
			{
				PottoAssert(m_classLookupMap.find(classInfo.Id) == m_classLookupMap.end(),
					"Duplicated object id were found");

				std::lock_guard<std::mutex> lock(m_mtxForClassLookupMap);
				m_classLookupMap[classInfo.Id] = pModule;
			}

			return POTTO_E_OK;
		}

		return POTTO_E_FAIL;
	}
}
