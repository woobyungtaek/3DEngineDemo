#include "pch.h"
#include "ModelDataSet.h"

ModelDataSet::ModelDataSet() :
	IsWorldTMUpdated(false)
{
}

ModelDataSet::~ModelDataSet()
{
}

ModelDataSet* ModelDataSet::GetCopy()
{
	ModelDataSet* copyData = new ModelDataSet();

	int count = (int)mModelDataVec.size();
	for (int index = 0; index < count; ++index)
	{
		copyData->mModelDataVec.push_back(mModelDataVec[index]->GetCopy());
	}

	copyData->SetParentNode();
	copyData->SetBonesInfo();

	return copyData;
}

void ModelDataSet::SetParentNode()
{
	int meshCount = (int)mModelDataVec.size();
	for (int index = 0; index < meshCount; ++index)
	{
		for (int fIndex = 0; fIndex < meshCount; ++fIndex)
		{
			if (mModelDataVec[index]->mParentNodeName.empty() == true) { continue; }

			int result = mModelDataVec[fIndex]->mNodeName.compare(mModelDataVec[index]->mParentNodeName);
			if (result == 0)
			{
				mModelDataVec[index]->mParentMeshData = mModelDataVec[fIndex];
				break;
			}
		}
	}
}

void ModelDataSet::SetBonesInfo()
{
	// 값이 복사 될때 마다 달라지네?
	int modelCnt = (int)mModelDataVec.size();
	for (int modelIdx = 0; modelIdx < modelCnt; ++modelIdx)
	{
		int boneCnt = (int)mModelDataVec[modelIdx]->mBoneList.size();

		for (int boneIdx = 0; boneIdx < boneCnt; ++boneIdx)
		{
			mModelDataVec[modelIdx]->mBoneList[boneIdx];
			wstring boneName = wstring(
				mModelDataVec[modelIdx]->mBoneList[boneIdx].m_bone_name.begin(), 
				mModelDataVec[modelIdx]->mBoneList[boneIdx].m_bone_name.end());

			for (int secIdx = 0; secIdx < modelCnt; ++secIdx)
			{
				if (mModelDataVec[secIdx]->mNodeName.compare(boneName) != 0) { continue; }
				mModelDataVec[modelIdx]->mBoneList[boneIdx].m_boneTM_NodeTM = &(mModelDataVec[secIdx]->mNodeMatrix);
				mModelDataVec[modelIdx]->mBoneList[boneIdx].m_boneTM_WorldTM = &(mModelDataVec[secIdx]->mWorldMatrix);
				break;
			}
		}
	}
}

void ModelDataSet::AddModelData(wstring meshName, ModelData* data)
{
	mModelDataVec.push_back(data);
}

void ModelDataSet::UpdateLocalMatrix()
{
	for (int index = 0; index < mModelDataVec.size(); ++index)
	{
		mModelDataVec[index]->SetLocalMatrix();
	}
}

void ModelDataSet::UpdateUseTMByLocalTM()
{
	int count = (int)mModelDataVec.size();
	for (int index = 0; index < count; ++index)
	{
		ModelData* instData = mModelDataVec[index];
		instData->mUseMatrix = instData->mLocalMatrix;
	}
}

void ModelDataSet::UpdateWorldMatrix()
{
	IsWorldTMUpdated = true;
	int count = (int)mModelDataVec.size();
	for (int index = 0; index < count; ++index)
	{
		ModelData* instData = mModelDataVec[index];

		if (instData->mParentMeshData == nullptr)
		{
			instData->mWorldMatrix = instData->mUseMatrix;
		}
		else
		{
			instData->mWorldMatrix = instData->GetUseMatrixTM();
		}
	}
}

void ModelDataSet::CreateFinalBoneTM()
{
	int count = (int)mModelDataVec.size();
	for (int index = 0; index < count; ++index)
	{
		if (!mModelDataVec[index]->mbSkin) { continue; }
		mModelDataVec[index]->SetBoneMatrix();
	}
}