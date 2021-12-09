#pragma once
class ModelData;

class ModelDataSet
{
public:
	ModelDataSet();
	~ModelDataSet();

public:
	vector<ModelData*> mModelDataVec;

	bool IsWorldTMUpdated;

public:
	ModelDataSet* GetCopy();

	void SetParentNode();
	void SetBonesInfo();
	void AddModelData(wstring meshName, ModelData* data);

	void UpdateLocalMatrix();
	void UpdateWorldMatrix();

	void CreateFinalBoneTM();

	// 애니메이션
	void UpdateUseTMByLocalTM();
};