#pragma once

#include "Includes/Patchable/Offset.hpp"
#include "Includes/Utils/Macro.h"

using namespace Patchable::Offset::Il2CppString;

struct Il2CppString
{
	
	char padding_0[0x10];
	int length;
	wchar_t data[1];

	typedef Il2CppString* __fastcall il2cpp_string_new(const char* str);
	typedef Il2CppString* __fastcall il2CppString_Concat(Il2CppString* str0, Il2CppString* str1);

	static Il2CppString* New(const char* str)
	{
		return (reinterpret_cast<il2cpp_string_new*>(BASE_ADDRESS + oString_New))(str);
	}

	static Il2CppString* Concat(Il2CppString* str0, Il2CppString* str1)
	{
		return (reinterpret_cast<il2CppString_Concat*>(BASE_ADDRESS + oString_Concat))(str0, str1);
	}
};