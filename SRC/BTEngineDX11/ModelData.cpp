#include "pch.h"
#include "ModelData.h"

ModelData::ModelData() :
	mbNoBuffer(false),
	VB(nullptr), IB(nullptr),
	IndexCount(0), Stride(0),
	mParentMeshData(nullptr)
{
}

ModelData::~ModelData()
{
}

ModelData* ModelData::GetCopy()
{
	ModelData* copyData  = new ModelData();

	copyData->mbNoBuffer = mbNoBuffer;
	copyData->mbSkin = mbSkin;

	copyData->mNodeName		  = mNodeName;
	copyData->mParentNodeName = mParentNodeName;

	copyData->VB = VB;
	copyData->IB = IB;

	copyData->VertexCount	= VertexCount;
	copyData->FaceCount		= FaceCount;
	copyData->IndexCount	= IndexCount;
	copyData->Stride		= Stride;

	copyData->mLocalMatrix = mLocalMatrix;
	copyData->mNodeMatrix = mNodeMatrix;

	copyData->mBoneList = mBoneList;

	copyData->mMaterial = mMaterial;

	return copyData;
}

// TM 설정
void ModelData::SetLocalMatrix()
{
	if (mParentMeshData != nullptr)
	{
		mLocalMatrix = mWorldMatrix * mParentMeshData->GetInvertWorldMatrix();
	}
	else
	{
		mLocalMatrix = mWorldMatrix;
	}
}

void ModelData::SetAnimationMatrix(Animation* aniData, int tickFrame)
{
	mUseMatrix = mLocalMatrix;

	// 필요 지역 변수 선언 
	int currentIdx;
	int nextIdx;

	Vector3		s;
	Quaternion	r;
	Vector3		t;

	Vector3		cPos;
	Quaternion	cRot;

	// 요소 분해
	mUseMatrix.Decompose(s, r, t);

#pragma region 포지션 구하기
	if (aniData->m_position.size() > 0)
	{
		// 인덱스는 0부터 시작하지만 기본적으로 1을 빼주고 사용할 것이기 때문에
		currentIdx = -1;
		nextIdx = -1;

		for (int index = 0; index < aniData->m_position.size(); ++index)
		{
			// 현재 틱프레임이 애니메이션 키프레임 보다 작으면
			if (tickFrame < aniData->m_position[index]->m_time)
			{
				currentIdx = index - 1;
				nextIdx = index;
				break;
			}
		}

		// 초기값이 -1이라 변경이 없는 경우 ( 변경이 없다 = 가장 마지막 프레임보다 크다 )
		if (nextIdx + currentIdx < 0)
		{
			currentIdx = (int)aniData->m_position.size() - 1;
			cPos = aniData->m_position[currentIdx]->m_pos;
		}
		else
		{
			//TRACE("Cur : %d / Next : %d \n", currentIdx, nextIdx);
			// 보간시 0 부터 1사이값 구해야함
			float cTime = (float)aniData->m_position[currentIdx]->m_time;
			float nTime = (float)aniData->m_position[nextIdx]->m_time;

			// (분모)현재 - 인근 / (분자) 인근 - 다음 	
			float lerpVal = ((float)tickFrame - cTime) / (nTime - cTime);

			// lerp로 보간
			cPos = Vector3::Lerp(
				aniData->m_position[currentIdx]->m_pos,
				aniData->m_position[nextIdx]->m_pos,
				lerpVal);
		}

		t = cPos;
	}
#pragma endregion

#pragma region 회전 구하기
	if (aniData->m_rotation.size() > 0)
	{
		currentIdx = -1;
		nextIdx = -1;
		for (int index = 0; index < aniData->m_rotation.size(); index++)
		{
			// 현재 틱프레임이 애니메이션 키프레임 보다 작으면
			if (tickFrame < aniData->m_rotation[index]->m_time)
			{
				currentIdx = index - 1;
				nextIdx = index;
				break;
			}
			// 보간시 0 부터 1사이값 구해야함
			// (분모)현재 - 2880 (분자) 3880 - 2880 > 현재랑 다음 프레임으로 계산해야함 
			// Slerp로 보간
			// 미리 로테이션 값을 누적해서 가지고 있으면 대체
			// 아니라면 누적 (곱하기)
		}
		if (nextIdx + currentIdx < 0)
		{
			currentIdx = (int)aniData->m_rotation.size() - 1;
			cRot = aniData->m_rotation[currentIdx]->m_rotQT_accumulation;
		}
		else
		{
			// 보간시 0 부터 1사이값 구해야함
			float cTime = (float)aniData->m_rotation[currentIdx]->m_time;
			float nTime = (float)aniData->m_rotation[nextIdx]->m_time;
			// (분모)현재 - 인근 / (분자) 인근 - 다음 	
			float lerpVal = ((float)tickFrame - cTime) / (nTime - cTime);
			// lerp로 보간
			Quaternion::Slerp(
				aniData->m_rotation[currentIdx]->m_rotQT_accumulation,
				aniData->m_rotation[nextIdx]->m_rotQT_accumulation,
				lerpVal, cRot);
		}
		r = cRot;
	}
#pragma endregion

	mUseMatrix = Matrix::CreateScale(s) * Matrix::CreateFromQuaternion(r) * Matrix::CreateTranslation(t);
}

void ModelData::SetBoneMatrix()
{
	mFinalBoneTMVec.clear();
	Bone* tempBone;

	for (int index = 0; index < (int)mBoneList.size(); ++index)
	{
		tempBone = &mBoneList[index];
		Matrix boneWorldTM = *tempBone->m_boneTM_WorldTM;	//애니메이션 이후 WorldTM
		Matrix boneNodeTM  = *tempBone->m_boneTM_NodeTM;	//처음 NodeTM

		Matrix skinWorldTM = XMLoadFloat4x4(&mWorldMatrix);
		Matrix skinWorldTM_Inverse = XMMatrixInverse(nullptr, skinWorldTM);

		Matrix boneOffsetTM = boneNodeTM * skinWorldTM_Inverse;
		Matrix boneOffsetTM_Inverse = boneOffsetTM.Invert();

		Matrix finalBoneTM = boneOffsetTM_Inverse * boneWorldTM;

		XMFLOAT4X4 finalTM;
		XMStoreFloat4x4(&finalTM, finalBoneTM);
		mFinalBoneTMVec.push_back(finalBoneTM);
	}
}

// TM 얻기
Matrix ModelData::GetInvertWorldMatrix()
{
	return mWorldMatrix.Invert();
}

Matrix ModelData::GetWorldMatrix()
{
	if (mParentMeshData == nullptr)
	{
		return mLocalMatrix;
	}
	return mLocalMatrix * (mParentMeshData->GetWorldMatrix());
}

Matrix ModelData::GetUseMatrixTM()
{
	if (mParentMeshData == nullptr)
	{
		return mUseMatrix;
	}
	return mUseMatrix * mParentMeshData->GetUseMatrixTM();
}

void ModelData::SetEffectAndInputLayout(Effect** refEffect, ID3D11InputLayout** refInputLayout)
{
	if (mBoneList.size() <= 0)
	{
		if (mMaterial->mNormalMap != nullptr)
		{
			*refInputLayout = InputLayouts::NormalMap;
			*refEffect = Effects::NormalFX;
			return;
		}
	}
	else
	{
		*refInputLayout = InputLayouts::Skin;
		*refEffect = Effects::SkinFX;
		return;
	}

	*refInputLayout = InputLayouts::Basic32;
	*refEffect = Effects::BasicFX;
}
