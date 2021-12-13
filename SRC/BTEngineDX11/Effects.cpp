//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

// 디퍼드 적용을 위해 다음으로 봐야 할 곳

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region BasicEffect

// 기본( 이란다...)
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	LightTech = mFX->GetTechniqueByName("Light");
	LightTexTech = mFX->GetTechniqueByName("LightTex");

	mLightCount = mFX->GetVariableByName("gLightCount")->AsScalar();
	DirLights = mFX->GetVariableByName("gDirLights");

	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	Mat = mFX->GetVariableByName("gMaterial");

	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();

	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
}

BasicEffect::~BasicEffect()
{
}

SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename) : Effect(device, filename)
{
	mTech = mFX->GetTechniqueByName("SkyTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mCubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect()
{
}


// Normal Effect
NormalEffect::NormalEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	LightTech = mFX->GetTechniqueByName("Light");
	LightTexTech = mFX->GetTechniqueByName("LightTex");

	mLightCount = mFX->GetVariableByName("gLightCount")->AsScalar();
	DirLights = mFX->GetVariableByName("gDirLights");

	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	Mat = mFX->GetVariableByName("gMaterial");

	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();

	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
}

NormalEffect::~NormalEffect()
{
}

// 버텍스 컬러 
ColorEffect::ColorEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

ColorEffect::~ColorEffect()
{
}

// 와이어 프레임만
WireEffect::WireEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

WireEffect::~WireEffect()
{
}

SkinEffect::SkinEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	LightTech = mFX->GetTechniqueByName("Light");
	LightTexTech = mFX->GetTechniqueByName("LightTex");

	mLightCount = mFX->GetVariableByName("gLightCount")->AsScalar();
	DirLights = mFX->GetVariableByName("gDirLights");

	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	Mat = mFX->GetVariableByName("gMaterial");

	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();

	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();

	gBoneTMVec = mFX->GetVariableByName("gBoneTMs")->AsMatrix();
}

SkinEffect::~SkinEffect()
{
}

SpriteEffect::SpriteEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	mTech = mFX->GetTechniqueByName("Sprite2D");

	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

SpriteEffect::~SpriteEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect*	Effects::BasicFX	= 0;
ColorEffect*	Effects::ColorFX	= 0;
NormalEffect*	Effects::NormalFX	= 0;
SkyEffect*		Effects::SkyFX		= 0;
SkinEffect*		Effects::SkinFX		= 0;
SpriteEffect*	Effects::Sprite2DFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX		= new BasicEffect(device, L"../FX/Basic.cso");
	ColorFX		= new ColorEffect(device, L"../FX/color.fxo");
	NormalFX	= new NormalEffect(device, L"../FX/Normal.cso");
	SkyFX		= new SkyEffect(device, L"../FX/Sky.cso");
	SkinFX		= new SkinEffect(device, L"../FX/Skin.cso");
	Sprite2DFX	= new SpriteEffect(device, L"../FX/Sprite2D.cso");

}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(ColorFX);
	SafeDelete(NormalFX);
	SafeDelete(SkyFX);
	SafeDelete(SkinFX);
	SafeDelete(Sprite2DFX);
}

#pragma endregion

