#include "struct_and_const.h"
#include "Effects.h"

#pragma region InputLayoutDesc

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Basic32[3] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::ColorWire[2] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::NormalMap[4] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::SkyBox[1] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Skin[5] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WEIGHTS",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT,   0, 44,	D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Sprite2D[2] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}

};
#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::Basic32	= 0;
ID3D11InputLayout* InputLayouts::ColorWire	= 0;
ID3D11InputLayout* InputLayouts::NormalMap	= 0;
ID3D11InputLayout* InputLayouts::SkyBox		= 0;
ID3D11InputLayout* InputLayouts::Skin		= 0;
ID3D11InputLayout* InputLayouts::Sprite2D	= 0;

void InputLayouts::InitAll(ID3D11Device* device)
{
	//
	// PosNormal
	//
	D3DX11_PASS_DESC passDesc;
	Effects::BasicFX->LightTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Basic32));

	//
	// Vertex
	//
	D3DX11_PASS_DESC passDesc_color;
	Effects::ColorFX->mTech->GetPassByIndex(0)->GetDesc(&passDesc_color);
	HR(device->CreateInputLayout(InputLayoutDesc::ColorWire, 2, passDesc_color.pIAInputSignature,
		passDesc_color.IAInputSignatureSize, &ColorWire));


	//
	// NormalMap
	//
	D3DX11_PASS_DESC passDescNormal;
	Effects::NormalFX->LightTech->GetPassByIndex(0)->GetDesc(&passDescNormal);
	HR(device->CreateInputLayout(InputLayoutDesc::NormalMap, 4, passDescNormal.pIAInputSignature,
		passDescNormal.IAInputSignatureSize, &NormalMap));

	//
	// SkyBox
	//
	D3DX11_PASS_DESC passDescSkyBox;
	Effects::SkyFX->mTech->GetPassByIndex(0)->GetDesc(&passDescSkyBox);
	HR(device->CreateInputLayout(InputLayoutDesc::SkyBox, 1, passDescSkyBox.pIAInputSignature,
		passDescSkyBox.IAInputSignatureSize, &SkyBox));

	//
	// Skin
	//
	D3DX11_PASS_DESC passDescSkin;
	Effects::SkinFX->LightTech->GetPassByIndex(0)->GetDesc(&passDescSkin);
	HR(device->CreateInputLayout(InputLayoutDesc::Skin, 5, passDescSkin.pIAInputSignature,
		passDescSkin.IAInputSignatureSize, &Skin));

	//
	// Sprite2D
	//
	D3DX11_PASS_DESC passDescSprite2D;
	Effects::Sprite2DFX->mTech->GetPassByIndex(0)->GetDesc(&passDescSprite2D);
	HR(device->CreateInputLayout(InputLayoutDesc::Sprite2D, 2, passDescSprite2D.pIAInputSignature,
		passDescSprite2D.IAInputSignatureSize, &Sprite2D));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(Basic32);
	ReleaseCOM(ColorWire);
	ReleaseCOM(NormalMap);
	ReleaseCOM(SkyBox);
	ReleaseCOM(Skin);
}

#pragma endregion
