/*
	디퍼드를 하기위한 단계
	1. 포지션, 노말, 알베도 3개 우선으로하기
	2. 렌더러가 완젼 하나로 뭉쳐져 있네..  현재상태 forward로 만들고 deferred 따로
	3. 쉐이더도 한개로 뭉쳐서 만들자 일단. vs, ps가 하나로된 deferred 쉐이더 하나 만들기
	4. 라이트 쉐이더도 따로 만들기 ( 마지막에 그리는 (빛연산) 쉐이더가 각각 필요 )
*/

#include "pch.h"

D3D11Render::D3D11Render()
{
}

D3D11Render::~D3D11Render()
{
	Effects::DestroyAll();
}

bool D3D11Render::Init(int sWidth, int sHeight, bool vsync, HWND hWnd, bool fullscreen, float sDepth, float sNear)
{
	mhWnd = hWnd;

	mbVSync = vsync;

	mScreenWidth = sWidth;
	mScreenHeight = sHeight;

	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
	{
		return false;
	}

	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
	{
		return false;
	}

	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
	{
		return false;
	}

	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
	{
		return false;
	}

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
	{
		return false;
	}

	unsigned int numerator = 0;
	unsigned int denomiator = 0;

	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)mScreenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)mScreenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denomiator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	mVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, mVideoCardDesc, 128, adapterDesc.Description, 128) != 0)
	{
		return false;
	}

	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;

	//스왑체인
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = mScreenWidth;
	sd.BufferDesc.Height = mScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	if (mbVSync)
	{
		sd.BufferDesc.RefreshRate.Numerator = numerator;
		sd.BufferDesc.RefreshRate.Denominator = denomiator;
	}
	else
	{
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;

	sd.Windowed = fullscreen;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
		&featureLevel, 1, D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, NULL, &mDeviceContext)))
	{
		return false;
	}

	HR(mDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer)))
	{
		return false;
	}
	if (FAILED(mDevice->CreateRenderTargetView(backBuffer, NULL, &mRenderTargetView)))
	{
		return false;
	}

	backBuffer->Release();
	backBuffer = 0;

	//깊이 스텐실 버퍼
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = mScreenWidth;
	depthBufferDesc.Height = mScreenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if (FAILED(mDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer)))
	{
		return false;
	}

	//깊이 스텐실
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	if (FAILED(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState)))
	{
		return false;
	}
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView)))
	{
		return false;
	}

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	//래스터 라이저
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(mDevice->CreateRasterizerState(&rasterDesc, &mRasterState)))
	{
		return false;
	}

	mDeviceContext->RSSetState(mRasterState);

	// 뷰포트
	D3D11_VIEWPORT vp;
	vp.Width = (float)mScreenWidth;
	vp.Height = (float)mScreenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;

	mDeviceContext->RSSetViewports(1, &vp);

	mSwapChain->SetFullscreenState(fullscreen, NULL);

	mWorld = Matrix::Identity;
	mView = Matrix::Identity;
	mProj = Matrix::Identity;

	OnResize(mScreenWidth, mScreenHeight);

	Effects::InitAll(mDevice);
	InputLayouts::InitAll(mDevice);

	CreateRenderState();
	
	return true;
}

void D3D11Render::Release()
{
	if (mSwapChain) { mSwapChain->SetFullscreenState(false, NULL); }
	if (mRasterState) { mRasterState->Release();		  mRasterState = 0; }
	if (mDepthStencilView) { mDepthStencilView->Release();   mDepthStencilView = 0; }
	if (mDepthStencilState) { mDepthStencilState->Release();  mDepthStencilState = 0; }
	if (mDepthStencilBuffer) { mDepthStencilBuffer->Release(); mDepthStencilBuffer = 0; }
	if (mRenderTargetView) { mRenderTargetView->Release();   mRenderTargetView = 0; }
	if (mDeviceContext) { mDeviceContext->Release();	  mDeviceContext = 0; }
	if (mDevice) { mDevice->Release();			  mDevice = 0; }
	if (mSwapChain) { mSwapChain->Release();		  mSwapChain = 0; }
}

void D3D11Render::BeginRender(float r, float g, float b, float a)
{
	// 바인딩 부분들을 다 비긴으로 빼
	// 왜냐하면 패스마다 스테이트가 바뀌기 때문에

	const float color[4] = { r,g,b,a };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	
}

// 라이트 패스를 타야한다.
// >>


// 포스트 프로세싱을 먼저해서 랜더타겟 늘리는것 해보고
// 그 다음 디퍼드를 해보자

void D3D11Render::EndRender()
{
	if (mbVSync)
	{
		mSwapChain->Present(1, 0);
	}
	else
	{
		mSwapChain->Present(0, 0);
	}
}

void D3D11Render::BeginSceneRender()
{
	mWorld = Matrix::Identity;
	mView = Matrix::Identity;
	mProj = Matrix::Identity;

	// Set constants
	Matrix world = mWorld;
	Matrix view  = mView;
	Matrix proj  = mProj;

	mDeviceContext->OMSetDepthStencilState(NormalDSS, 0);
}

void D3D11Render::EndSceneRender()
{
	mDeviceContext->RSSetState(0);
}

void D3D11Render::OnResize(int sWidth, int sHeight)
{
	assert(mDeviceContext);
	assert(mDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);

	mScreenWidth = sWidth;
	mScreenHeight = sHeight;

	// Resize the swap chain and recreate the render target view.

	HR(mSwapChain->ResizeBuffers(1, sWidth, sHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer = nullptr;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(mDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = sWidth;
	depthStencilDesc.Height = sHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(mDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(mDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.
	/// 렌더타겟뷰, 뎁스/스탠실뷰를 파이프라인에 바인딩한다.

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);


	// Set the viewport transform.
	/// 뷰포트 변환을 셋팅한다.

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(sWidth);
	mScreenViewport.Height = static_cast<float>(sHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mDeviceContext->RSSetViewports(1, &mScreenViewport);
}


#pragma region Get & Set

HWND D3D11Render::GetHWnd()
{
	return mhWnd;
}

ID3D11Device* D3D11Render::GetDevice()
{
	return mDevice;
}

ID3D11DeviceContext* D3D11Render::GetDeviceContext()
{
	return mDeviceContext;
}

ID3D11RasterizerState* D3D11Render::GetRasterState()
{
	return mRasterState;
}

ID3D11RasterizerState* D3D11Render::GetWireframeRS()
{
	return mWireframeRS;
}

ID3D11RasterizerState* D3D11Render::GetSolidRS()
{
	return mSolidRS;
}

ID3D11DepthStencilState* D3D11Render::GetNormalDSS()
{
	return NormalDSS;
}

ID3D11DepthStencilState* D3D11Render::GetDS()
{
	return mDepthStencilState;
}
ID3D11DepthStencilState* D3D11Render::GetDisableDS()
{
	return mDepthDisabledStencilState;
}
Vector2 D3D11Render::GetScreenSize()
{
	return Vector2(mScreenWidth, mScreenHeight);
}
#pragma endregion


void D3D11Render::CreateRenderState()
{
	D3D11_RASTERIZER_DESC solidDesc;
	ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidDesc.FillMode = D3D11_FILL_SOLID;
	solidDesc.CullMode = D3D11_CULL_BACK;
	solidDesc.FrontCounterClockwise = false;
	solidDesc.DepthClipEnable = true;

	HR(mDevice->CreateRasterizerState(&solidDesc, &mSolidRS));

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(mDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));

	//Font
	D3D11_DEPTH_STENCIL_DESC equalsDesc;
	ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalsDesc.DepthEnable = true;
	equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	equalsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	HR(mDevice->CreateDepthStencilState(&equalsDesc, &NormalDSS));

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(mDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &mDepthDisabledStencilState));

}

