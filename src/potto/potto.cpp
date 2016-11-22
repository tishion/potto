#include <potto/pottoptr.h>
#include <potto/internal/pottoobject.h>
#include "include/potto.h"
#include "PottoModulePoolManager.h"
#include "PottoModuleEntryManager.h"

namespace Potto
{

	POTTO_ERROR Initialize(const std::string& moduleDatabase, const std::string& moduleRootPath)
	{
		if (PottoModulePoolManager::GetInstance().InitFromLibraryFile(moduleDatabase, moduleRootPath))
			return POTTO_E_OK;
		return POTTO_E_FAIL;
	}

	POTTO_ERROR CreateInstance(const PottoUuid&  classId, const PottoUuid&  interfaceId, void** ppv, const std::string& moduleName)
	{
		PottoPtr<IClassFactory> pClassFactory;
		POTTO_ERROR error = GetClassObject(classId, IIDOF(IClassFactory), (void**)&pClassFactory, moduleName);
		if (POTTO_E_OK != error || !pClassFactory)
			return error;

		return pClassFactory->CreateInstance(interfaceId, ppv);
	}

	POTTO_ERROR GetClassObject(const PottoUuid&  classId, const PottoUuid&  interfaceId, void** ppv, const std::string& moduleName)
	{
		if (!ppv)
			return POTTO_E_INVALIDARG;

		*ppv = NULL;

		POTTO_ERROR error = POTTO_E_MODULENOTFOUND;
		ModuleEntryPtr pModule;

		// If module name is not empty then try to get object from the module
		if (moduleName.empty())
		{
			// Find module entry
			error = PottoModuleEntryManager::GetInstance().GetModuleByClassId(classId, pModule);

			// If the module entry was not found in the then find the module path in the module pool
			if (POTTO_E_OK != error)
			{
				const std::string modulePath = PottoModulePoolManager::GetInstance().GetModulePathByClassId(classId);
				if (modulePath.empty())
					return error;

				error = PottoModuleEntryManager::GetInstance().LoadModule(modulePath, pModule);
			}
		}
		else
		{
			// Find module by name from the module pool
			error = PottoModuleEntryManager::GetInstance().LoadModule(moduleName, pModule);
		}

		if (POTTO_E_OK != error)
			return error;

		return pModule->pfnModuleGetClassObject(classId, interfaceId, ppv);
	}
}