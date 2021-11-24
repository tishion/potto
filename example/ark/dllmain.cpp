// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <potto/module/pottoexport.hpp>

POTTO_MODULE("{470c2187-24ad-4c0c-a388-19d85ae8b909}");

/// <summary>
/// 
/// </summary>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}