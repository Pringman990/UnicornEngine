#pragma once
#include <Singleton.h>
#include <StandardTypes/StandardTypes.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11SamplerState;
struct IDXGIAdapter3;
struct ID3D11Texture2D;

struct Chunk;
struct Vertex;

class WindowsApplication;
class RenderTarget;
class Camera;

class ConstantBuffer;

struct GraphicsCardInfo
{
	std::string name = "";
	uint32 vendorID = 0;
	uint64 totalVideoMemoryInMB = 0;	//VRAM
	uint32 systemMemoryInMB = 0;		//For integrated GPU, the dedicated RAM memory for GPU
	uint32 sharedMemoryInMB = 0;		//Fallback RAM memory for the GPU if it runs out of VRAM
	uint64 currentVideoMemoryUsage = 0;
	uint64 approxFreeVideoMemory = 0;
};


class Renderer : public Singleton<Renderer>
{
	friend class Singleton<Renderer>;
public:
	bool Init();
	void PreRender();
	void RenderToBackbuffer();
	void CopyTextureToBackbuffer(ID3D11Texture2D* aTexture);
	void Present();

	void UpdateObjectBuffer(const Matrix& aMatrix);

	void SetMainCamera(Camera* aCamera);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain1* GetSwapChain();
	RenderTarget* GetBackBuffer();
	ID3D11SamplerState* GetSamplerState();
	ID3D11SamplerState** GetAdressOfSamplerState();

	const GraphicsCardInfo& GetGraphicsCardInfo() const;

private:
	Renderer();
	~Renderer();

	bool SetupDevice();
	bool SetupSwapChain(WindowsApplication* aApp);
	bool SetupBackBufferAndDepthBuffer(WindowsApplication* aApp);
	bool SetupSamplerState();
private:
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDeviceContext;
	ComPtr<IDXGISwapChain1> mSwapChain;
	ComPtr<ID3D11SamplerState> mSamplerState;
#ifdef _DEBUG
	ComPtr<IDXGIAdapter3> mAdapter3;
#endif

	GraphicsCardInfo mGrapicsCardInfo;
	ConstantBuffer* mObjConstant = nullptr; 
	ConstantBuffer* mCamConstant = nullptr; 
	Camera* mDefaultCamera = nullptr; 
	Camera* mCurrentCamera = nullptr; 
	
	RenderTarget* mBackBuffer;
	Color mClearColor;
	bool mVsync;
};