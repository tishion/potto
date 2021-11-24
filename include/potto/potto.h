#ifndef POTTO_H_
#define POTTO_H_
#pragma once

#ifdef POTTO_STATIC_LIB
#define POTTO_API // For building or refer to static library
#else
#ifdef POTTO_EXPORTS
#define POTTO_API __declspec(dllexport) // For building dynamic library
#else
#define POTTO_API __declspec(dllimport) // For refer to dynamic library
#endif                                  // POTTO_EXPORTS
#endif                                  // POTTO_STATIC_LIB

#include <string>

#include <potto/pottoerror.hpp>
#include <potto/pottouuid.hpp>

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto {
/// <summary>
/// Initializes the module library.
/// </summary>
/// <param name="moduleDatabase">The path of the module library.</param>
/// <param name="moduleRootPath">The path of the module root path.</param>
/// <returns>POTTO_E_OK if successful.</returns>
POTTO_API POTTO_ERROR Initialize(const std::string& moduleDatabase,
                                 const std::string& mouduleRootPath);

/// <summary>
/// Creates the instance.
/// </summary>
/// <param name="classId">The class id.</param>
/// <param name="interfaceId">The interface id.</param>
/// <param name="ppv">The pointer to receive the interface.</param>
/// <param name="moduleName">Optional. The path of the module to use.</param>
/// <returns>POTTO_E_OK if successful.</returns>
///	<remarks>
/// If the moduleName is not empty, this function will try to find the module in the
/// module entry list. If the module is found the function will use the module found,
/// if it is not found then the function will load the module. Then the function will
/// create the instance from the the module. If the module is not found or loaded or
///	failed to create the instance the function will fail with the error code.
/// </remarks>
POTTO_API POTTO_ERROR CreateInstance(const PottoUuid& classId, const PottoUuid& interfaceId,
                                     void** ppv, const std::string& moduleName = std::string());

/// <summary>
/// Gets the class object.
/// </summary>
/// <param name="classId">The class id.</param>
/// <param name="interfaceId">The interface id.</param>
/// <param name="ppv">the ponter to receive the interface.</param>
/// <param name="moduleName">The module path.</param>
/// <returns>POTTO_E_OK if successful.</returns>
///	<remarks>
/// If the moduleName is not empty, this function will try to find the module in the
/// module entry list. If the module is found the function will use the module found,
/// if it is not found then the function will load the module. Then the function will
/// create the instance from the the module. If the module is not found or loaded or
///	failed to get the class object the function will fail with the error code.
/// </remarks>
POTTO_API POTTO_ERROR GetClassObject(const PottoUuid& classId, const PottoUuid& interfaceId,
                                     void** ppv, const std::string& moduleName = std::string());
} // namespace Potto

#endif // POTTO_H_