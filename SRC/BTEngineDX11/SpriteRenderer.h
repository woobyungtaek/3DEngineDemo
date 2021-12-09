#pragma once
class Renderer;

class SpriteRenderer : public Renderer
{
public :
	SpriteRenderer();
	virtual ~SpriteRenderer();

public:
	virtual void Render(Camera* pCamera) override;

	void SetSpriteByName(std::wstring dataName);

private:
	ID3D11ShaderResourceView* mTexture;

	Matrix mTexTM;

	Vector2 mScreenSize;
	Vector2 mImageSize;
};

