#ifndef POTTOOBJECT_H_
#define POTTOOBJECT_H_
#pragma once

#include <assert.h>
#include <atomic>
#include <cstddef>
#include <limits>
#include <mutex>

#include <potto/module/pottodef.hpp>
#include <potto/module/pottomodule.hpp>
#include <potto/pottoerror.hpp>
#include <potto/pottointerface.hpp>
#include <potto/pottoptr.hpp>

/// <summary>
/// The invalid offset of This pointer of interface.
/// </summary>
#define INTERFACE_INVALID_OFFSET (std::ptrdiff_t)(-1)

/// <summary>
/// Destroyed reference count.
/// </summary>
#define PROTECT_REFCOUNT (0x0FFFFFFF / 2)

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto {
/// <summary>
///
/// </summary>
/// <param name="pThis"></param>
/// <param name="pEntries"></param>
/// <param name="riid"></param>
/// <param name="ppvObject"></param>
/// <returns></returns>
static POTTO_ERROR PottoInternalQueryInterface(void* pThis, const InterfaceEntry* pEntries,
                                               const PottoUuid& riid, void** ppvObject) {
  PottoAssert(pThis != nullptr, "This pointer is null.");
  PottoAssert(pEntries != nullptr, "Interface entry list is null.");

  if (pThis == nullptr || pEntries == nullptr)
    return POTTO_E_INVALIDARG;

  if (ppvObject == nullptr)
    return POTTO_E_POINTER;

  if (IsEqualUuid(IID_IUnknown, riid)) {
    IUnknown* pUnk = (IUnknown*)((std::intptr_t)pThis + pEntries->offset);
    pUnk->AddRef();
    *ppvObject = pUnk;
    return POTTO_E_OK;
  }

  POTTO_ERROR error;

  for (;; pEntries++) {
    if (INTERFACE_INVALID_OFFSET == pEntries->offset) {
      error = POTTO_E_NOINTERFACE;
      break;
    }

    if (IsEqualUuid(pEntries->iid, riid)) {
      IUnknown* pUnk = (IUnknown*)((std::intptr_t)pThis + pEntries->offset);
      pUnk->AddRef();
      *ppvObject = pUnk;
      return POTTO_E_OK;
    }
  }

  *ppvObject = nullptr;

  return error;
}

/// <summary>
/// The potto object root base class.
/// </summary>
template <class RefCountType = ReferenceCount>
class PottoObjectRootBase {
public:
  /// <summary>
  ///
  /// </summary>
  PottoObjectRootBase()
      : m_nRefCnt(0) {
  }

  /// <summary>
  ///
  /// </summary>
  ~PottoObjectRootBase() {
  }

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  unsigned long InternalAddRef() {
    return ++m_nRefCnt;
  }

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  unsigned long InternalRelease() {
    return --m_nRefCnt;
  }

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  POTTO_ERROR FinalConstruct() {
    return POTTO_E_OK;
  }

  /// <summary>
  ///
  /// </summary>
  void FinalRelease() {
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="pThis"></param>
  /// <param name="pEntries"></param>
  /// <param name="riid"></param>
  /// <param name="ppvObject"></param>
  /// <returns></returns>
  static POTTO_ERROR InternalQueryInterface(void* pThis, const InterfaceEntry* pEntries,
                                            const PottoUuid& riid, void** ppvObject) {
    return PottoInternalQueryInterface(pThis, pEntries, riid, ppvObject);
  }

  /// <summary>
  ///
  /// </summary>
  void InternalFinalConstructAddRef() {
    InternalAddRef();
  }

  /// <summary>
  ///
  /// </summary>
  void InternalFinalConstructRelease() {
    InternalRelease();
    PottoAssert(m_nRefCnt == 0, "Reference count is not zero");
  }

  /// <summary>
  ///
  /// </summary>
  void Lock() {
    m_mtxLock.lock();
  }

  /// <summary>
  ///
  /// </summary>
  void Unlock() {
    m_mtxLock.unlock();
  }

public:
  /// <summary>
  ///
  /// </summary>
  RefCountType m_nRefCnt;

  /// <summary>
  ///
  /// </summary>
  std::mutex m_mtxLock;
};

/// <summary>
/// The root base class with multi threaded mode.
/// </summary>
typedef PottoObjectRootBase<ThreadSafeReferenceCount> PottoThreadSafeObjectRootBase;

///////////////////////////////////////////////////////////////////////

/// <summary>
/// The user potto com object class.
/// </summary>
template <class Base>
class PottoObject : public Base {
public:
  /// <summary>
  ///
  /// </summary>
  typedef Base _BaseClass;

  /// <summary>
  ///
  /// </summary>
  PottoObject() {
    PottoModule::GetInstance().Lock();
  }

  /// <summary>
  ///
  /// </summary>
  virtual ~PottoObject() {
    _BaseClass::m_nRefCnt = PROTECT_REFCOUNT;
    _BaseClass::FinalRelease();
    PottoModule::GetInstance().Unlock();
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="p"></param>
  virtual unsigned long AddRef() {
    return _BaseClass::InternalAddRef();
  }

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  virtual unsigned long Release() {
    unsigned long l = _BaseClass::InternalRelease();
    if (l == 0) {
      // Lock the module to avoid DLL unload when destruction of member variables take a long time
      PottoModule::GetInstance().Lock();
      delete this;
      PottoModule::GetInstance().Unlock();
    }
    return l;
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="riid"></param>
  /// <param name="pp"></param>
  /// <returns></returns>
  virtual POTTO_ERROR QueryInterface(const PottoUuid& riid, void** pp) {
    return _BaseClass::_InternalQueryInterface(riid, pp);
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="pp"></param>
  /// <returns></returns>
  template <class Q>
  POTTO_ERROR QueryInterface(Q** pp) {
    return QueryInterface(Q::IID(), (void**)pp);
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="pp"></param>
  /// <returns></returns>
  static POTTO_ERROR CreateInstance(PottoObject<Base>** pp);
};

template <class Base>
POTTO_ERROR PottoObject<Base>::CreateInstance(PottoObject<Base>** pp) {
  PottoAssert(pp != 0, "");
  if (pp == 0) {
    return POTTO_E_POINTER;
  }

  *pp = 0;

  POTTO_ERROR error = POTTO_E_OUTOFMEMORY;
  PottoObject<Base>* p = nullptr;

  p = new PottoObject<Base>();
  if (p != nullptr) {
    p->InternalFinalConstructAddRef();

    if (POTTO_E_OK == error)
      error = p->FinalConstruct();

    p->InternalFinalConstructRelease();

    if (POTTO_E_OK == error)
      error = p->QueryInterface(IID_IUnknown, pp);

    if (error != POTTO_E_OK) {
      delete p;
      p = nullptr;
    }
  }
  *pp = p;
  return error;
}

/// <summary>
/// The cached potto object.
/// </summary>
template <class Base>
class PottoCachedObject : public Base {
public:
  /// <summary>
  ///
  /// </summary>
  typedef Base _BaseClass;

  /// <summary>
  ///
  /// </summary>
  PottoCachedObject() {
  }

  /// <summary>
  ///
  /// </summary>
  virtual ~PottoCachedObject() {
    _BaseClass::m_nRefCnt = PROTECT_REFCOUNT;
    _BaseClass::FinalRelease();
  }

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  virtual unsigned long AddRef() {
    unsigned long l = _BaseClass::InternalAddRef();
    if (l == 2)
      PottoModule::GetInstance().Lock();
    return l;
  }

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  virtual unsigned long Release() {
    unsigned long l = _BaseClass::InternalRelease();
    if (l == 0)
      delete this;
    else if (l == 1)
      PottoModule::GetInstance().Unlock();
    return l;
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="riid"></param>
  /// <param name="pp"></param>
  /// <returns></returns>
  virtual POTTO_ERROR QueryInterface(const PottoUuid& riid, void** pp) {
    return _BaseClass::_InternalQueryInterface(riid, pp);
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="pp"></param>
  /// <returns></returns>
  static POTTO_ERROR CreateInstance(PottoCachedObject<Base>** pp);
};

template <class Base>
POTTO_ERROR PottoCachedObject<Base>::CreateInstance(PottoCachedObject<Base>** pp) {
  PottoAssert(pp != 0, "");
  if (pp == 0)
    return POTTO_E_POINTER;
  *pp = 0;

  POTTO_ERROR error = POTTO_E_OUTOFMEMORY;
  PottoCachedObject<Base>* p = nullptr;

  p = new PottoCachedObject<Base>();
  if (p != nullptr) {
    p->InternalFinalConstructAddRef();

    error = p->FinalConstruct();

    p->InternalFinalConstructRelease();

    if (POTTO_E_OK == error)
      error = p->QueryInterface(IID_IUnknown, pp);

    if (error != POTTO_E_OK) {
      delete p;
      p = nullptr;
    }
  }
  *pp = p;
  return error;
}

///////////////////////////////////////////////////////////////////////

/// <summary>
/// The potto object creator.
/// </summary>
template <class T>
class PottoObjectCreator {
public:
  /// <summary>
  ///
  /// </summary>
  /// <param name="riid"></param>
  /// <param name="ppv"></param>
  /// <returns></returns>
  static POTTO_ERROR CreateInstance(const PottoUuid& riid, void** ppv) {
    if (ppv == nullptr)
      return POTTO_E_POINTER;

    *ppv = nullptr;

    POTTO_ERROR error = POTTO_E_OUTOFMEMORY;
    T* p = nullptr;

    p = new T();

    if (p != nullptr) {
      p->InternalFinalConstructAddRef();

      error = p->FinalConstruct();

      p->InternalFinalConstructRelease();

      if (POTTO_E_OK == error)
        error = p->QueryInterface(riid, ppv);

      if (error != POTTO_E_OK)
        delete p;
    }
    return error;
  }
};

/// <summary>
/// The potto class register.
/// </summary>
template <class T>
class PottoClassRegister {
public:
  /// <summary>
  ///
  /// </summary>
  /// <param name="name"></param>
  /// <param name="id"></param>
  PottoClassRegister(const std::string& name, const PottoUuid& id) {
    ClassEntry* pEntry = new ClassEntry{id, T::_ClassFactoryCreatorClass::CreateInstance,
                                        T::_CreatorClass::CreateInstance, (IUnknown*)0, name};
    PottoModule::GetInstance().GetClassEntryMap()[pEntry->classId] = pEntry;
  }
};
} // namespace Potto

/// <summary>
/// The begin of a single threaded potto object.
/// </summary>
#define POTTO_SINGLEHREADED_OBJECT(clsid, name)                                                    \
  const char* const CLSID_##name = clsid;                                                          \
  POTTO_DECLARE_NOVTABLE name                                                                      \
      : public Potto::PottoObjectRootBase                                                          \
      , public Potto::PottoObjectClass<name>

/// <summary>
/// The begin of a multiple threaded potto object.
/// </summary>
#define POTTO_MULTITHREADED_OBJECT(clsid, name)                                                    \
  const char* const CLSID_##name = clsid;                                                          \
  class POTTO_DECLARE_NOVTABLE name                                                                \
      : public Potto::PottoThreadSafeObjectRootBase                                                \
      , public Potto::PottoObjectClass<name>

/// <summary>
/// Implements an interface.
/// </summary>
#define POTTO_IMPLEMENT_INTERFACE(name) , public name

/// <summary>
/// Declares the class factory.
/// </summary>
#define DECLARE_CLASSFACTORY                                                                       \
  typedef Potto::PottoObjectCreator<Potto::PottoCachedObject<Potto::PottoClassFactory>>            \
      _ClassFactoryCreatorClass;

/// <summary>
/// Declares the singleton class factory.
/// </summary>
#define DECLARE_SINGLETON_CLASSFACTORY(name)                                                       \
  typedef Potto::PottoObjectCreator<                                                               \
      Potto::PottoCachedObject<Potto::PottoClassSingletonFactory<name>>>                           \
      _ClassFactoryCreatorClass;

/// <summary>
/// The begin of the interface entry map.
/// </summary>
#define POTTO_BEGIN_INTERFACE_MAP(c)                                                               \
  const static PottoUuid& CLSID() {                                                                \
    static PottoUuid clsid(CLSID_##c);                                                             \
    return clsid;                                                                                  \
  };                                                                                               \
  POTTO_ERROR _InternalQueryInterface(const PottoUuid& iid, void** ppvObject) {                    \
    return InternalQueryInterface(this, _GetEntries(), iid, ppvObject);                            \
  }                                                                                                \
  const static InterfaceEntry* _GetEntries() {                                                     \
    typedef c _PottoObjectType;                                                                    \
    static const InterfaceEntry _entries[] = {

/// <summary>
/// Gets the offset of This pointer.
/// </summary>
#define POTTO_OFFSET_OF(b, i) ((ptrdiff_t)(static_cast<b*>((i*)0x08)) - 0x08)

/// <summary>
/// Define a potto interface entry.
/// </summary>
#define POTTO_INTERFACE_ENTRY(i) {IID_##i, POTTO_OFFSET_OF(i, _PottoObjectType)},

/// <summary>
/// The end of the interface entry map.
/// </summary>
#define POTTO_END_INTERFACE_MAP()                                                                  \
  { PottoUuid(), INTERFACE_INVALID_OFFSET }                                                        \
  }                                                                                                \
  ;                                                                                                \
  return _entries;                                                                                 \
  } /*                                                                                             \
virtual unsigned long AddRef() = 0;                                                                \
virtual unsigned long Release() = 0;                                                               \
virtual POTTO_ERROR QueryInterface(const PottoUuid& interfaceId, void** pp) = 0;*/

/// <summary>
/// Define a potto class register.
/// </summary>
#define POTTO_CLASS_REGISTER(name)                                                                 \
  static Potto::PottoClassRegister<name> _##name##_autoregister(#name, CLSID_##name);

namespace Potto {
/// <summary>
/// The potto factory.
/// </summary>

const char* CLSID_PottoClassFactory = "10000000-0000-0000-0000-100000000000";
class POTTO_DECLARE_NOVTABLE PottoClassFactory
    : public PottoThreadSafeObjectRootBase
    , public IClassFactory {
public:
  // clang-format off
  POTTO_BEGIN_INTERFACE_MAP(PottoClassFactory)
    POTTO_INTERFACE_ENTRY(IClassFactory)
  POTTO_END_INTERFACE_MAP()
  // clang-format on

  /// <summary>
  ///
  /// </summary>
  PottoClassFactory()
      : m_pfnObjectCreator(nullptr) {
  }

  /// <summary>
  ///
  /// </summary>
  virtual ~PottoClassFactory() {
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="riid"></param>
  /// <param name="ppv"></param>
  /// <returns></returns>
  POTTO_ERROR CreateInstance(const PottoUuid& riid, void** ppv) override {
    PottoAssert(m_pfnObjectCreator != NULL, "");
    POTTO_ERROR error = POTTO_E_POINTER;
    if (ppv != NULL) {
      *ppv = NULL;
      error = m_pfnObjectCreator(riid, ppv);
    }
    return error;
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="p"></param>
  void SetCreatorFunction(void* p) override {
    m_pfnObjectCreator = reinterpret_cast<TypeObjectCreatorFunction>(p);
  }

private:
  TypeObjectCreatorFunction m_pfnObjectCreator;
};

/// <summary>
/// The potto singleton factory.
/// </summary>
///
const char* CLSID_PottoClassSingletonFactory = "10000000-0000-0000-0000-200000000000";
template <class T>
class POTTO_DECLARE_NOVTABLE PottoClassSingletonFactory
    : public PottoThreadSafeObjectRootBase
    , public IClassFactory {
public:
  // clang-format off
  POTTO_BEGIN_INTERFACE_MAP(PottoClassSingletonFactory)
    POTTO_INTERFACE_ENTRY(IClassFactory)
  POTTO_END_INTERFACE_MAP()
  // clang-format on

  /// <summary>
  ///
  /// </summary>
  virtual ~PottoClassSingletonFactory() {
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="riid"></param>
  /// <param name="ppv"></param>
  /// <returns></returns>
  POTTO_ERROR CreateInstance(const PottoUuid& riid, void** ppv) override {
    POTTO_ERROR error = POTTO_E_POINTER;
    if (ppv != nullptr) {
      if (POTTO_E_OK == m_Created && nullptr == m_spObj) {
        std::lock_guard<std::mutex> lock(m_mtxLock);

        // Did another thread get here first?
        if (POTTO_E_OK == m_Created && nullptr == m_spObj) {
          PottoCachedObject<T>* p;
          m_Created = PottoCachedObject<T>::CreateInstance(&p);
          if (POTTO_E_OK == m_Created) {
            m_Created = p->QueryInterface(IID_IUnknown, (void**)&m_spObj);
            if (POTTO_E_OK != m_Created)
              delete p;
          }
        }
      }

      if (m_Created == POTTO_E_OK)
        error = m_spObj->QueryInterface(riid, ppv);
      else
        error = m_Created;
    }
    return error;
  }

  /// <summary>
  ///
  /// </summary>
  POTTO_ERROR m_Created;

  /// <summary>
  ///
  /// </summary>
  PottoPtr<IUnknown> m_spObj;
};

/// <summary>
/// The potto object helper class.
/// </summary>
template <class T>
class PottoObjectClass {
public:
  DECLARE_CLASSFACTORY

  /// <summary>
  ///
  /// </summary>
  typedef PottoObjectCreator<PottoObject<T>> _CreatorClass;

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  static const PottoUuid& CLSID() {
    return T::CLSID();
  }

  /// <summary>
  ///
  /// </summary>
  /// <param name="pp"></param>
  /// <returns></returns>
  template <class Q>
  static POTTO_ERROR CreateInstance(Q** pp) {
    return T::_CreatorClass::CreateInstance(Q::IID(), (void**)pp);
  }
};
} // namespace Potto

#endif // POTTOOBJECT_H_
