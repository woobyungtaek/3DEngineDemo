#pragma once

class MaterialProperties;

class BTMaterial
{
public:
	BTMaterial();
	~BTMaterial();

public:
	int mMatNum;

	wstring mDiffuseName;
	wstring mNormalMapName;

	Effect* mEffect;

	MaterialProperties* mProperties;

	ID3D11ShaderResourceView* mDiffuseMap;
	ID3D11ShaderResourceView* mNormalMap;
};

