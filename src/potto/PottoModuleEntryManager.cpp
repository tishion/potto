#include <windows.h>
#include "PottoModuleEntryManager.h"

namespace Potto
{
	PottoModuleEntryManager& PottoModuleEntryManager::GetInstance()
	{
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

		{
			std::lock_guard<std::mutex> lock(m_mtxForModuleEntryList);
			m_moduleEntryList.push_back(pNewModule);
		}

		// Register Module Entry
		POTTO_ERROR error = RegisterModuleEntry(pNewModule);
		if (POTTO_E_OK == error)
			pModule = pNewModule;

		return error;
	}

	POTTO_ERROR Potto::PottoModuleEntryManager::UnloadModule(const ModuleEntryPtr& pModule)
	{
		if (pModule && POTTO_E_OK == pModule->pfnModuleCanUnloadNow())
		{
			if (::FreeLibrary((HMODULE)(pModule->hMod)))
				return POTTO_E_OK;
		}
		return POTTO_E_FAIL;
	}

	POTTO_ERROR Potto::PottoModuleEntryManager::GetModuleByClassId(const PottoUuid& id, ModuleEntryPtr& pModule) const
	{
		auto it = m_classLookupMap.end();
		{
			std::lock_guard<std::mutex> lock(m_mtxForClassLookupMap);
			it = m_classLookupMap.find(id);
		}

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
		{
			std::lock_guard<std::mutex> lock(m_mtxForClassLookupMap);
			m_classLookupMap.clear();
		}

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
		if (pModule)
		{
			PottoUuid moduleId;
			ClassInfoList classInfoList;
			POTTO_ERROR error = pModule->pfnRegisterModule(moduleId, classInfoList);

			if (POTTO_E_OK != error)
				return error;
			
			pModule->moduleId = moduleId;

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
