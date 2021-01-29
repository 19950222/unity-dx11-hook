#pragma once

#include "Offset.hpp"
#include "Includes/Utils/Macro.h"

struct Il2CppString
{
	char padding_0[0x10];
	int length;
	wchar_t data[1];

	typedef Il2CppString* __fastcall il2cpp_string_new(const char* str);
	typedef Il2CppString* __fastcall il2CppString_Concat(Il2CppString* str0, Il2CppString* str1);

	static Il2CppString* New(const char* str)
	{
		return ((il2cpp_string_new*)(BASE_ADDR + Patchable::Offset::Il2CppString::oString_New))(str);
	}

	static Il2CppString* Concat(Il2CppString* str0, Il2CppString* str1)
	{
		return ((il2CppString_Concat*)(BASE_ADDR + Patchable::Offset::Il2CppString::oString_Concat))(str0, str1);
	}
};