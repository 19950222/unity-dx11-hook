#pragma once

#include "pch.h"

typedef HRESULT(__fastcall* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
typedef void(__stdcall* ID3D11DrawIndexed)(ID3D11DeviceContext* pDeviceContext, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation);

