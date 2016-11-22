#ifndef POTTOERROR_H_
#define POTTOERROR_H_
#pragma once

/// <summary>
/// Type of potto error code.
/// </summary>
typedef int POTTO_ERROR;

/// <summary>
/// Error codes.
/// </summary>
enum POTTOR_ERROR_CODE
{
	// Operation fails
	POTTO_E_FAIL = 0xFFFFFFFF,

	// Operation succeeds
	POTTO_E_OK = 0,
	
	// Pointer is null
	POTTO_E_POINTER,
	
	// Invalid arguments
	POTTO_E_INVALIDARG,
	
	// Out of memory
	POTTO_E_OUTOFMEMORY,
	
	// Module was not found
	POTTO_E_MODULENOTFOUND,
	
	// Export function was not found
	POTTO_E_EXPORTNOTFOUND,
	
	// Class was not found
	POTTO_E_CLASSNOTAVAILABLE,
	
	// Interface was not found
	POTTO_E_NOINTERFACE,
};

#endif // POTTOERROR_H_