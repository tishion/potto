#ifndef POTTO_H_
#define POTTO_H_
#pragma once
#ifdef POTTO_STATIC_LIB
#	define POTTO_API							// For building or refer to static library
#else
#	ifdef POTTO_EXPORTS
#		define POTTO_API __declspec(dllexport)	// For building dynamic library
#	else
#		define POTTO_API __declspec(dllimport)	// For refer to dynamic library
#	endif // POTTO_EXPORTS
#endif // POTTO_STATIC_LIB
#include <string>
#include <potto/pottoerror.h>
#include <potto/pottouuid.h>

namespace Potto
{
	/// <summary>
	/// 
	/// </summary>
	POTTO_API POTTO_ERROR Initialize(
		const std::string& moduleDatabase,
		const std::string& mouduleRootPath);

	/// <summary>
	/// 
	/// </summary>
	POTTO_API POTTO_ERROR CreateInstance(
		const PottoUuid& classId,
		const PottoUuid& interfaceId,
		void** ppv,
		const std::string& moduleName = std::string());

	/// <summary>
	/// 
	/// </summary>
	POTTO_API POTTO_ERROR GetClassObject(
		const PottoUuid& classId,
		const PottoUuid& interfaceId,
		void** ppv,
		const std::string& moduleName = std::string());
}

#endif // POTTO_H_