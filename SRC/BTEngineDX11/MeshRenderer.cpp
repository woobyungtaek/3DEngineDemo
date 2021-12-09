#include "pch.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer()
{
	D3D11Render* instRender = BTGameEngine::GetInstance()->GetRender();
	mDeviceContext = instRender->GetDeviceContext();
	mpRenderState = instRender->GetSolidRS();

	mPrimitiveTopo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mInputLayout = InputLayouts::Basic32;
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Render(Camera* pCamera)
{
	if (mModelDataSet == nullptr) { return; }

	mModelDataSet->IsWorldTMUpdated = false;
	mOffset = 0;

	mModelDataSet->CreateFinalBoneTM(); // 맘에는 안들지만 여기서

	mTexTransform = Matrix::Identity;

	CommonSetting();

	ID3DX11EffectTechnique* tech;
	D3DX11_TECHNIQUE_DESC techDesc;

	// MeshData의 수만큼 반복해야한다.
	for (int index = 0; index < mModelDataSet->mModelDataVec.size(); ++index)
	{
		ModelData* modelData = mModelDataSet->mModelDataVec[index];
		if (modelData->mMaterial == nullptr)
		{
			modelData->mMaterial = ResourceManager::GetInstance()->GetMaterial(L"BasicMat", 0);
		}
		BTMaterial* btMat = modelData->mMaterial;

		modelData->SetEffectAndInputLayout(&mEffect, &mInputLayout);

		mDeviceContext->IASetInputLayout(mInputLayout);
		mEffect->SetDirLights(Object::Scene->mDirLightVec);
		mEffect->SetEyePosW(mEyePos);

		mEffect->SetTexTransform(mTexTransform);
		mEffect->SetDiffuseMap(btMat->mDiffuseMap);
		mEffect->SetNormalMap(btMat->mNormalMap);
		
		if (modelData->mbSkin)
		{
			mEffect->SetBoneTMVec(modelData->mFinalBoneTMVec);
		}

		Matrix world = modelData->mWorldMatrix * gameObject->mTransform->GetTransformMatrix();
		Matrix worldViewProj = world * mView * mProj;
		Matrix worldInvTranspose = MathHelper::InverseTranspose(world);

		mEffect->SetWorld(world);
		mEffect->SetWorldInvTranspose(worldInvTranspose);
		mEffect->SetWorldViewProj(worldViewProj);

		if (modelData->mbNoBuffer == true) { continue; }
		mEffect->SetBTMaterial(*modelData->mMaterial->mProperties); // 메테리얼 프로퍼티가 들어가야함

		SettingBuffers(modelData);

		tech = mEffect->GetTechByTexture(btMat->mDiffuseMap == nullptr);
		tech->GetDesc(&techDesc);
		for (UINT pass = 0; pass < techDesc.Passes; ++pass)
		{
			tech->GetPassByIndex(pass)->Apply(0, mDeviceContext);
			mDeviceContext->DrawIndexed(modelData->IndexCount, 0, 0); // Index개수로 들어가야함
		}
	}
}
