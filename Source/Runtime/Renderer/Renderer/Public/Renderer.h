#pragma once
#include <Singleton.h>
#include <StandardTypes/StandardTypes.h>
#include <DebugLineRenderer.h>

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
struct ID3D11Buffer;

struct Chunk;
struct Vertex;
struct ObjectBounds;

class WindowsApplication;
class RenderTarget;
class Camera;

class ConstantBuffer;
class RendererWindowsMessageObserver;

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

	void DisableDepthWriting();
	void EnableDepthWriting();

	void ResizeBackbuffer(int32 aWidth, int32 aHeight);

	void UpdateObjectBuffer(const Matrix& aMatrix, const ObjectBounds& aBounds);

	void SetMainCamera(Camera* aCamera);
	Camera* GetMainCamera();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain1* GetSwapChain();
	RenderTarget* GetBackBuffer();
	ID3D11SamplerState* GetSamplerState();
	ID3D11SamplerState** GetAdressOfSamplerState();

	const GraphicsCardInfo& GetGraphicsCardInfo() const;
	void DrawDebugLine(const Vector3& aStart, const Vector3& aEnd, const Color& aColor = Color(0, 1, 0, 1));
	void DrawDebugCube(const Vector3& aCenter, const Vector3& aSize, const Color& aColor = Color(0, 1, 0, 1));

	//TODO: remove this and make a proper pipeline for rendering not all in gameworld
	void RenderDebugLines();

	ComPtr<ID3D11Buffer> CreateIndexBuffer(const std::vector<uint32>& aIndexArray);
	ComPtr<ID3D11Buffer> CreateVertexBuffer(const std::vector<Vertex>& aVertexArray);

	void AddDrawCall();
	uint32 GetDrawCalls();

private:
	Renderer();
	~Renderer();

	bool SetupDevice();
	bool SetupSwapChain(WindowsApplication* aApp);
	bool SetupBackBufferAndDepthBuffer(WindowsApplication* aApp);
	bool SetupSamplerState();
	bool SetupRasterizerState();
private:
	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDeviceContext;
	ComPtr<IDXGISwapChain1> mSwapChain;
	ComPtr<ID3D11SamplerState> mSamplerState;
	ComPtr<ID3D11DepthStencilState> mDSSWriteZero;
#ifdef _DEBUG
	ComPtr<IDXGIAdapter3> mAdapter3;
#endif

	GraphicsCardInfo mGrapicsCardInfo;
	ConstantBuffer* mObjConstant = nullptr; 
	ConstantBuffer* mCamConstant = nullptr; 
	ConstantBuffer* mLightConstant = nullptr; 
	Camera* mDefaultCamera = nullptr; 
	Camera* mCurrentCamera = nullptr; 
	
	RendererWindowsMessageObserver* mWindowsMessageObserver;

	RenderTarget* mBackBuffer;
	Color mClearColor;
	bool mVsync;

	uint32 mDrawCalls;

	DebugLineRenderer mDebugLineRenderer;
};