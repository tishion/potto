#ifndef POTTO_MODULE_ENTRY_NAMANGER_H_
#define POTTO_MODULE_ENTRY_NAMANGER_H_
#pragma once
#include <list>
#include <mutex>
#include <string>
#include <potto/pottoerror.hpp>
#include <potto/pottouuid.hpp>
#include <potto/internal/pottodef.hpp>

namespace Potto
{
	/// <summary>
	/// The management class of module entry.
	/// </summary>
	class PottoModuleEntryManager
	{
	public:
		/// <summary>
		/// Gets the instance of the class.
		/// </summary>
		/// <returns>The reference to the singleton instance.</returns>
		static PottoModuleEntryManager& GetInstance();

		/// <summary>
		/// Loads the module and creates the module entry.
		/// </summary>
		/// <param name="moduleName">The path to the module.</param>
		/// <param name="pModule">The module entry.</param>
		/// <returns>POTTO_E_OK if successful.</returns>
		POTTO_ERROR LoadModule(const std::string& moduleName, ModuleEntryPtr& pModule);

		/// <summary>
		/// Unloads the  module and removes the module entry..
		/// </summary>
		/// <param name="pModule">The module entry.</param>
		/// <returns>POTTO_E_OK if successful.</returns>
		POTTO_ERROR UnloadModule(const ModuleEntryPtr& pModule);

		/// <summary>
		/// Gets the module entry by class id.
		/// </summary>
		/// <param name="id">The class id.</param>
		/// <param name="pModule">The module entry containing the class.</param>
		/// <returns>POTTO_E_OK if successful.</returns>
		POTTO_ERROR GetModuleByClassId(const PottoUuid& id, ModuleEntryPtr& pModule) const;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		PottoModuleEntryManager();
		
		/// <summary>
		/// Destructor.
		/// </summary>
		~PottoModuleEntryManager();

		/// <summary>
		/// Registers the module entry.
		/// </summary>
		/// <param name="pModule">The module entry.</param>
		/// <returns>POTTO_E_OK if successful.</returns>
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
