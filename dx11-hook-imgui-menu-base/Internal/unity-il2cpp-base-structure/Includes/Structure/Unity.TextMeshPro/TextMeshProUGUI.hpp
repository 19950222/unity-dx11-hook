#pragma once

#include "Includes/Patchable/Offset.hpp"
#include "Includes/Utils/Macro.h"
#include "Includes/Structure/IL2CPP/Il2CppString.hpp"
#include "Includes/Structure/UnityEngine/Color.hpp"

using namespace IL2CPP;
using namespace Patchable::Offset::TextMeshPro::TextMeshProUGUI;

struct TextMeshProUGUI
{
	char padding_0[0x17C];
	UnityEngine::Color fontColor; // 0x17C

	typedef void __fastcall fnSetText(TextMeshProUGUI* textMeshProUGUI, Il2CppString* text);
	typedef void __fastcall fnSetFaceColor(TextMeshProUGUI* textMeshProUGUI, UnityEngine::Color newColor);
	typedef void __fastcall fnSetAllDirty(TextMeshProUGUI* textMeshProUGUI);

	void SetText(Il2CppString* text)
	{
		(reinterpret_cast<fnSetText*>(BASE_ADDRESS + oSetText))(this, text);
		(reinterpret_cast<fnSetAllDirty*>(BASE_ADDRESS + oSetAllDirty))(this);
	}

	void SetFaceColor(UnityEngine::Color newColor)
	{
		(reinterpret_cast<fnSetFaceColor*>(BASE_ADDRESS + oSetFaceColor))(this, newColor);
		(reinterpret_cast<fnSetAllDirty*>(BASE_ADDRESS + oSetAllDirty))(this);
	}
};
