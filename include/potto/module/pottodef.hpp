#ifndef POTTODEF_H_
#define POTTODEF_H_
#pragma once

#include <atomic>
#include <map>
#include <memory>

#include <potto/pottoerror.hpp>
#include <potto/pottointerface.hpp>
#include <potto/pottouuid.hpp>

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto {
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
/// <param name="id">The object id.</param>
/// <param name="ppv">The pointer.</param>
/// <returns>POTTO_E_OK if successful.</returns>
typedef POTTO_ERROR (*TypeObjectCreatorFunction)(const PottoUuid& id, void** ppv);

/// <summary>
/// The interface entry.
/// </summary>
class InterfaceEntry {
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
/// The class entry.
/// </summary>
class ClassEntry {
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
class ClassInfo {
public:
  /// <summary>
  /// The class id.
  /// </summary>
  const PottoUuid Id;

  /// <summary>
  /// The class name.
  /// </summary>
  const std::string Name;
};

/// <summary>
/// The class info list.
/// </summary>
typedef std::vector<ClassInfo> ClassInfoList;

/// <summary>
/// The function type of ModuleCanUnloadNow
/// </summary>
/// <returns>POTTO_E_OK if successful.</returns>
typedef POTTO_ERROR (*TypeModuleCanUnloadNow)();

/// <summary>
/// The function type of ModuleGetClassObject
/// </summary>
/// <param name="clsid">The class id.</param>
/// <param name="iid">The interface id.</param>
/// <param name="ppv">The pointer.</param>
/// <returns>POTTO_E_OK if successful.</returns>
typedef POTTO_ERROR (*TypeModuleGetClassObject)(const PottoUuid& clsid, const PottoUuid& iid,
                                                void** ppv);

/// <summary>
/// The function type of RegisterModule
/// </summary>
/// <param name="id">The module id.</param>
/// <param name="classInfoList">The class info list.</param>
typedef POTTO_ERROR (*TypeRegisterModule)(PottoUuid& id, ClassInfoList& classInfoList);

/// <summary>
/// The class entry map.
/// </summary>
typedef std::map<const PottoUuid, ClassEntry*> ClassEntryMap;

/// <summary>
/// The module entry.
/// </summary>
class ModuleEntry {
public:
  /// <summary>
  /// The module handle.
  /// </summary>
  void* hMod;

  /// <summary>
  /// The module id.
  /// </summary>
  PottoUuid moduleId;

  /// <summary>
  /// The function pointer of ModuleGetClassObject.
  /// </summary>
  TypeModuleGetClassObject pfnModuleGetClassObject;

  /// <summary>
  /// The function pointer of ModuleCanUnloadNow.
  /// </summary>
  TypeModuleCanUnloadNow pfnModuleCanUnloadNow;

  /// <summary>
  /// The function pointer of RegisterModule.
  /// </summary>
  TypeRegisterModule pfnRegisterModule;
};

/// <summary>
/// The smart pointer of module entry.
/// </summary>
typedef std::shared_ptr<ModuleEntry> ModuleEntryPtr;

/// <summary>
/// The weak pointer of module entry.
/// </summary>
typedef std::weak_ptr<ModuleEntry> ModuleEntryWeakPtr;
} // namespace Potto

#if defined(_MSC_VER)
// Microsoft
#define EXPORT_C_SYMBOL extern "C" __declspec(dllexport)
#elif defined(__GNUC__)
// GCC
#define EXPORT_C_SYMBOL extern "C" __attribute__((visibility("default")))
#else
#define EXPORT_C_SYMBOL
#pragma warning Unknown dynamic link import / export semantics.
#endif

/// <summary>
/// Name and forward declaration of ModuleCanUnloadNow.
/// </summary>
#define ModuleCanUnloadNowName "ModuleCanUnloadNow"
EXPORT_C_SYMBOL
Potto::POTTO_ERROR ModuleCanUnloadNow();

/// <summary>2
/// Name and forward declaration of ModuleGetClassObject.
/// </summary>
#define ModuleGetClassObjectName "ModuleGetClassObject"
EXPORT_C_SYMBOL
Potto::POTTO_ERROR ModuleGetClassObject(const Potto::PottoUuid& clsid, const Potto::PottoUuid& iid,
                                        void** ppv);

/// <summary>
/// Name and forward declaration of RegisterModule.
/// </summary>
#define RegisterModuleName "RegisterModule"
EXPORT_C_SYMBOL Potto::POTTO_ERROR RegisterModule(Potto::PottoUuid& moduleId,
                                                  Potto::ClassInfoList& classInfoList);

/// <summary>
/// Potto assert.
/// </summary>
#define PottoAssert(expr, msg) assert((expr) && (msg))

#endif // POTTODEF_H_
