//***************************************************************************************
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include "DX11Define.h"
#include "d3dx11Effect.h"

class MaterialProperties;

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();
	
private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
	ID3DX11EffectScalarVariable* mLightCount;

public:
	// 인터페이스	 
	virtual	void SetWorldViewProj(CXMMATRIX M)		{}
	virtual	void SetWorld(CXMMATRIX M)				{}
	virtual	void SetWorldInvTranspose(CXMMATRIX M)  {}
	virtual	void SetTexTransform(CXMMATRIX M)		{}
	virtual	void SetEyePosW(const XMFLOAT3& v)		{}

	virtual void SetDirLights(const std::vector<DirectionalLight>& lights)  {}
	virtual void SetBTMaterial(const MaterialProperties& mat)				{}
	virtual void SetDiffuseMap(ID3D11ShaderResourceView* tex)				{}
	virtual void SetNormalMap(ID3D11ShaderResourceView* tex)				{}
	virtual void SetCubeMap(ID3D11ShaderResourceView* tex)					{}
	virtual void SetBoneTMVec(const std::vector<XMFLOAT4X4> boneTMList)		{}

	virtual ID3DX11EffectTechnique* GetTechByTexture(bool bNoDiffuse) { return nullptr; }
};
#pragma endregion


#pragma region BasicEffect

class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	virtual void SetWorldViewProj(CXMMATRIX M)		override { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetWorld(CXMMATRIX M)				override { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetWorldInvTranspose(CXMMATRIX M)  override { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetTexTransform(CXMMATRIX M)		override { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetEyePosW(const XMFLOAT3& v)		override { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	virtual void SetDirLights(const std::vector<DirectionalLight>& lights) override
	{
		int size = (int)lights.size();
		mLightCount->SetInt(size);
		DirLights->SetRawValue(&lights[0], 0, (uint32_t)(lights.size() * sizeof(DirectionalLight)));
	}

	virtual void SetBTMaterial(const MaterialProperties& mat) override
	{
		Mat->SetRawValue(&mat, 0, sizeof(MaterialProperties));
	}

	virtual void SetDiffuseMap(ID3D11ShaderResourceView* tex) override { DiffuseMap->SetResource(tex); }

	virtual ID3DX11EffectTechnique* GetTechByTexture(bool bNoDiffuse)
	{
		// 디퍼드인지 아닌지에 따라 또 나눠서 줘야하네..
		if (bNoDiffuse)
		{
			//return LightTech;
			return DeferredTech;
		}
		//return LightTexTech;
		return DeferredTexTech;
	}

public:
	ID3DX11EffectTechnique* LightTech;
	ID3DX11EffectTechnique* LightTexTech;

	ID3DX11EffectTechnique* DeferredTech;
	ID3DX11EffectTechnique* DeferredTexTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};

class DeferredLightEffect : public Effect
{
public:
	DeferredLightEffect(ID3D11Device* device, const std::wstring& filename);
	~DeferredLightEffect();

	virtual void SetWorldViewProj(CXMMATRIX M)		override { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetEyePosW(const XMFLOAT3& v)		override { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	virtual void SetDirLights(const std::vector<DirectionalLight>& lights) override
	{
		int size = (int)lights.size();
		mLightCount->SetInt(size);
		DirLights->SetRawValue(&lights[0], 0, (uint32_t)(lights.size() * sizeof(DirectionalLight)));
	}

	//virtual void SetBTMaterial(const MaterialProperties& mat) override
	//{
	//	Mat->SetRawValue(&mat, 0, sizeof(MaterialProperties));
	//}

	void SetSRVByIndex(int index, ID3D11ShaderResourceView* tex)
	{
		DeferredRTArr[index]->SetResource(tex);
	}

	// GetTech이긴 하네.
	virtual ID3DX11EffectTechnique* GetTechByTexture(bool bNoDiffuse)
	{
		return LightTech;
	}

public:
	ID3DX11EffectTechnique* LightTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DeferredRTArr[3] = { nullptr, };
};
class NormalEffect : public Effect
{
public:
	NormalEffect(ID3D11Device* device, const std::wstring& filename);
	~NormalEffect();

	virtual void SetWorldViewProj(CXMMATRIX M)		override { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetWorld(CXMMATRIX M)				override { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetWorldInvTranspose(CXMMATRIX M)	override { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetTexTransform(CXMMATRIX M)		override { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetEyePosW(const XMFLOAT3& v)		override { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	
	virtual void SetDirLights(const std::vector<DirectionalLight>& lights) override 
	{
		int size = (int)lights.size();
		mLightCount->SetInt(size);
		DirLights->SetRawValue(&lights[0], 0, (uint32_t)(lights.size() * sizeof(DirectionalLight)));
	}
	virtual void SetBTMaterial(const MaterialProperties& mat) override
	{
		Mat->SetRawValue(&mat, 0, sizeof(MaterialProperties));
	}

	virtual void SetDiffuseMap(ID3D11ShaderResourceView* tex) override { DiffuseMap->SetResource(tex); }
	virtual void SetNormalMap(ID3D11ShaderResourceView* tex)  override { NormalMap->SetResource(tex); }

	virtual ID3DX11EffectTechnique* GetTechByTexture(bool bNoDiffuse)
	{
		if (bNoDiffuse)
		{
			return LightTech;
		}
		return LightTexTech;
	}

public:
	ID3DX11EffectTechnique* LightTech;

	ID3DX11EffectTechnique* LightTexTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};

class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	virtual void SetWorldViewProj(CXMMATRIX M)				override { mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetCubeMap(ID3D11ShaderResourceView* tex)  override { mCubeMap->SetResource(tex); }

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3DX11EffectShaderResourceVariable* mCubeMap;
};

class ColorEffect : public Effect
{
public:
	ColorEffect(ID3D11Device* device, const std::wstring& filename);
	~ColorEffect();

	virtual void SetWorldViewProj(CXMMATRIX M) override { mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
};

class WireEffect : public Effect
{
public:
	WireEffect(ID3D11Device* device, const std::wstring& filename);
	~WireEffect();

	virtual void SetWorldViewProj(CXMMATRIX M) override { mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
};

class SkinEffect : public Effect
{
public:
	SkinEffect(ID3D11Device* device, const std::wstring& filename);
	~SkinEffect();

	virtual void SetWorldViewProj(CXMMATRIX M)		override { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetWorld(CXMMATRIX M)				override { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetWorldInvTranspose(CXMMATRIX M)	override { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetTexTransform(CXMMATRIX M)		override { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetEyePosW(const XMFLOAT3& v)		override { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	virtual void SetDirLights(const std::vector<DirectionalLight>& lights) override
	{
		int size = (int)lights.size();
		mLightCount->SetInt(size);
		DirLights->SetRawValue(&lights[0], 0, (uint32_t)(lights.size() * sizeof(DirectionalLight)));
	}
	virtual void SetBTMaterial(const MaterialProperties& mat) override
	{
		Mat->SetRawValue(&mat, 0, sizeof(MaterialProperties));
	}

	virtual void SetDiffuseMap(ID3D11ShaderResourceView* tex) override { DiffuseMap->SetResource(tex); }
	virtual void SetNormalMap(ID3D11ShaderResourceView* tex)  override { NormalMap->SetResource(tex); }

	virtual void SetBoneTMVec(const std::vector<XMFLOAT4X4> boneTMList) 
	{
		gBoneTMVec->SetMatrixArray(reinterpret_cast<const float*>(&boneTMList[0].m), 0, (uint32_t)boneTMList.size());
	}

	virtual ID3DX11EffectTechnique* GetTechByTexture(bool bNoDiffuse)
	{
		// 요기서 또 나눠누어 줘야함
		if (bNoDiffuse)
		{
			return LightTech;
		}
		return LightTexTech;
	}

public:
	ID3DX11EffectTechnique* LightTech;
	ID3DX11EffectTechnique* LightTexTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectMatrixVariable* gBoneTMVec;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};

class SpriteEffect : Effect
{
public:
	SpriteEffect(ID3D11Device* device, const std::wstring& filename);
	~SpriteEffect();

	virtual void SetWorldViewProj(CXMMATRIX M) override { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	virtual void SetDiffuseMap(ID3D11ShaderResourceView* tex) override { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* mTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};

#pragma endregion


// 통합관리 용 static
#pragma region Effects

class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;
	static NormalEffect* NormalFX;
	static SkyEffect* SkyFX;
	static ColorEffect* ColorFX;
	static SkinEffect* SkinFX;
	static SpriteEffect* Sprite2DFX;
	static DeferredLightEffect* DeferredLightFX;
	//static WireEffect*  WireFX;
};

#pragma endregion

#endif // EFFECTS_H