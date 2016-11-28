#pragma once
#include <potto/internal/pottoobject.hpp>
#include "include/IFlyable.h"

using namespace Potto;

/// <summary>
/// 
/// </summary>
POTTO_MULTITHREADED_OBJECT("0633e58e-11b0-4aa4-9ed7-bb147367fbe0", Sparrow)
	POTTO_IMPLEMENT_INTERFACE(IFlyable)
{
public:
	POTTO_BEGIN_INTERFACE_MAP(Sparrow)
		POTTO_INTERFACE_ENTRY(IFlyable)
	POTTO_END_INTERFACE_MAP()

	Sparrow();

	bool Fly(int distance);
};

/// <summary>
/// 
/// </summary>
POTTO_CLASS_REGISTER(Sparrow);

