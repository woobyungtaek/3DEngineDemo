
#include "pch.h"
#include "HelperRenderer.h"

HelperRenderer::HelperRenderer()
{
	D3D11Render* instRender = BTGameEngine::GetInstance()->GetRender();
	mDeviceContext = instRender->GetDeviceContext();
	mpRenderState = instRender->GetWireframeRS();

	mPrimitiveTopo = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	mInputLayout = InputLayouts::ColorWire;
}

HelperRenderer::~HelperRenderer()
{
}

void HelperRenderer::Render(Camera* pCamera)
{
	if (mModelDataSet == nullptr) { return; }

	mModelDataSet->IsWorldTMUpdated = false;
	mOffset = 0;

	mDeviceContext->IASetInputLayout(mInputLayout);
	CommonSetting();

	mWorld = gameObject->mTransform->GetTransformMatrix();
	
	// WVP TM 셋팅
	Matrix worldViewProj = mWorld * mView * mProj;

	// 테크닉은...
	ID3DX11EffectTechnique* tech = Effects::ColorFX->mTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	Effects::ColorFX->SetWorldViewProj(worldViewProj);

	// MeshData의 수만큼 반복해야한다.
	for (int index = 0; index < mModelDataSet->mModelDataVec.size(); ++index)
	{
		if (mModelDataSet->mModelDataVec[index]->mbNoBuffer == true) { continue; }

		SettingBuffers(mModelDataSet->mModelDataVec[index]);

		for (UINT pass = 0; pass < techDesc.Passes; ++pass)
		{
			tech->GetPassByIndex(pass)->Apply(0, mDeviceContext);
			mDeviceContext->DrawIndexed(mModelDataSet->mModelDataVec[index]->IndexCount, 0, 0);
		}
	}
}
