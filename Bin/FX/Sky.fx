//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// 하늘 돔 렌더링에 쓰이는 효과
//=============================================================================

#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
    float4x4 gWorldViewProj;
};

TextureCube gCubeMap;
int         gLightCount;

SamplerState samTriLinearSam
{
    Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosL    : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// z/w = 이 되도록(즉 하늘 돔이 항상 먼 평면에 있도록) z = w 로 설정한다.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj ).xyww;
    vout.PosL = vin.PosL;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return gCubeMap.Sample(samTriLinearSam, pin.PosL);
}

RasterizerState NoCull
{
    CullMode = None;
};

DepthStencilState LessEqualDSS
{
    // 깊이 함수를 그냥 LESS_EQUAL로 해야 한다.
    // 그렇게 하지 않으면, 깊이 버퍼를 1로 지웠다고 할 때 z = 1 (NDC)의
    // 정규화된 깊이 값들이 깊이 판정에 실패한다.
    DepthFunc = LESS_EQUAL;
};

technique11 SkyTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
        
        SetRasterizerState(NoCull);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}