#ifndef POTTO_MODULE_POOL_NAMANGER_H_
#define POTTO_MODULE_POOL_NAMANGER_H_
#pragma once
#include <map>
#include <mutex>
#include <string>
#include <potto/pottouuid.hpp>

namespace Potto
{
	/// <summary>
	/// The management class of module path pool.
	/// </summary>
	class PottoModulePoolManager
	{
	public:
		/// <summary>
		/// Gets the instance of the class.
		/// </summary>
		/// <returns>The reference to the singleton instance.</returns>
		static PottoModulePoolManager& GetInstance();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="libraryPath"></param>
		/// <param name="moduleRootPath"></param>
		/// <returns></returns>
		bool InitFromLibraryFile(const std::string& libraryPath, const std::string& moduleRootPath);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		const std::string GetModulePathByClassId(const PottoUuid& id) const;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		PottoModulePoolManager();

		/// <summary>
		/// Destructor.
		/// </summary>
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

