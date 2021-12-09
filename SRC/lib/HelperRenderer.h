#pragma once
class Renderer;

class HelperRenderer : public Renderer
{
public:
	HelperRenderer();
	virtual ~HelperRenderer();

public:
	virtual void Render(Camera* pCamera) override;
};

