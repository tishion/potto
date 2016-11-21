#ifndef POTTO_MODULE_ENTRY_NAMANGER_H_
#define POTTO_MODULE_ENTRY_NAMANGER_H_
#pragma once
#include <list>
#include <mutex>
#include <string>
#include <potto/pottoerror.h>
#include <potto/pottouuid.h>
#include <potto/internal/pottodef.h>

namespace Potto
{
	class PottoModuleEntryManager
	{
	public:
		static PottoModuleEntryManager& GetInstance();

		POTTO_ERROR LoadModule(const std::string& moduleName, ModuleEntryPtr& pModule);

		POTTO_ERROR UnloadModule(const ModuleEntryPtr& pModule);

		POTTO_ERROR GetModuleByClassId(const PottoUuid& id, ModuleEntryPtr& pModule) const;

	protected:
		PottoModuleEntryManager();
		
		~PottoModuleEntryManager();

		POTTO_ERROR RegisterModuleEntry(const ModuleEntryPtr& pModule);

	private:
		/// <summary>
		/// 
		/// </summary>
		std::mutex m_mtxForLoadModule;

		/// <summary>
		/// 
		/// </summary>
		typedef std::map<const PottoUuid, ModuleEntryWeakPtr> ClassLookupMap;

		/// <summary>
		/// 
		/// </summary>
		ClassLookupMap m_classLookupMap;
		
		/// <summary>
		/// 
		/// </summary>
		mutable std::mutex m_mtxForClassLookupMap;

		/// <summary>
		/// 
		/// </summary>
		typedef std::list<ModuleEntryPtr> ModuleEntryList;

		/// <summary>
		/// 
		/// </summary>
		ModuleEntryList m_moduleEntryList;

		/// <summary>
		/// 
		/// </summary>
		std::mutex m_mtxForModuleEntryList;
	};
}

#endif // POTTO_MODULE_ENTRY_NAMANGER_H_
