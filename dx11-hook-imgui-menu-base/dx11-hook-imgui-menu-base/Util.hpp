#pragma once

#include "pch.h"

namespace Util
{
	inline bool SetupConsole()
	{
		const bool result = AllocConsole();

		if (result)
		{
			FILE* pFile = nullptr;
			freopen_s(&pFile, "CONOUT$", "w", stdout);
			freopen_s(&pFile, "CONOUT$", "w", stderr);
			freopen_s(&pFile, "CONIN$", "r", stdin);

			SetConsoleTitle(L"DirectX 11 Hook");
		}

		return result;
	}

	inline bool ReleaseConsole()
	{
		const bool result = FreeConsole();

		if (result)
		{
			
		}

		return result;
	}
}