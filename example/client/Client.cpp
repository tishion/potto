// Client.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32)
#include <windows.h>
#endif

#include <ghc/filesystem.hpp>

//#define POTTO_STATIC_LIB
#include <potto/potto.h>
#include <potto/pottointerface.hpp>
#include <potto/pottoptr.hpp>
#include <potto/pottouuid.hpp>

#include "../ark/include/Ark_CLSID.h"
#include "../ark/include/IFlyable.h"
#include "../ark/include/ILogger.h"
#include "../ark/include/IPlayer.h"

#define MAX_PATH 256

using namespace Potto;

int main() {
  char szExePath[MAX_PATH] = {0};
#if defined(_WIN32)
  ::GetModuleFileNameA(NULL, szExePath, _countof(szExePath));
#else
  ::readlink("/proc/self/exe", szExePath, PATH_MAX);
#endif

  ghc::filesystem::path exePath = szExePath;

  ghc::filesystem::path moduleRootPath = exePath.parent_path();
  ghc::filesystem::path moduleLibPath = moduleRootPath;
  moduleLibPath /= "modulelib.xml";

  Initialize(moduleLibPath.string(), moduleRootPath.string());
  {
    PottoPtr<ILogger> pLogger;
    POTTO_ERROR error = CreateInstance(CLSID_Misc, IIDOF(ILogger), (void**)&pLogger);
    if (POTTO_E_OK == error) {
      pLogger->Error("test test\n");
      PottoPtr<IPlayer> pPlayer;
      error = CreateInstance(CLSID_Misc, IIDOF(IPlayer), (void**)&pPlayer);
      // error = pLogger->QueryInterface(IIDOF(IPlayer), (void**)&pPlayer);
      if (POTTO_E_OK == error)
        pPlayer->Play("Lost River.mp3");
      else
        printf("Failed to query interface of IFlyable\n");
    } else
      printf("Failed to create instance of ILogger\n");
  }
  printf("Object should be destroyed\n");

  return 0;
}
