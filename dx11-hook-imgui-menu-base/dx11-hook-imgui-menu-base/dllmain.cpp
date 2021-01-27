#include "pch.h"

#include "TypeDefs.h"
#include "Util.hpp"
#include "Macro.h"
#include "DXGI_INDEX.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hWndProc(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

void WINAPI Init();
bool WINAPI GetSwapChainVTable();
void PrintRVA();
void ConfigureGUIStyle();

///////////////////////
/// 윈도우 초기화
///////////////////////

HWND window = nullptr;
WNDPROC originWndProcHandler = nullptr;

///////////////////////
/// 로컬 변수
///////////////////////

bool bShowMenu = false;
bool bPresentInitialized = false;

/// <summary>
/// 새 스레드 생성
/// </summary>
bool bCreateThread = true;

/// <summary>
/// 콘솔 사용
/// </summary>
bool bUseConsole = true;

/// <summary>
/// 초기화 후 콘솔 닫기
/// </summary>
bool bCloseConsoleAfterInit = true;

/// <summary>
/// 찾은 Relative Virtual Address 출력
/// </summary>
bool bPrintRVA = true;

/// <summary>
/// 단축키
/// </summary>
UINT menuKey = VK_INSERT;


///////////////////////
/// D3D11 핸들
///////////////////////
ID3D11Device* pDevice;
ID3D11DeviceContext* pDeviceContext;
ID3D11RenderTargetView* pTargetRT;
IDXGISwapChain* pSwapChain;

///////////////////////
/// Virtual Address Table
///////////////////////

DWORD_PTR* pDeviceContextVTable = nullptr;
DWORD_PTR* pSwapChainVTable = nullptr;

///////////////////////
/// ImGui 폰트
///////////////////////

ImFont* m_default = nullptr;
ImFont* g_font;
ImFont* t_font;

/// <summary>
/// 스왑 체인 루프
/// </summary>
IDXGISwapChainPresent fnIDXGISwapChainPresent;
HRESULT __fastcall onPresent(IDXGISwapChain* _chain, UINT syncInterval, UINT flags);

bool InitializePresent(IDXGISwapChain* pChain, UINT SyncInterval, UINT Flags);

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);

		if (bCreateThread)
		{
			CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)Init, nullptr, NULL, nullptr);
		}
		else
		{
			Init();
		}
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	default:
		break;
	}
	
	return TRUE;
}

LRESULT CALLBACK hWndProc(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	POINT mPos;

	GetCursorPos(&mPos);
	ScreenToClient(window, &mPos);
	
	ImGui::GetIO().MousePos.x = (float)mPos.x;
	ImGui::GetIO().MousePos.y = (float)mPos.y;

	if (uMessage == WM_KEYUP)
	{
		if (wParam == menuKey)
		{
			bShowMenu = !bShowMenu;
		}
	}

	if (bShowMenu)
	{
		ImGui_ImplWin32_WndProcHandler(hwnd, uMessage, wParam, lParam);
		return true;
	}

	return CallWindowProc(originWndProcHandler, hwnd, uMessage, wParam, lParam);
}

void WINAPI Init()
{
	bool result;

	/// <summary>
	/// 콘솔 설정
	/// </summary>
	/// <returns></returns>
	if (bUseConsole)
	{
		result = Util::SetupConsole();
		if (!result)
		{
			cout << "[-] Console already allocated!" << endl;
		}
		else
		{
			cout << "[+] Console alloced successfully." << endl;
		}
	}

	/// <summary>
	/// 스왑체인 가상 함수 테이블
	/// </summary>
	/// <returns></returns>
	result = GetSwapChainVTable();
	if (!result)
	{
		cout << "[-] Hooking virtual table failed!" << endl;
		system("pause");
		return;
	}

	/// <summary>
	/// 후킹
	/// </summary>
	/// <returns></returns>
	try
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(LPVOID&)fnIDXGISwapChainPresent, (PBYTE)onPresent);
		DetourTransactionCommit();
	}
	catch (int e)
	{
		cout << "[-] SwapChainPresent detour failed : " << e << endl;
		return;
	}

	SleepUntil(bPresentInitialized, 100)

	if (bPrintRVA)
	{
		PrintRVA();
	}

	Sleep(100);

	if (bUseConsole && bCloseConsoleAfterInit)
	{
		result = Util::ReleaseConsole();
		if (!result)
		{
			cout << "[-] Console deallocation failed!" << endl;
			system("pause");
			return;
		}
	}

	cout << "[+] Successfully hooked!" << endl;
}

bool WINAPI GetSwapChainVTable()
{
	HRESULT result;

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_CLASSDC,
		[](const HWND hwnd, const UINT uMessage, const WPARAM wParam, const LPARAM lParam)
		{
			return DefWindowProc(hwnd, uMessage, wParam, lParam);
		},
		0L, 0L, GetModuleHandle(nullptr),
		nullptr, nullptr, nullptr, nullptr,
		L"DX", nullptr
	};

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow(L"DX", NULL, WS_OVERLAPPEDWINDOW,
	                         100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 2;
	swapChainDesc.BufferDesc.Height = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL requestedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	const UINT nRequestedFeatureLevel = 1;

	D3D_FEATURE_LEVEL supportedFeatureLevel;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
	                                       &requestedFeatureLevel, nRequestedFeatureLevel, D3D11_SDK_VERSION,
	                                       &swapChainDesc, &swapChain, &device, &supportedFeatureLevel, &deviceContext);

	if (FAILED(result))
	{
		cout << "[-] D3D11CreateDeviceAndSwapChain failed!" << endl;
		return false;
	}


	pSwapChainVTable = (DWORD_PTR*)swapChain;
	pSwapChainVTable = (DWORD_PTR*)pSwapChainVTable[0];

	fnIDXGISwapChainPresent = (IDXGISwapChainPresent)(DWORD_PTR)pSwapChainVTable[SC_PRESENT];
	cout << "[+] D3D11 present hooked!" << endl;

	if (bCreateThread)
	{
		Sleep(2000);
	}

	return true;
}

HRESULT __fastcall onPresent(IDXGISwapChain* _chain, UINT syncInterval, UINT flags)
{
	bool result;
	if (!bPresentInitialized)
	{
		result = InitializePresent(_chain, syncInterval, flags);
		if (!result)
		{
			return fnIDXGISwapChainPresent(_chain, syncInterval, flags);
		}
	}

	ImGui::CreateContext();

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();

	ImGui::NewFrame();

	const auto f = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs;

	ImGui::GetStyle().AntiAliasedFill = true;
	ImGui::GetStyle().AntiAliasedLines = true;


	if (bShowMenu)
	{
		bool bShow = true;

		ImGui::Begin("Root", nullptr, f);

		ImGui::PushFont(g_font);

		ImGui::ShowDemoWindow(&bShow);

		ImGui::PopFont();

		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();

	pDeviceContext->OMSetRenderTargets(1, &pTargetRT, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return fnIDXGISwapChainPresent(_chain, syncInterval, flags);
}

void PrintRVA()
{
	cout << "[+] pDevice              :: 0x" << hex << pDevice << endl;
	cout << "[+] pDeviceContext       :: 0x" << hex << pDeviceContext << endl;
	cout << "[+] pDeviceContextVTable :: 0x" << hex << pDeviceContextVTable << endl;
	cout << "[+] fnDXGISwapChainPres  :: 0x" << hex << fnIDXGISwapChainPresent << endl;
}

bool InitializePresent(IDXGISwapChain* pChain, UINT SyncInterval, UINT Flags)
{
	HRESULT result;

	result = pChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)&pDevice);
	if (FAILED(result))
	{
		return false;
	}

	pDevice->GetImmediateContext(&pDeviceContext);
	pSwapChain = pChain;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	pSwapChain->GetDesc(&swapChainDesc);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	window = swapChainDesc.OutputWindow;

	originWndProcHandler = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hWndProc);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pDeviceContext);

	ImGui::GetIO().ImeWindowHandle = window;

	ConfigureGUIStyle();

	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (pBackBuffer != nullptr)
	{
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTargetRT);
		pBackBuffer->Release();
	}

	bPresentInitialized = true;

	return true;
}

void ConfigureGUIStyle()
{
	if (!m_default)
	{
		m_default = ImGui::GetIO().Fonts->AddFontDefault();
	}

	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	ImGuiIO io = ImGui::GetIO();

	io.Fonts->AddFontDefault();

	g_font = io.Fonts->AddFontFromFileTTF((R"(C:\Windows\Fonts\Ruda-Regular.ttf)"), 16);
	t_font = io.Fonts->AddFontFromFileTTF((R"(C:\Windows\Fonts\Ruda-Regular.ttf)"), 20);
}
