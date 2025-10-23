#include "LogicalDevice.h"

#include "Renderer.h"
#include "CommandList.h"

#include <d3d11.h>

LogicalDevice::LogicalDevice()
{
}

LogicalDevice::~LogicalDevice()
{
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

void LogicalDevice::Destroy()
{
	mFreeCommandLists.clear();
	mCommandLists.clear();
	mImmediateContext->ClearState();
	mImmediateContext->Flush();
	mImmediateContext.Reset();
}

CommandList* LogicalDevice::RequestCommandList(Renderer* InRenderer)
{
	if (!mFreeCommandLists.empty())
	{
		CommandList* list = mFreeCommandLists.back();
		mFreeCommandLists.pop_back();

		return list;
	}

	ComPtr<ID3D11DeviceContext> defferedContext;
	HRESULT hr = mDevice->CreateDeferredContext(0, defferedContext.GetAddressOf());

	if (FAILED(hr))
	{
		THROW("Failed to create deffered context: {}", hr);
		return nullptr;
	}

	OwnedPtr<CommandList> list = MakeOwned<CommandList>(defferedContext, InRenderer);
	mCommandLists.push_back(std::move(list));

	return mCommandLists.back().get();
}

void LogicalDevice::RecycleCommandList(CommandList* List)
{
	mFreeCommandLists.push_back(List);
}