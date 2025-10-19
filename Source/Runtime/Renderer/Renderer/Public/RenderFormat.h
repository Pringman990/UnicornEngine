#pragma once

#ifdef _WIN32
#include <d3d11.h>
#endif

enum class PrimitiveTopology : uint32
{
	Undefined = 0,
	LineList = 1,
	TriangleList = 2,
};

inline D3D_PRIMITIVE_TOPOLOGY ToD11Topology(PrimitiveTopology flags)
{
	switch (flags)
	{
		break;
	case PrimitiveTopology::LineList:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case PrimitiveTopology::TriangleList:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	default:
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		break;
	}
}

enum class ShaderStage : uint32
{
	Undefined = 0,
	VS = 1 << 0, //Vertex Shader
	FS = 1 << 1, //Fragment/Pixel shader
};

inline ShaderStage operator|(ShaderStage a, ShaderStage b)
{
	return static_cast<ShaderStage>(static_cast<uint32>(a) | static_cast<uint32>(b));
}

inline ShaderStage operator&(ShaderStage a, ShaderStage b)
{
	return static_cast<ShaderStage>(static_cast<uint32>(a) & static_cast<uint32>(b));
}

inline bool HasFlag(ShaderStage value, ShaderStage flag)
{
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

inline bool operator==(ShaderStage a, ShaderStage b)
{
	return HasFlag(a, b);
}

enum class BufferUsage : uint32
{
	Undefined = 0,
	Dynamic = 1 << 0, //D3D11_USAGE_DYNAMIC - CPU access
	Default = 1 << 1, //D3D11_USAGE_DEFAULT - no CPU access
};

inline BufferUsage operator&(BufferUsage a, BufferUsage b)
{
	return static_cast<BufferUsage>(static_cast<uint32>(a) & static_cast<uint32>(b));
}

inline D3D11_USAGE ToD11BufferUsage(BufferUsage flags)
{
	if ((flags & BufferUsage::Default) != BufferUsage::Undefined)
		return D3D11_USAGE_DEFAULT;

	if ((flags & BufferUsage::Dynamic) != BufferUsage::Undefined)
		return D3D11_USAGE_DYNAMIC;

	return D3D11_USAGE_DEFAULT;
}

enum class TextureUsage : uint32
{
	Undefined = 0, //D3D11_USAGE_DEFAULT
	CPURead = 1 << 0, //D3D11_USAGE_STAGING
	CPUWrite = 1 << 1 //D3D11_USAGE_DYNAMIC
};

inline TextureUsage operator|(TextureUsage a, TextureUsage b)
{
	return static_cast<TextureUsage>(static_cast<uint32>(a) | static_cast<uint32>(b));
}

inline TextureUsage operator&(TextureUsage a, TextureUsage b)
{
	return static_cast<TextureUsage>(static_cast<uint32>(a) & static_cast<uint32>(b));
}

inline bool HasFlag(TextureUsage value, TextureUsage flag)
{
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}


// Map high-level flags to DX11 D3D11_USAGE
inline D3D11_USAGE ToD11Usage(TextureUsage flags)
{
	if ((flags & TextureUsage::CPUWrite) != TextureUsage::Undefined &&
		(flags & TextureUsage::CPURead) != TextureUsage::Undefined
		)
		return D3D11_USAGE_STAGING;

	if ((flags & TextureUsage::CPUWrite) != TextureUsage::Undefined)
		return D3D11_USAGE_DYNAMIC;

	if ((flags & TextureUsage::CPURead) != TextureUsage::Undefined)
		return D3D11_USAGE_STAGING;

	return D3D11_USAGE_DEFAULT;
}

enum class TextureBindFlags : uint32
{
	Undefined = 0, //D3D11_USAGE_DEFAULT
	ShaderRead = 1 << 0, //D3D11_BIND_SHADER_RESOURCE
	ShaderWrite = 1 << 1, //D3D11_BIND_UNORDERED_ACCESS
	RenderTarget = 1 << 2, //D3D11_BIND_RENDER_TARGET
	DepthStencil = 1 << 3, //D3D11_BIND_DEPTH_STENCIL
};

inline TextureBindFlags operator|(TextureBindFlags a, TextureBindFlags b)
{
	return static_cast<TextureBindFlags>(static_cast<uint32>(a) | static_cast<uint32>(b));
}

inline TextureBindFlags operator&(TextureBindFlags a, TextureBindFlags b)
{
	return static_cast<TextureBindFlags>(static_cast<uint32>(a) & static_cast<uint32>(b));
}

inline bool HasFlag(TextureBindFlags value, TextureBindFlags flag)
{
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

// Map high-level flags to DX11 bind flags
inline UINT ToD11BindFlags(TextureBindFlags flags)
{
	UINT bind = 0;

	if ((flags & TextureBindFlags::ShaderRead) != TextureBindFlags::Undefined)
		bind |= D3D11_BIND_SHADER_RESOURCE;

	if ((flags & TextureBindFlags::ShaderWrite) != TextureBindFlags::Undefined)
		bind |= D3D11_BIND_UNORDERED_ACCESS;

	if ((flags & TextureBindFlags::RenderTarget) != TextureBindFlags::Undefined)
		bind |= D3D11_BIND_RENDER_TARGET;

	if ((flags & TextureBindFlags::DepthStencil) != TextureBindFlags::Undefined)
		bind |= D3D11_BIND_DEPTH_STENCIL;

	return bind;
}

enum class RenderFormat
{
	UNKNOWN,
	R32G32B32A32_TYPELESS,
	R32G32B32A32_FLOAT,
	R32G32B32A32_UINT,
	R32G32B32A32_SINT,
	R32G32B32_TYPELESS,
	R32G32B32_FLOAT,
	R32G32B32_UINT,
	R32G32B32_SINT,
	R16G16B16A16_TYPELESS,
	R16G16B16A16_FLOAT,
	R16G16B16A16_UNORM,
	R16G16B16A16_UINT,
	R16G16B16A16_SNORM,
	R16G16B16A16_SINT,
	R32G32_TYPELESS,
	R32G32_FLOAT,
	R32G32_UINT,
	R32G32_SINT,
	R32G8X24_TYPELESS,
	D32_FLOAT_S8X24_UINT,
	R32_FLOAT_X8X24_TYPELESS,
	X32_TYPELESS_G8X24_UINT,
	R10G10B10A2_TYPELESS,
	R10G10B10A2_UNORM,
	R10G10B10A2_UINT,
	R11G11B10_FLOAT,
	R8G8B8A8_TYPELESS,
	R8G8B8A8_UNORM,
	R8G8B8A8_UNORM_SRGB,
	R8G8B8A8_UINT,
	R8G8B8A8_SNORM,
	R8G8B8A8_SINT,
	R16G16_TYPELESS,
	R16G16_FLOAT,
	R16G16_UNORM,
	R16G16_UINT,
	R16G16_SNORM,
	R16G16_SINT,
	R32_TYPELESS,
	D32_FLOAT,
	R32_FLOAT,
	R32_UINT,
	R32_SINT,
	R24G8_TYPELESS,
	D24_UNORM_S8_UINT,
	R24_UNORM_X8_TYPELESS,
	X24_TYPELESS_G8_UINT,
	R8G8_TYPELESS,
	R8G8_UNORM,
	R8G8_UINT,
	R8G8_SNORM,
	R8G8_SINT,
	R16_TYPELESS,
	R16_FLOAT,
	D16_UNORM,
	R16_UNORM,
	R16_UINT,
	R16_SNORM,
	R16_SINT,
	R8_TYPELESS,
	R8_UNORM,
	R8_UINT,
	R8_SNORM,
	R8_SINT,
	A8_UNORM,
	R1_UNORM,
	R9G9B9E5_SHAREDEXP,
	R8G8_B8G8_UNORM,
	G8R8_G8B8_UNORM,
	BC1_TYPELESS,
	BC1_UNORM,
	BC1_UNORM_SRGB,
	BC2_TYPELESS,
	BC2_UNORM,
	BC2_UNORM_SRGB,
	BC3_TYPELESS,
	BC3_UNORM,
	BC3_UNORM_SRGB,
	BC4_TYPELESS,
	BC4_UNORM,
	BC4_SNORM,
	BC5_TYPELESS,
	BC5_UNORM,
	BC5_SNORM,
	B5G6R5_UNORM,
	B5G5R5A1_UNORM,
	B8G8R8A8_UNORM,
	B8G8R8X8_UNORM,
	R10G10B10_XR_BIAS_A2_UNORM,
	B8G8R8A8_TYPELESS,
	B8G8R8A8_UNORM_SRGB,
	B8G8R8X8_TYPELESS,
	B8G8R8X8_UNORM_SRGB,
	BC6H_TYPELESS,
	BC6H_UF16,
	BC6H_SF16,
	BC7_TYPELESS,
	BC7_UNORM,
	BC7_UNORM_SRGB,
	AYUV,
	Y410,
	Y416,
	NV12,
	P010,
	P016,
	OPAQUE_420,
	YUY2,
	Y210,
	Y216,
	NV11,
	AI44,
	IA44,
	P8,
	A8P8,
	B4G4R4A4_UNORM,

	P208,
	V208,
	V408,


	SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
	SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,


	FORCE_UINT
};

inline DXGI_FORMAT ToDXFormat(RenderFormat fmt)
{
	switch (fmt)
	{
	case RenderFormat::UNKNOWN: return DXGI_FORMAT_UNKNOWN;

	case RenderFormat::R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case RenderFormat::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case RenderFormat::R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
	case RenderFormat::R32G32B32A32_SINT: return DXGI_FORMAT_R32G32B32A32_SINT;

	case RenderFormat::R32G32B32_TYPELESS: return DXGI_FORMAT_R32G32B32_TYPELESS;
	case RenderFormat::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
	case RenderFormat::R32G32B32_UINT: return DXGI_FORMAT_R32G32B32_UINT;
	case RenderFormat::R32G32B32_SINT: return DXGI_FORMAT_R32G32B32_SINT;

	case RenderFormat::R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case RenderFormat::R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RenderFormat::R16G16B16A16_UNORM: return DXGI_FORMAT_R16G16B16A16_UNORM;
	case RenderFormat::R16G16B16A16_UINT: return DXGI_FORMAT_R16G16B16A16_UINT;
	case RenderFormat::R16G16B16A16_SNORM: return DXGI_FORMAT_R16G16B16A16_SNORM;
	case RenderFormat::R16G16B16A16_SINT: return DXGI_FORMAT_R16G16B16A16_SINT;

	case RenderFormat::R32G32_TYPELESS: return DXGI_FORMAT_R32G32_TYPELESS;
	case RenderFormat::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
	case RenderFormat::R32G32_UINT: return DXGI_FORMAT_R32G32_UINT;
	case RenderFormat::R32G32_SINT: return DXGI_FORMAT_R32G32_SINT;

	case RenderFormat::R32G8X24_TYPELESS: return DXGI_FORMAT_R32G8X24_TYPELESS;
	case RenderFormat::D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case RenderFormat::R32_FLOAT_X8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case RenderFormat::X32_TYPELESS_G8X24_UINT: return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

	case RenderFormat::R10G10B10A2_TYPELESS: return DXGI_FORMAT_R10G10B10A2_TYPELESS;
	case RenderFormat::R10G10B10A2_UNORM: return DXGI_FORMAT_R10G10B10A2_UNORM;
	case RenderFormat::R10G10B10A2_UINT: return DXGI_FORMAT_R10G10B10A2_UINT;
	case RenderFormat::R11G11B10_FLOAT: return DXGI_FORMAT_R11G11B10_FLOAT;

	case RenderFormat::R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case RenderFormat::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case RenderFormat::R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case RenderFormat::R8G8B8A8_UINT: return DXGI_FORMAT_R8G8B8A8_UINT;
	case RenderFormat::R8G8B8A8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
	case RenderFormat::R8G8B8A8_SINT: return DXGI_FORMAT_R8G8B8A8_SINT;

	case RenderFormat::R16G16_TYPELESS: return DXGI_FORMAT_R16G16_TYPELESS;
	case RenderFormat::R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
	case RenderFormat::R16G16_UNORM: return DXGI_FORMAT_R16G16_UNORM;
	case RenderFormat::R16G16_UINT: return DXGI_FORMAT_R16G16_UINT;
	case RenderFormat::R16G16_SNORM: return DXGI_FORMAT_R16G16_SNORM;
	case RenderFormat::R16G16_SINT: return DXGI_FORMAT_R16G16_SINT;

	case RenderFormat::R32_TYPELESS: return DXGI_FORMAT_R32_TYPELESS;
	case RenderFormat::D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
	case RenderFormat::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
	case RenderFormat::R32_UINT: return DXGI_FORMAT_R32_UINT;
	case RenderFormat::R32_SINT: return DXGI_FORMAT_R32_SINT;

	case RenderFormat::R24G8_TYPELESS: return DXGI_FORMAT_R24G8_TYPELESS;
	case RenderFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case RenderFormat::R24_UNORM_X8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case RenderFormat::X24_TYPELESS_G8_UINT: return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

	case RenderFormat::R8G8_TYPELESS: return DXGI_FORMAT_R8G8_TYPELESS;
	case RenderFormat::R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
	case RenderFormat::R8G8_UINT: return DXGI_FORMAT_R8G8_UINT;
	case RenderFormat::R8G8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
	case RenderFormat::R8G8_SINT: return DXGI_FORMAT_R8G8_SINT;

	case RenderFormat::R16_TYPELESS: return DXGI_FORMAT_R16_TYPELESS;
	case RenderFormat::R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
	case RenderFormat::D16_UNORM: return DXGI_FORMAT_D16_UNORM;
	case RenderFormat::R16_UNORM: return DXGI_FORMAT_R16_UNORM;
	case RenderFormat::R16_UINT: return DXGI_FORMAT_R16_UINT;
	case RenderFormat::R16_SNORM: return DXGI_FORMAT_R16_SNORM;
	case RenderFormat::R16_SINT: return DXGI_FORMAT_R16_SINT;

	case RenderFormat::R8_TYPELESS: return DXGI_FORMAT_R8_TYPELESS;
	case RenderFormat::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
	case RenderFormat::R8_UINT: return DXGI_FORMAT_R8_UINT;
	case RenderFormat::R8_SNORM: return DXGI_FORMAT_R8_SNORM;
	case RenderFormat::R8_SINT: return DXGI_FORMAT_R8_SINT;

	case RenderFormat::A8_UNORM: return DXGI_FORMAT_A8_UNORM;
	case RenderFormat::R1_UNORM: return DXGI_FORMAT_R1_UNORM;
	case RenderFormat::R9G9B9E5_SHAREDEXP: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	case RenderFormat::R8G8_B8G8_UNORM: return DXGI_FORMAT_R8G8_B8G8_UNORM;
	case RenderFormat::G8R8_G8B8_UNORM: return DXGI_FORMAT_G8R8_G8B8_UNORM;

	case RenderFormat::BC1_TYPELESS: return DXGI_FORMAT_BC1_TYPELESS;
	case RenderFormat::BC1_UNORM: return DXGI_FORMAT_BC1_UNORM;
	case RenderFormat::BC1_UNORM_SRGB: return DXGI_FORMAT_BC1_UNORM_SRGB;
	case RenderFormat::BC2_TYPELESS: return DXGI_FORMAT_BC2_TYPELESS;
	case RenderFormat::BC2_UNORM: return DXGI_FORMAT_BC2_UNORM;
	case RenderFormat::BC2_UNORM_SRGB: return DXGI_FORMAT_BC2_UNORM_SRGB;
	case RenderFormat::BC3_TYPELESS: return DXGI_FORMAT_BC3_TYPELESS;
	case RenderFormat::BC3_UNORM: return DXGI_FORMAT_BC3_UNORM;
	case RenderFormat::BC3_UNORM_SRGB: return DXGI_FORMAT_BC3_UNORM_SRGB;

	case RenderFormat::BC4_TYPELESS: return DXGI_FORMAT_BC4_TYPELESS;
	case RenderFormat::BC4_UNORM: return DXGI_FORMAT_BC4_UNORM;
	case RenderFormat::BC4_SNORM: return DXGI_FORMAT_BC4_SNORM;

	case RenderFormat::BC5_TYPELESS: return DXGI_FORMAT_BC5_TYPELESS;
	case RenderFormat::BC5_UNORM: return DXGI_FORMAT_BC5_UNORM;
	case RenderFormat::BC5_SNORM: return DXGI_FORMAT_BC5_SNORM;

	case RenderFormat::B5G6R5_UNORM: return DXGI_FORMAT_B5G6R5_UNORM;
	case RenderFormat::B5G5R5A1_UNORM: return DXGI_FORMAT_B5G5R5A1_UNORM;
	case RenderFormat::B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
	case RenderFormat::B8G8R8X8_UNORM: return DXGI_FORMAT_B8G8R8X8_UNORM;

	case RenderFormat::R10G10B10_XR_BIAS_A2_UNORM: return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	case RenderFormat::B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case RenderFormat::B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case RenderFormat::B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_TYPELESS;
	case RenderFormat::B8G8R8X8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

	case RenderFormat::BC6H_TYPELESS: return DXGI_FORMAT_BC6H_TYPELESS;
	case RenderFormat::BC6H_UF16: return DXGI_FORMAT_BC6H_UF16;
	case RenderFormat::BC6H_SF16: return DXGI_FORMAT_BC6H_SF16;
	case RenderFormat::BC7_TYPELESS: return DXGI_FORMAT_BC7_TYPELESS;
	case RenderFormat::BC7_UNORM: return DXGI_FORMAT_BC7_UNORM;
	case RenderFormat::BC7_UNORM_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;

	case RenderFormat::AYUV: return DXGI_FORMAT_AYUV;
	case RenderFormat::Y410: return DXGI_FORMAT_Y410;
	case RenderFormat::Y416: return DXGI_FORMAT_Y416;
	case RenderFormat::NV12: return DXGI_FORMAT_NV12;
	case RenderFormat::P010: return DXGI_FORMAT_P010;
	case RenderFormat::P016: return DXGI_FORMAT_P016;
	case RenderFormat::OPAQUE_420: return DXGI_FORMAT_420_OPAQUE;
	case RenderFormat::YUY2: return DXGI_FORMAT_YUY2;
	case RenderFormat::Y210: return DXGI_FORMAT_Y210;
	case RenderFormat::Y216: return DXGI_FORMAT_Y216;
	case RenderFormat::NV11: return DXGI_FORMAT_NV11;
	case RenderFormat::AI44: return DXGI_FORMAT_AI44;
	case RenderFormat::IA44: return DXGI_FORMAT_IA44;
	case RenderFormat::P8: return DXGI_FORMAT_P8;
	case RenderFormat::A8P8: return DXGI_FORMAT_A8P8;
	case RenderFormat::B4G4R4A4_UNORM: return DXGI_FORMAT_B4G4R4A4_UNORM;

	case RenderFormat::P208: return DXGI_FORMAT_P208;
	case RenderFormat::V208: return DXGI_FORMAT_V208;
	case RenderFormat::V408: return DXGI_FORMAT_V408;

	case RenderFormat::SAMPLER_FEEDBACK_MIN_MIP_OPAQUE: return DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
	case RenderFormat::SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE: return DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;

	case RenderFormat::FORCE_UINT: return DXGI_FORMAT_FORCE_UINT;

	default: return DXGI_FORMAT_UNKNOWN; // fallback for safety
	}
}

inline RenderFormat FromDXFormat(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_UNKNOWN: return RenderFormat::UNKNOWN;

	case DXGI_FORMAT_R32G32B32A32_TYPELESS: return RenderFormat::R32G32B32A32_TYPELESS;
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return RenderFormat::R32G32B32A32_FLOAT;
	case DXGI_FORMAT_R32G32B32A32_UINT: return RenderFormat::R32G32B32A32_UINT;
	case DXGI_FORMAT_R32G32B32A32_SINT: return RenderFormat::R32G32B32A32_SINT;

	case DXGI_FORMAT_R32G32B32_TYPELESS: return RenderFormat::R32G32B32_TYPELESS;
	case DXGI_FORMAT_R32G32B32_FLOAT: return RenderFormat::R32G32B32_FLOAT;
	case DXGI_FORMAT_R32G32B32_UINT: return RenderFormat::R32G32B32_UINT;
	case DXGI_FORMAT_R32G32B32_SINT: return RenderFormat::R32G32B32_SINT;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS: return RenderFormat::R16G16B16A16_TYPELESS;
	case DXGI_FORMAT_R16G16B16A16_FLOAT: return RenderFormat::R16G16B16A16_FLOAT;
	case DXGI_FORMAT_R16G16B16A16_UNORM: return RenderFormat::R16G16B16A16_UNORM;
	case DXGI_FORMAT_R16G16B16A16_UINT: return RenderFormat::R16G16B16A16_UINT;
	case DXGI_FORMAT_R16G16B16A16_SNORM: return RenderFormat::R16G16B16A16_SNORM;
	case DXGI_FORMAT_R16G16B16A16_SINT: return RenderFormat::R16G16B16A16_SINT;

	case DXGI_FORMAT_R32G32_TYPELESS: return RenderFormat::R32G32_TYPELESS;
	case DXGI_FORMAT_R32G32_FLOAT: return RenderFormat::R32G32_FLOAT;
	case DXGI_FORMAT_R32G32_UINT: return RenderFormat::R32G32_UINT;
	case DXGI_FORMAT_R32G32_SINT: return RenderFormat::R32G32_SINT;

	case DXGI_FORMAT_R32G8X24_TYPELESS: return RenderFormat::R32G8X24_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return RenderFormat::D32_FLOAT_S8X24_UINT;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return RenderFormat::R32_FLOAT_X8X24_TYPELESS;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return RenderFormat::X32_TYPELESS_G8X24_UINT;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS: return RenderFormat::R10G10B10A2_TYPELESS;
	case DXGI_FORMAT_R10G10B10A2_UNORM: return RenderFormat::R10G10B10A2_UNORM;
	case DXGI_FORMAT_R10G10B10A2_UINT: return RenderFormat::R10G10B10A2_UINT;
	case DXGI_FORMAT_R11G11B10_FLOAT: return RenderFormat::R11G11B10_FLOAT;

	case DXGI_FORMAT_R8G8B8A8_TYPELESS: return RenderFormat::R8G8B8A8_TYPELESS;
	case DXGI_FORMAT_R8G8B8A8_UNORM: return RenderFormat::R8G8B8A8_UNORM;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return RenderFormat::R8G8B8A8_UNORM_SRGB;
	case DXGI_FORMAT_R8G8B8A8_UINT: return RenderFormat::R8G8B8A8_UINT;
	case DXGI_FORMAT_R8G8B8A8_SNORM: return RenderFormat::R8G8B8A8_SNORM;
	case DXGI_FORMAT_R8G8B8A8_SINT: return RenderFormat::R8G8B8A8_SINT;

	case DXGI_FORMAT_R16G16_TYPELESS: return RenderFormat::R16G16_TYPELESS;
	case DXGI_FORMAT_R16G16_FLOAT: return RenderFormat::R16G16_FLOAT;
	case DXGI_FORMAT_R16G16_UNORM: return RenderFormat::R16G16_UNORM;
	case DXGI_FORMAT_R16G16_UINT: return RenderFormat::R16G16_UINT;
	case DXGI_FORMAT_R16G16_SNORM: return RenderFormat::R16G16_SNORM;
	case DXGI_FORMAT_R16G16_SINT: return RenderFormat::R16G16_SINT;

	case DXGI_FORMAT_R32_TYPELESS: return RenderFormat::R32_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT: return RenderFormat::D32_FLOAT;
	case DXGI_FORMAT_R32_FLOAT: return RenderFormat::R32_FLOAT;
	case DXGI_FORMAT_R32_UINT: return RenderFormat::R32_UINT;
	case DXGI_FORMAT_R32_SINT: return RenderFormat::R32_SINT;

	case DXGI_FORMAT_R24G8_TYPELESS: return RenderFormat::R24G8_TYPELESS;
	case DXGI_FORMAT_D24_UNORM_S8_UINT: return RenderFormat::D24_UNORM_S8_UINT;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return RenderFormat::R24_UNORM_X8_TYPELESS;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return RenderFormat::X24_TYPELESS_G8_UINT;

	case DXGI_FORMAT_R8G8_TYPELESS: return RenderFormat::R8G8_TYPELESS;
	case DXGI_FORMAT_R8G8_UNORM: return RenderFormat::R8G8_UNORM;
	case DXGI_FORMAT_R8G8_UINT: return RenderFormat::R8G8_UINT;
	case DXGI_FORMAT_R8G8_SNORM: return RenderFormat::R8G8_SNORM;
	case DXGI_FORMAT_R8G8_SINT: return RenderFormat::R8G8_SINT;

	case DXGI_FORMAT_R16_TYPELESS: return RenderFormat::R16_TYPELESS;
	case DXGI_FORMAT_R16_FLOAT: return RenderFormat::R16_FLOAT;
	case DXGI_FORMAT_D16_UNORM: return RenderFormat::D16_UNORM;
	case DXGI_FORMAT_R16_UNORM: return RenderFormat::R16_UNORM;
	case DXGI_FORMAT_R16_UINT: return RenderFormat::R16_UINT;
	case DXGI_FORMAT_R16_SNORM: return RenderFormat::R16_SNORM;
	case DXGI_FORMAT_R16_SINT: return RenderFormat::R16_SINT;

	case DXGI_FORMAT_R8_TYPELESS: return RenderFormat::R8_TYPELESS;
	case DXGI_FORMAT_R8_UNORM: return RenderFormat::R8_UNORM;
	case DXGI_FORMAT_R8_UINT: return RenderFormat::R8_UINT;
	case DXGI_FORMAT_R8_SNORM: return RenderFormat::R8_SNORM;
	case DXGI_FORMAT_R8_SINT: return RenderFormat::R8_SINT;

	case DXGI_FORMAT_A8_UNORM: return RenderFormat::A8_UNORM;
	case DXGI_FORMAT_R1_UNORM: return RenderFormat::R1_UNORM;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return RenderFormat::R9G9B9E5_SHAREDEXP;
	case DXGI_FORMAT_R8G8_B8G8_UNORM: return RenderFormat::R8G8_B8G8_UNORM;
	case DXGI_FORMAT_G8R8_G8B8_UNORM: return RenderFormat::G8R8_G8B8_UNORM;

	case DXGI_FORMAT_BC1_TYPELESS: return RenderFormat::BC1_TYPELESS;
	case DXGI_FORMAT_BC1_UNORM: return RenderFormat::BC1_UNORM;
	case DXGI_FORMAT_BC1_UNORM_SRGB: return RenderFormat::BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_TYPELESS: return RenderFormat::BC2_TYPELESS;
	case DXGI_FORMAT_BC2_UNORM: return RenderFormat::BC2_UNORM;
	case DXGI_FORMAT_BC2_UNORM_SRGB: return RenderFormat::BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_TYPELESS: return RenderFormat::BC3_TYPELESS;
	case DXGI_FORMAT_BC3_UNORM: return RenderFormat::BC3_UNORM;
	case DXGI_FORMAT_BC3_UNORM_SRGB: return RenderFormat::BC3_UNORM_SRGB;

	case DXGI_FORMAT_BC4_TYPELESS: return RenderFormat::BC4_TYPELESS;
	case DXGI_FORMAT_BC4_UNORM: return RenderFormat::BC4_UNORM;
	case DXGI_FORMAT_BC4_SNORM: return RenderFormat::BC4_SNORM;
	case DXGI_FORMAT_BC5_TYPELESS: return RenderFormat::BC5_TYPELESS;
	case DXGI_FORMAT_BC5_UNORM: return RenderFormat::BC5_UNORM;
	case DXGI_FORMAT_BC5_SNORM: return RenderFormat::BC5_SNORM;

	case DXGI_FORMAT_B5G6R5_UNORM: return RenderFormat::B5G6R5_UNORM;
	case DXGI_FORMAT_B5G5R5A1_UNORM: return RenderFormat::B5G5R5A1_UNORM;
	case DXGI_FORMAT_B8G8R8A8_UNORM: return RenderFormat::B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_UNORM: return RenderFormat::B8G8R8X8_UNORM;

	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return RenderFormat::R10G10B10_XR_BIAS_A2_UNORM;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS: return RenderFormat::B8G8R8A8_TYPELESS;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return RenderFormat::B8G8R8A8_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS: return RenderFormat::B8G8R8X8_TYPELESS;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return RenderFormat::B8G8R8X8_UNORM_SRGB;

	case DXGI_FORMAT_BC6H_TYPELESS: return RenderFormat::BC6H_TYPELESS;
	case DXGI_FORMAT_BC6H_UF16: return RenderFormat::BC6H_UF16;
	case DXGI_FORMAT_BC6H_SF16: return RenderFormat::BC6H_SF16;
	case DXGI_FORMAT_BC7_TYPELESS: return RenderFormat::BC7_TYPELESS;
	case DXGI_FORMAT_BC7_UNORM: return RenderFormat::BC7_UNORM;
	case DXGI_FORMAT_BC7_UNORM_SRGB: return RenderFormat::BC7_UNORM_SRGB;

	case DXGI_FORMAT_AYUV: return RenderFormat::AYUV;
	case DXGI_FORMAT_Y410: return RenderFormat::Y410;
	case DXGI_FORMAT_Y416: return RenderFormat::Y416;
	case DXGI_FORMAT_NV12: return RenderFormat::NV12;
	case DXGI_FORMAT_P010: return RenderFormat::P010;
	case DXGI_FORMAT_P016: return RenderFormat::P016;
	case DXGI_FORMAT_420_OPAQUE: return RenderFormat::OPAQUE_420;
	case DXGI_FORMAT_YUY2: return RenderFormat::YUY2;
	case DXGI_FORMAT_Y210: return RenderFormat::Y210;
	case DXGI_FORMAT_Y216: return RenderFormat::Y216;
	case DXGI_FORMAT_NV11: return RenderFormat::NV11;
	case DXGI_FORMAT_AI44: return RenderFormat::AI44;
	case DXGI_FORMAT_IA44: return RenderFormat::IA44;
	case DXGI_FORMAT_P8: return RenderFormat::P8;
	case DXGI_FORMAT_A8P8: return RenderFormat::A8P8;
	case DXGI_FORMAT_B4G4R4A4_UNORM: return RenderFormat::B4G4R4A4_UNORM;

	case DXGI_FORMAT_P208: return RenderFormat::P208;
	case DXGI_FORMAT_V208: return RenderFormat::V208;
	case DXGI_FORMAT_V408: return RenderFormat::V408;

	case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE: return RenderFormat::SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
	case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE: return RenderFormat::SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;

	case DXGI_FORMAT_FORCE_UINT: return RenderFormat::FORCE_UINT;

	default: return RenderFormat::UNKNOWN;
	}
}