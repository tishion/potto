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
		/// Initializes the module path library from file.
		/// </summary>
		/// <param name="libraryPath">The library file path.</param>
		/// <param name="moduleRootPath">The module root path.</param>
		/// <returns>True if successful; otherwise false.</returns>
		bool InitFromLibraryFile(const std::string& libraryPath, const std::string& moduleRootPath);

		/// <summary>
		/// Gets the module path by the class id.
		/// </summary>
		/// <param name="id">The class id.</param>
		/// <returns>The module path.</returns>
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
		/// The module root path.
		/// </summary>
		std::string m_moduleRootPath;

		/// <summary>
		/// The module path lookup map.
		/// </summary>
		typedef std::map<const PottoUuid, const std::string&> ModulePathLookupMap;

		/// <summary>
		/// The module path lookup map.
		/// </summary>
		ModulePathLookupMap m_modulePathLookupMap;

		/// <summary>
		/// The mutex for module path lookup map.
		/// </summary>
		mutable std::mutex m_mtxFormodulePathLookupMap;

		/// <summary>
		/// The module path list.
		/// </summary>
		typedef std::vector<std::string> PathList;

		/// <summary>
		/// The module path list.
		/// </summary>
		PathList m_modulePathList;
	};
}

#endif // POTTO_MODULE_POOL_NAMANGER_H_

