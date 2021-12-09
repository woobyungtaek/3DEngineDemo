#include "DXTKFont.h"
#include <tchar.h>

#define SafeDelete(x) { delete x; x = 0; }
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = 0; } }	// 예전 스타일

#include "SimpleMath.h"

DXTKFont::DXTKFont()
	: m_pSpriteBatch(nullptr), m_pSpriteFont(m_pSpriteFont), m_RasterizerState(nullptr), m_DepthStencilState(nullptr)
{

}

DXTKFont::~DXTKFont()
{
	SafeDelete(m_pSpriteFont);
	SafeDelete(m_pSpriteBatch);
}

void DXTKFont::Create(ID3D11Device* pDevice, ID3D11RasterizerState* rs, ID3D11DepthStencilState* ds)
{
	ID3D11DeviceContext* pDC = nullptr;
	pDevice->GetImmediateContext(&pDC);

	m_pSpriteBatch = new DirectX::SpriteBatch(pDC);

	TCHAR* _fileName = (TCHAR*)L"../Font/gulim9k.spritefont";

	m_pSpriteFont = new DirectX::SpriteFont(pDevice, _fileName);
	m_pSpriteFont->SetLineSpacing(14.0f);

	SAFE_RELEASE(pDC);

	m_RasterizerState = rs;
	m_DepthStencilState = ds;
}

void DXTKFont::DrawTextColor(int x, int y, DirectX::XMFLOAT4 color, TCHAR* text, ...)
{
	TCHAR _buffer[1024] = L"";
	va_list vl;
	va_start(vl, text);
	_vstprintf(_buffer, 1024, text, vl);
	va_end(vl);

	m_pSpriteBatch->Begin();
	m_pSpriteFont->DrawString(m_pSpriteBatch, _buffer, DirectX::XMFLOAT2((float)x, (float)y), DirectX::SimpleMath::Vector4(color));
	m_pSpriteBatch->End();
}
