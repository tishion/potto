#pragma once

#include <potto/pottointerface.hpp>

/// <summary>
/// Logger interface declared with potto macros.
/// This interface is equivalent to 
// class ILogger : public PottoCom::IUnknown
// {
// public:
//	 virtual ~IFlyable() {}
//	 virtual bool Fly(int distance) = 0;
// };
/// </summary>
POTTO_INTERFACE_BEGINE("{d4c5a174-763c-4e6b-83f8-73355002fb7f}", IFlyable)
	/// <summary>
	/// Outputs Info message.
	/// </summary>
	POTTO_METHOD(bool, Fly(int distance))
POTTO_INTERFACE_END