#ifndef POTTODEF_H_
#define POTTODEF_H_
#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <potto/pottoerror.h>
#include <potto/pottouuid.h>
#include <potto/pottointerface.h>

namespace Potto
{
	/// <summary>
	/// The non-thread-safe reference count.
	/// </summary>
	typedef unsigned long ReferenceCount;

	/// <summary>
	/// The reference thread-safe count.
	/// </summary>
	typedef std::atomic<ReferenceCount> ThreadSafeReferenceCount;

	/// <summary>
	/// The function type of creator.
	/// </summary>
	typedef POTTO_ERROR(*TypeObjectCreatorFunction)(const PottoUuid&, void** ppv);

	/// <summary>
	/// The interface entry.
	/// </summary>
	class InterfaceEntry
	{
	public:
		/// <summary>
		/// The id.
		/// </summary>
		const PottoUuid iid;
		
		/// <summary>
		/// The offset of This pointer.
		/// </summary>
		const std::ptrdiff_t offset;
	};

	/// <summary>
	/// Class entry.
	/// </summary>
	class ClassEntry
	{
	public:
		/// <summary>
		/// The class id.
		/// </summary>
		const PottoUuid classId;

		/// <summary>
		/// The class factory creator.
		/// </summary>
		const TypeObjectCreatorFunction pfnClassFactoryCreator;

		/// <summary>
		/// The object creator.
		/// </summary>
		const TypeObjectCreatorFunction pfnObjectCreator;

		/// <summary>
		/// The cached IUnknown pointer.
		/// </summary>
		IUnknown* pCachedIUnknown;

		/// <summary>
		/// The class Name.
		/// </summary>
		const std::string className;
	};

	/// <summary>
	/// The class info.
	/// </summary>
	class ClassInfo
	{
	public:
		/// <summary>
		/// The class id.
		/// </summary>
		const PottoUuid		Id;

		/// <summary>
		/// The class name.
		/// </summary>
		const std::string	Name;
	};

	/// <summary>
	/// The class info list.
	/// </summary>
	typedef std::vector<ClassInfo> ClassInfoList;

	/// <summary>
	/// The function type of ModuleCanUnloadNow
	/// </summary>
	typedef POTTO_ERROR(*TypeModuleCanUnloadNow)();

	/// <summary>
	/// The function type of ModuleGetClassObject
	/// </summary>
	typedef POTTO_ERROR(*TypeModuleGetClassObject)(const PottoUuid&, const PottoUuid&, void**);

	/// <summary>
	/// The function type of RegisterModule
	/// </summary>
	typedef POTTO_ERROR(*TypeRegisterModule)(PottoUuid&, ClassInfoList&);

	/// <summary>
	/// The class entry map.
	/// </summary>
	typedef std::map<const PottoUuid, ClassEntry*> ClassEntryMap;

	/// <summary>
	/// The module entry.
	/// </summary>
	class ModuleEntry
	{
	public:
		/// <summary>
		/// The module handle.
		/// </summary>
		int							hMod;

		/// <summary>
		/// The module id.
		/// </summary>
		PottoUuid					moduleId;

		/// <summary>
		/// The function pointer of ModuleGetClassObject.
		/// </summary>
		TypeModuleGetClassObject	pfnModuleGetClassObject;

		/// <summary>
		/// The function pointer of ModuleCanUnloadNow.
		/// </summary>
		TypeModuleCanUnloadNow		pfnModuleCanUnloadNow;
		
		/// <summary>
		/// The function pointer of RegisterModule.
		/// </summary>
		TypeRegisterModule			pfnRegisterModule;
	};

	/// <summary>
	/// The smart pointer of module entry.
	/// </summary>
	typedef std::shared_ptr<ModuleEntry> ModuleEntryPtr;

	/// <summary>
	/// The weak pointer of module entry.
	/// </summary>
	typedef std::weak_ptr<ModuleEntry> ModuleEntryWeakPtr;
}

/// <summary>
/// Name and forward declaration of ModuleCanUnloadNow.
/// </summary>
#define ModuleCanUnloadNowName "ModuleCanUnloadNow"
POTTO_ERROR ModuleCanUnloadNow();

/// <summary>
/// Name and forward declaration of ModuleGetClassObject.
/// </summary>
#define ModuleGetClassObjectName "ModuleGetClassObject"
POTTO_ERROR ModuleGetClassObject(const Potto::PottoUuid& clsid, const Potto::PottoUuid& iid, void** ppv);

/// <summary>
/// Name and forward declaration of RegisterModule.
/// </summary>
#define RegisterModuleName "RegisterModule"
POTTO_ERROR RegisterModule(Potto::PottoUuid& moduleId, Potto::ClassInfoList& classInfoList);

/// <summary>
/// Potto assert.
/// </summary>
#define PottoAssert(expr, msg) assert((expr) && (msg))

#endif // POTTODEF_H_