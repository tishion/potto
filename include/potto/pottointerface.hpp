#ifndef POTTOINTERFACE_H_
#define POTTOINTERFACE_H_
#pragma once

#include <potto/pottoconf.hpp>
#include <potto/pottoerror.hpp>
#include <potto/pottouuid.hpp>

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto {
/// <summary>
/// The id of <see cref="IUnknown" />.
/// </summary>
const char* const IID_IUnknown = "10000000-0000-0000-0000-000000000000";

/// <summary>
/// The Unknown interface. This is the base of all interfaces in potto Framework.
/// </summary>
class POTTO_DECLARE_NOVTABLE IUnknown {
public:
  /// <summary>
  ///
  /// </summary>
  virtual ~IUnknown() {
  }

  /// <summary>
  ///
  /// </summary>
  virtual unsigned long AddRef() = 0;

  /// <summary>
  ///
  /// </summary>
  virtual unsigned long Release() = 0;

  /// <summary>
  ///
  /// </summary>
  virtual POTTO_ERROR QueryInterface(const PottoUuid& interfaceId, void** pp) = 0;
};

/// <summary>
/// The id of <see cref="IClassFactory" />.
/// </summary>
const char* const IID_IClassFactory = "10000000-0000-0000-0000-000000000001";

/// <summary>
/// The class factory interface.
/// </summary>
class POTTO_DECLARE_NOVTABLE IClassFactory : public IUnknown {
public:
  /// <summary>
  ///
  /// </summary>
  virtual ~IClassFactory() {
  }

  /// <summary>
  ///
  /// </summary>
  virtual POTTO_ERROR CreateInstance(const PottoUuid& riid, void** ppv) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual void SetCreatorFunction(void* p) = 0;
};
} // namespace Potto

/// <summary>
/// The begin of a interface.
/// </summary>
#define POTTO_INTERFACE_BEGINE(iid, name)                                                          \
  const char* const IID_##name = iid;                                                              \
  class POTTO_DECLARE_NOVTABLE name : public Potto::IUnknown {                                     \
  public:                                                                                          \
    virtual ~name(){};

/// <summary>
/// Declares a method.
/// </summary>
#define POTTO_METHOD(_type, _name_arg) virtual _type _name_arg = 0;

/// <summary>
/// The end of the interface.
/// </summary>
#define POTTO_INTERFACE_END                                                                        \
  }                                                                                                \
  ;

#endif // POTTOINTERFACE_H_