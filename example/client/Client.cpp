// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <filesystem>
#include <potto/pottouuid.hpp>
#include <potto/pottoptr.hpp>
#include <potto/pottointerface.hpp>

#define POTTO_STATIC_LIB
#include <potto/potto.h>

#include "../include/Ark/Ark_CLSID.h"
#include "../include/Ark/ILogger.h"
#include "../include/Ark/IPlayer.h"
#include "../include/Ark/IFlyable.h"

using namespace Potto;

int main()
{
	char szExePath[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, szExePath, _countof(szExePath));

	std::tr2::sys::path exePath = szExePath;

	std::tr2::sys::path moduleRootPath = exePath.parent_path();
	std::tr2::sys::path moduleLibPath = moduleRootPath;
	moduleLibPath /= "modulelib.xml";

	Initialize(moduleLibPath.string(), moduleRootPath.string());
	{
		PottoPtr<ILogger> pLogger;
		POTTO_ERROR error = CreateInstance(CLSID_Misc, IIDOF(ILogger), (void**)&pLogger);
		if (POTTO_E_OK == error)
		{
			pLogger->Error("test test\r\n");
			PottoPtr<IPlayer> pPlayer;
			error = CreateInstance(CLSID_Misc, IIDOF(IPlayer), (void**)&pPlayer);
			//error = pLogger->QueryInterface(IIDOF(IPlayer), (void**)&pPlayer);
			if (POTTO_E_OK == error)
				pPlayer->Play("Lost River.mp3");
			else
				printf("Failed to query interface of IFlyable\r\n");
		}
		else
			printf("Failed to create instance of ILogger\r\n");
	}
	printf("Object should be destroyed\r\n");

	return 0;
}

