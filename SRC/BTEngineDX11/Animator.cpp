#include "pch.h"
#include "Animator.h"

Animator::Animator() :
	mFrame(0.0f), mFrameIdx(0),
	mDelay(0.0f), mTempDelay(0.0f),
	mbPlaying(false), mbPause(false), mbLoop(false),
	mpCurrentRenderer(nullptr), mpCurrentClip(nullptr)

{
}

void Animator::Start()
{
}

void Animator::Update(float deltaTime)
{
	if (mbPlaying == false) { return; }	// 재생 상태가 아님 

	if (mDelay > 0)	// 대기시간이 존재
	{
		mTempDelay += deltaTime;
		if (mTempDelay < mDelay) { return; }
	}

	if (mpCurrentClip == nullptr) { return; } // 클립이 없음
	if (mpCurrentRenderer == nullptr)		  // 렌더러가 없음
	{
		mpCurrentRenderer = gameObject->GetComponent<Renderer>();
		if (mpCurrentRenderer == nullptr) { return; }
	}

	// 정지 상태가 아닐 때 Frame 계산
	if (mbPause == false) 
	{
		// Frame / FrameIdx 계산
		mFrame += (deltaTime * 1000);
		if (mFrame > mpCurrentClip->mSceneData.m_framespeed)
		{
			mFrame = 0;
			mFrameIdx += 1;

			if (mFrameIdx > mpCurrentClip->mSceneData.m_lastframe)
			{
				if (mbLoop)
				{
					// 반복 재생
					mFrameIdx = 0;
				}
				else
				{
					// 애니메이션 끝남
					Stop();
				}
			}
		}
	}

	// tickFrame 계산
	int tickFrame = mFrameIdx * mpCurrentClip->mSceneData.m_ticksperframe;

	// MeshData의 LocalTM을 이용해 실 사용 TM을 갱신한다.
	ModelDataSet* meshDatas = mpCurrentRenderer->mModelDataSet;

	int count = (int)meshDatas->mModelDataVec.size();
	for (int index = 0; index < count; ++index)
	{
		// 갱신 할 MeshData
		ModelData* instMeshData = meshDatas->mModelDataVec[index];

		// 적용할 AniData 검색
		if (mpCurrentClip->mAnimationMap.find(instMeshData->mNodeName)
			== mpCurrentClip->mAnimationMap.end())
		{
			continue;
		}

		// Ani데이터와 Renderer에 실사용 Matrix까지 있으므로 애니메이션 진행
		Animation* instAniData = mpCurrentClip->mAnimationMap[instMeshData->mNodeName];

		instMeshData->SetAnimationMatrix(instAniData, tickFrame);
	}

	// 렌더러의 WorldMatrix를 갱신
	meshDatas->UpdateWorldMatrix();
}

// Clip 추가

void Animator::AddAnimationClipByStr(wstring clipName)
{
	AnimationClip* animationClip = ResourceManager::GetInstance()->GetAniClipData(clipName);

	mAnimationClipVec.push_back(animationClip);
}


// Clip 선택 

void Animator::SetAnimationClipByStr(wstring clipName)
{
	int count = (int)mAnimationClipVec.size();
	for (int index = 0; index < count; ++index)
	{
		if (clipName.compare(mAnimationClipVec[index]->mClipName))
		{
			mpCurrentClip = mAnimationClipVec[index];
			return;
		}
	}

	mpCurrentClip = nullptr;
}

void Animator::SetAnimationClipByIdx(int index)
{
	int count = (int)mAnimationClipVec.size();
	if (index > count && index < 0)
	{
		mpCurrentClip = nullptr;
		return;
	}

	mpCurrentClip = mAnimationClipVec[index];
}


// 재생

void Animator::Play(bool isLoop)
{
	if (mbPlaying == true) { return; }
	mbLoop = isLoop;

	mFrame = 0.0f;
	mFrameIdx = 0;

	mTempDelay = 0.0f;

	mbPlaying = true;
	mbPause = false;
}

void Animator::PlayWithDelay(float delay, bool isLoop)
{
	if (mbPlaying == true) { return; }
	Play(isLoop);

	mDelay = delay;
}

void Animator::Pause()
{
	if (mbPlaying == false) { return; }
	mbPause = true;
}

void Animator::Resume()
{
	mbPause = false;
}

void Animator::Stop()
{
	mbPlaying = false;
	mbPause = false;
}

