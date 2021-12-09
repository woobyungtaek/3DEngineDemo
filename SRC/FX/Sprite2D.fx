//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx" 

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
}; 

Texture2D gDiffuseMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 Tex     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj).xyww;
    vout.PosH.w = 1.5f;
    vout.PosH.z = 0;
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = vin.Tex;

	return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
	
    texColor = gDiffuseMap.Sample(samLinear, pin.Tex);
	
    return texColor;
}

technique11 Sprite2D
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
