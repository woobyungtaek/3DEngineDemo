//=============================================================================
// Normal.fx 
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
// 를 기반으로 새로 추가함
//=============================================================================

#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
}; 

cbuffer cbSkinned
{
    float4x4 gBoneTMs[96];
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gNormalMap;
int gLightCount;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samLinear
{
    Filter = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct SkinnedVertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
    float3 Weights : WEIGHTS;
    uint4  BoneIndices : BONEINDICES;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct VertexOutTan
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 Tex : TEXCOORD;
};

VertexOut VS(SkinnedVertexIn vin)
{
	VertexOut vout;
    
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.Weights.x;
    weights[1] = vin.Weights.y;
    weights[2] = vin.Weights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTMs[vin.BoneIndices[i]]).xyz;
        normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTMs[vin.BoneIndices[i]]);
    }
    
    vout.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(normalL, (float3x3) gWorldInvTranspose);
		
    vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}
 
float4 PS(VertexOut pin, uniform bool gUseTexure) : SV_Target
{
    pin.NormalW = normalize(pin.NormalW);
    
    float3 toEye = gEyePosW - pin.PosW;
    
    float distToEye = length(toEye);
    
    toEye /= distToEye;
	
    float4 texColor = float4(1, 1, 1, 1);
    if (gUseTexure)
    {
        texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    }
	 
    float4 litColor = texColor;
    if (gLightCount > 0)
    {
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
		[unroll]
        for (int i = 0; i < gLightCount; ++i)
        {
            float4 A, D, S;
            ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye,
				A, D, S);

            ambient += A;
            diffuse += D;
            spec += S;
        }
        litColor = texColor * (ambient + diffuse) + spec;
    }
    litColor.a = gMaterial.Diffuse.a * texColor.a;
	
    //litColor.rgb = pin.NormalW.rgb;
	
    return litColor;
}

// Tan과 일반으로 또 나눠져야함
technique11 Light
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false) ) );
    }
}

technique11 LightTex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true) ) );
    }
}