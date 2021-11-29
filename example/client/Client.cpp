// Client.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32)
#include <windows.h>
#define PATH_BUFFER_LIMIT MAX_PATH
#else
#define PATH_BUFFER_LIMIT PATH_MAX
#endif

#if defined(__APPLE__)
#include <libproc.h>
#endif

#include <iostream>

#include <ghc/filesystem.hpp>

//#define POTTO_STATIC_LIB
#include <potto/potto.h>
#include <potto/pottointerface.hpp>
#include <potto/pottoptr.hpp>
#include <potto/pottouuid.hpp>

#include "../ark/include/ark_CLSID.h"

#include "../ark/include/IFlyable.h"
#include "../ark/include/ILogger.h"
#include "../ark/include/IPlayer.h"

using namespace Potto;

int main() {
  char szExePath[PATH_BUFFER_LIMIT] = {0};
#if defined(_WIN32)
  if (::GetModuleFileNameA(NULL, szExePath, PATH_BUFFER_LIMIT) <= 0) {
#elif defined(__APPLE__)
  pid_t pid = getpid();
  if (::proc_pidpath(pid, szExePath, PATH_BUFFER_LIMIT) <= 0) {
#else
  if (::readlink("/proc/self/exe", szExePath, PATH_BUFFER_LIMIT) <= 0) {
#endif
    std::cerr << "Failed to get self path" << std::endl;
    return -1;
  }

  ghc::filesystem::path exePath = szExePath;

  ghc::filesystem::path moduleRootPath = exePath.parent_path();
  ghc::filesystem::path moduleLibPath = moduleRootPath;
  moduleLibPath /= "modulelib.xml";

  std::cout << "Initializing Potto ..." << std::endl;
  if (POTTO_E_OK == Initialize(moduleLibPath.string(), moduleRootPath.string())) {
    std::cout << "  [OK]" << std::endl;

    std::cout << "Creating Misc instance and querying ILogger interface ..." << std::endl;
    PottoPtr<ILogger> pLogger;
    POTTO_ERROR error = CreateInstance(CLSID_Misc, IIDOF(ILogger), (void**)&pLogger);
    if (POTTO_E_OK != error) {
      std::cerr << "  [Failed]" << std::endl;
      return -1;
    } else {
      std::cout << "  [OK]" << std::endl;
    }

    std::cout << "Invoking ILogger::Error() method ..." << std::endl;
    pLogger->Error("test test");

    std::cout << "Querying IPlayer interface from ILogger interface ..." << std::endl;
    PottoPtr<IPlayer> pPlayer;
    error = pLogger->QueryInterface(IIDOF(IPlayer), (void**)&pPlayer);
    if (POTTO_E_OK != error) {
      std::cerr << "  [Failed]" << std::endl;
      return -1;
    } else {
      std::cout << "  [OK]" << std::endl;
    }

    std::cout << "Invoking IPlayer::Play() method ..." << std::endl;
    if (!pPlayer->Play("Lost River.mp3")) {
      std::cerr << "  [Failed]" << std::endl;
    } else {
      std::cout << "  [OK]" << std::endl;
    }

  } else {
    std::cerr << "  [Failed]" << std::endl;
  }

  return 0;
}
