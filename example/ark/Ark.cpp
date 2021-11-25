// Ark.cpp : Defines the exported functions for the DLL application.
//

#include <potto/module/pottoexport.hpp>

POTTO_MODULE("{470c2187-24ad-4c0c-a388-19d85ae8b909}");

#if defined(_WIN32)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  return TRUE;
}
#endif
