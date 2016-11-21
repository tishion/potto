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
	/// 
	/// </summary>
	typedef unsigned long ReferenceCount;

	/// <summary>
	/// 
	/// </summary>
	typedef std::atomic<ReferenceCount> ThreadSafeReferenceCount;

	/// <summary>
	/// 
	/// </summary>
	typedef POTTO_ERROR(*TypeObjectCreatorFunction)(const PottoUuid&, void** ppv);

	/// <summary>
	/// 
	/// </summary>
	class InterfaceEntry
	{
	public:
		const PottoUuid iid;
		const std::ptrdiff_t offset;
	};

	/// <summary>
	/// 
	/// </summary>
	class ClassEntry
	{
	public:
		const PottoUuid classId;
		const TypeObjectCreatorFunction pfnClassFactoryCreator;
		const TypeObjectCreatorFunction pfnObjectCreator;
		IUnknown* pCachedIUnknown;
		const std::string clsName;
	};

	/// <summary>
	/// 
	/// </summary>
	class ClassInfo
	{
	public:
		const PottoUuid		Id;
		const std::string	Name;
	};

	/// <summary>
	/// 
	/// </summary>
	typedef std::vector<ClassInfo> ClassInfoList;

	/// <summary>
	/// 
	/// </summary>
	typedef POTTO_ERROR(*TypeModuleCanUnloadNow)();

	/// <summary>
	/// 
	/// </summary>
	typedef POTTO_ERROR(*TypeModuleGetClassObject)(const PottoUuid&, const PottoUuid&, void**);

	/// <summary>
	/// 
	/// </summary>
	typedef POTTO_ERROR(*TypeRegisterModule)(PottoUuid&, ClassInfoList&);

	/// <summary>
	/// 
	/// </summary>
	typedef std::map<const PottoUuid, ClassEntry*> ClassEntryMap;

	/// <summary>
	/// 
	/// </summary>
	class ModuleEntry
	{
	public:
		int							hMod;
		PottoUuid					moduleId;
		TypeModuleGetClassObject	pfnModuleGetClassObject;
		TypeModuleCanUnloadNow		pfnModuleCanUnloadNow;
		TypeRegisterModule			pfnRegisterModule;
	};

	/// <summary>
	/// 
	/// </summary>
	typedef std::shared_ptr<ModuleEntry> ModuleEntryPtr;

	/// <summary>
	/// 
	/// </summary>
	typedef std::weak_ptr<ModuleEntry> ModuleEntryWeakPtr;
}

#define ModuleCanUnloadNowName "ModuleCanUnloadNow"
POTTO_ERROR ModuleCanUnloadNow();

#define ModuleGetClassObjectName "ModuleGetClassObject"
POTTO_ERROR ModuleGetClassObject(const Potto::PottoUuid& clsid, const Potto::PottoUuid& iid, void** ppv);

#define RegisterModuleName "RegisterModule"
POTTO_ERROR RegisterModule(Potto::PottoUuid& moduleId, Potto::ClassInfoList& classInfoList);

#define PottoAssert(expr, msg) assert((expr) && (msg))

#endif // POTTODEF_H_