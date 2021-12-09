#ifndef VERTEX_H
#define VERTEX_H

//***************************************************************************************
// Vertex.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines vertex structures and input layouts.
//***************************************************************************************


#include "DX11Define.h"
#include "d3dx11Effect.h"	// effect, tech

namespace BTStruct
{
	// 같은 이름이 있어서 문제 바꿔야한다.
	struct VertexColor
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct Vertex2D
	{
		XMFLOAT3 Pos;
		XMFLOAT2 UV;
	};

	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 UV;
		XMFLOAT3 Tangent;
	};

	struct VertexSkin
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 UV;

		XMFLOAT3 Weights;
		BYTE BoneIndices[4];
	};

	struct VertexPos
	{
		XMFLOAT3 Pos;
	};
}


class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC ColorWire[2];
	static const D3D11_INPUT_ELEMENT_DESC NormalMap[4];
	static const D3D11_INPUT_ELEMENT_DESC SkyBox[1];
	static const D3D11_INPUT_ELEMENT_DESC Skin[5];
	static const D3D11_INPUT_ELEMENT_DESC Sprite2D[2];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* ColorWire;
	static ID3D11InputLayout* NormalMap;
	static ID3D11InputLayout* SkyBox;
	static ID3D11InputLayout* Skin;
	static ID3D11InputLayout* Sprite2D;
};

#endif // VERTEX_H
