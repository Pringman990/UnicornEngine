#pragma once
#include <EngineMinimal.h>
#include <RendererMinimal.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

class Renderer;
class CommandList;

class LogicalDevice
{
public:
	
	CommandList* RequestCommandList(Renderer* InRenderer);
	void RecycleCommandList(CommandList* List);

	inline CommandList* GetImmediateContext() { return mImmediateContext.get(); }
	inline ID3D11DeviceContext* GetDx11ImmediateContext() const { return mDx11ImmediateContext.Get(); }

	inline ID3D11Device* GetRaw() const { return mDevice.Get(); };

public:
	ID3D11Device* operator->() const noexcept
	{
		return mDevice.Get();
	}

private:
	friend class Renderer;

	/*
	* Only one single LogicalDevice can exist in DX11 at any time
	* But this is not a common singleton, we only hide the creation
	*/
	LogicalDevice();
	~LogicalDevice();

	/*
	* We hide the init and only allow the renderer to access it
	*/
	bool Init();

	void Destroy();
private:
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDx11ImmediateContext;

	OwnedPtr<CommandList> mImmediateContext;

	Vector<CommandList*> mFreeCommandLists;
	Vector<OwnedPtr<CommandList>> mCommandLists;
};