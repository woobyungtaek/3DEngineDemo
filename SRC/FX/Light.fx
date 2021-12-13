#include "LightHelper.fx"

cbuffer cbPerFrame
{
    DirectionalLight gDirLights[3];
    float3 gEyePosW;

    float gFogStart;
    float gFogRange;
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

// 사용할 텍스쳐들 즉 랜더 타겟들이 필요
Texture2D AlbedoTex;
Texture2D PositionTex;
Texture2D NormalTex;

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	// 평면상의 데이터만 들어와 있으므로 VS는 결국 그냥 그대로 반환
    VertexOut vout;

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.Tex = vin.Tex;
    return vout;
}

// Lighting PS
float4 PS(VertexOut pin, uniform bool gUseTexure) : SV_Target
{
    
	// The toEye vector is used in lighting.
    float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
    float distToEye = length(toEye);

	// Normalize.
    toEye /= distToEye;
    
    
    float4 litColor = texColor;
    if (gLightCount > 0)
    {
	    // Start with a sum of zero. 
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	    // Sum the light contribution from each light source.  
	    [unroll]
        for (int i = 0; i < gLightCount; ++i)
        {
            float4 A, D, S;
            ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye, A, D, S);

            ambient += A;
            diffuse += D;
            spec += S;
        }

	// Modulate with late add.
        litColor = texColor * (ambient + diffuse) + spec;
    }

// Common to take alpha from diffuse material and texture.
    litColor.a = gMaterial.Diffuse.a * texColor.a;
	
    return litColor;
}
technique11 Light
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(false)));
    }
}

technique11 LightTex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }
}