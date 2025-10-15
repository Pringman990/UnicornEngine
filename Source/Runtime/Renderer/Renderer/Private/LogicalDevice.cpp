#include "LogicalDevice.h"

#include <d3d11.h>

LogicalDevice::LogicalDevice()
{
}

LogicalDevice::~LogicalDevice()
{
	mDevice.Reset();
	mImmediateContext.Reset();
}

bool LogicalDevice::Init()
{
	D3D_FEATURE_LEVEL featureLevel = { D3D_FEATURE_LEVEL_11_1 };
	
	UINT creationFlags = 0;
#ifdef _DEBUG
#define REPORT_DX_WARNINGS
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&mDevice,
		&featureLevel,
		&mImmediateContext
	);

	if (FAILED(hr))
	{
		THROW("Failed to setup Device: {}", hr);
		return false;
	}

	return true;
}

