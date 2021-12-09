#pragma once
class Renderer;

class MeshRenderer : public Renderer
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

public:
	virtual void Render(Camera* pCamera) override;

private:
	ID3D11ShaderResourceView*   mTexture;	// 텍스쳐
	ID3D11ShaderResourceView*   mNormalMap; // 노멀맵

	Matrix mTexTransform; 

	bool isNormalMap;
};

