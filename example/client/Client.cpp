// Client.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <linux/limits.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <libproc.h>
#endif

#if defined(_WIN32)
#define PATH_BUFFER_LIMIT MAX_PATH
#else
#define PATH_BUFFER_LIMIT PATH_MAX
#endif

#include <iostream>

#if defined(CXX_17_ENABLED)
#include <filesystem>
#define fsns std
#else
#include <ghc/filesystem.hpp>
#define fsns ghc
#endif

// #define POTTO_STATIC_LIB
#include <potto/potto.h>
#include <potto/pottointerface.hpp>
#include <potto/pottoptr.hpp>
#include <potto/pottouuid.hpp>

#include "../ark/include/libark_CLSID.h"

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

  fsns::filesystem::path exePath = szExePath;

  fsns::filesystem::path moduleRootPath = exePath.parent_path();
  fsns::filesystem::path moduleLibPath = moduleRootPath;
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
