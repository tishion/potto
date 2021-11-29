#ifndef POTTOERROR_H_
#define POTTOERROR_H_
#pragma once

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto {
/// <summary>
/// Type of potto error code.
/// </summary>
typedef int POTTO_ERROR;

/// <summary>
/// Error codes.
/// </summary>
enum POTTOR_ERROR_CODE {
  /// <summary> Operation fails. </summary>
  POTTO_E_FAIL = 0xFFFFFFFF,

  /// <summary>Operation succeeds.</summary>
  POTTO_E_OK = 0,

  /// <summary>Pointer is null.</summary>
  POTTO_E_POINTER,

  /// <summary>Invalid arguments.</summary>
  POTTO_E_INVALIDARG,

  /// <summary>Out of memory.</summary>
  POTTO_E_OUTOFMEMORY,

  /// <summary>Module was not found.</summary>
  POTTO_E_MODULENOTFOUND,

  /// <summary>Export function was not found.</summary>
  POTTO_E_EXPORTNOTFOUND,

  /// <summary>Class was not found.</summary>
  POTTO_E_CLASSNOTAVAILABLE,

  /// <summary>Interface was not found.</summary>
  POTTO_E_NOINTERFACE,
};
} // namespace Potto

#endif // POTTOERROR_H_
