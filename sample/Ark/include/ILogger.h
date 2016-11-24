#pragma once

#include <potto/pottointerface.h>

/// <summary>
/// Logger interface declared with potto macros.
/// This interface is equivalent to 
// class ILogger : public PottoCom::IUnknown
// {
// public:
//	 virtual ~ILogger() {}
//	 virtual void Info(char*) = 0;
//	 virtual void Warn(char*) = 0;
//	 virtual void Error(char*) = 0;
//	 virtual void Fatal(char*) = 0;
// };
/// </summary>
POTTO_INTERFACE_BEGINE("ddc8bdd3-99d6-45ff-bda8-46850e318703", ILogger)
	/// <summary>
	/// Outputs Info message.
	/// </summary>
	POTTO_METHOD(void, Info(char*))
	
	/// <summary>
	/// Outputs Warn message.
	/// </summary>
	POTTO_METHOD(void, Warn(char*))
	
	/// <summary>
	/// Outputs Error message.
	/// </summary>
	POTTO_METHOD(void, Error(char*))
	
	/// <summary>
	/// Outputs Fatal message.
	/// </summary>
	POTTO_METHOD(void, Fatal(char*))
POTTO_INTERFACE_END
