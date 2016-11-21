#ifndef POTTO_MODULE_POOL_NAMANGER_H_
#define POTTO_MODULE_POOL_NAMANGER_H_
#pragma once
#include <map>
#include <mutex>
#include <string>
#include <potto/pottouuid.h>

namespace Potto
{

	class PottoModulePoolManager
	{
	public:
		static PottoModulePoolManager& GetInstance();

		bool InitFromLibraryFile(const std::string& libraryPath, const std::string& moduleRootPath);

		const std::string GetModulePathByClassId(const PottoUuid& id) const;

	protected:
		PottoModulePoolManager();

		~PottoModulePoolManager();

	private:
		/// <summary>
		/// 
		/// </summary>
		std::string m_moduleRootPath;

		/// <summary>
		/// 
		/// </summary>
		typedef std::map<const PottoUuid, const std::string&> ModulePathLookupMap;

		/// <summary>
		/// 
		/// </summary>
		ModulePathLookupMap m_modulePathLookupMap;

		/// <summary>
		/// 
		/// </summary>
		mutable std::mutex m_mtxFormodulePathLookupMap;

		/// <summary>
		/// 
		/// </summary>
		typedef std::vector<std::string> PathList;

		/// <summary>
		/// 
		/// </summary>
		PathList m_modulePathList;
	};
}

#endif // POTTO_MODULE_POOL_NAMANGER_H_

