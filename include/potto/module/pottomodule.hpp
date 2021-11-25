#ifndef POTTOMODULE_H_
#define POTTOMODULE_H_
#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#endif

#include <assert.h>
#include <mutex>

#include <potto/module/pottodef.hpp>
#include <potto/pottoerror.hpp>
#include <potto/pottoptr.hpp>
#include <potto/pottouuid.hpp>

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto {
/// <summary>
/// The potto module base class.
/// </summary>
class PottoModuleRootBase {
public:
  /// <summary>
  ///
  /// </summary>
  PottoModuleRootBase(const PottoUuid& id)
      : m_id(id)
      , m_nLockCount(0) {
    m_classEntryMap.clear();
  }

  /// <summary>
  ///
  /// </summary>
  ~PottoModuleRootBase() {
    ReleaseObjects();
  }

  /// <summary>
  ///
  /// </summary>
  unsigned long Lock() {
    return ++m_nLockCount;
  }

  /// <summary>
  ///
  /// </summary>
  unsigned long Unlock() {
    return --m_nLockCount;
  }

  /// <summary>
  ///
  /// </summary>
  unsigned long GetLockCount() const {
    return m_nLockCount;
  }

  /// <summary>
  ///
  /// </summary>
  void ReleaseObjects() {
    for (auto it = m_classEntryMap.begin(); it != m_classEntryMap.end(); it++) {
      ClassEntry* pEntry = it->second;
      if (pEntry && pEntry->pCachedIUnknown) {
        pEntry->pCachedIUnknown->Release();
        pEntry->pCachedIUnknown = nullptr;
        delete pEntry;
      }
    }
    m_classEntryMap.clear();
  }

  /// <summary>
  ///
  /// </summary>
  const PottoUuid GetId() const {
    return m_id;
  }

  /// <summary>
  ///
  /// </summary>
  std::mutex& GetModuleMutex() {
    return m_mtxMoudle;
  }

  /// <summary>
  ///
  /// </summary>
  ClassEntryMap& GetClassEntryMap() {
    return m_classEntryMap;
  }

private:
  /// <summary>
  ///
  /// </summary>
  PottoUuid m_id;

  /// <summary>
  ///
  /// </summary>
  ThreadSafeReferenceCount m_nLockCount;

  /// <summary>
  ///
  /// </summary>
  std::mutex m_mtxMoudle;

  /// <summary>
  ///
  /// </summary>
  ClassEntryMap m_classEntryMap;
};

/// <summary>
/// The potto module.
/// </summary>
class PottoModule : public PottoModuleRootBase {
public:
  PottoModule(const PottoUuid& id)
      : PottoModuleRootBase(id){};
  static PottoModule& GetInstance();
};
} // namespace Potto

/// <summary>
/// Definition of Potto::PottoModule::GetInstance.
/// </summary>
#define POTTO_MODULE(id)                                                                           \
  Potto::PottoModule& Potto::PottoModule::GetInstance() {                                          \
    static Potto::PottoModule s_instance(id);                                                      \
    return s_instance;                                                                             \
  }

#endif // POTTOMODULE_H_