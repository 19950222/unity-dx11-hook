#pragma once

#include <cstdio>
#include <cstdint>

typedef unsigned long long DWORD64;

namespace Patchable
{
	namespace Offset
	{
		namespace Class
		{
			DWORD64 GameManager;
		}

		namespace Method
		{

		}

		namespace Il2CppString
		{
			DWORD64 oString_New;
			DWORD64 oString_Concat;
		}
	}
}
