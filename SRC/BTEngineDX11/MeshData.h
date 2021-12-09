#pragma once
/*
	VertexTex로 저장 > 가장 큰 형태
	VB, IB를 만들때 필요한 부분만 쓴
*/
using namespace BTStruct;

template<typename T>
class MeshData
{
public:
	vector<T>	 mVertices;
	vector<UINT> mIndices;

	bool mbSkin;

	int mVertexCount;
	int mFaceCount;
	int mIndexCount;

	int mMaterialRef;

	UINT mStride;

public:
	int GetByteWidth_VB()
	{
		return mStride * mVertexCount;
	}

	int GetByteWidth_IB()
	{
		return sizeof(int) * mIndexCount;
	}
};

