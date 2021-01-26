#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include "detours.h"

#include "D3D11.h"
#include <D3Dcompiler.h>

#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "detours.lib")

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace std;

#endif //PCH_H
