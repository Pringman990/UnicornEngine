//#include "InputLayoutFactory.h"
//
//D3D12_INPUT_LAYOUT_DESC InputLayerFactory::GetStandardLayout(InputLayoutType Type)
//{
//	switch (Type)
//	{
//	case InputLayoutType::Sprite:
//		return GetSpriteLayout();
//	case InputLayoutType::StaticMesh:
//		break;
//	default:
//		break;
//	}
//	return D3D12_INPUT_LAYOUT_DESC();
//}
//
//D3D12_INPUT_LAYOUT_DESC InputLayerFactory::GetSpriteLayout()
//{
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//	{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//	};
//
//	D3D12_INPUT_LAYOUT_DESC desc = {};
//	desc.pInputElementDescs = inputLayout;
//	desc.NumElements = _countof(inputLayout);
//
//	return desc;
//}
