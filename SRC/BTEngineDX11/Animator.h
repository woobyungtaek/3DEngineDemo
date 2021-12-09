#pragma once

using namespace SimpleMath;

class Animation;
// Animation Clip을 재생하는 컴포넌트
class AnimationClip;
class Renderer;

class Animator : public BTBehaviour
{
public:
	Animator();
	~Animator() {}

private:
	// 현재 재생중인 Clip
	AnimationClip* mpCurrentClip;

	// 재생할 수 있는 Clip들
	vector<AnimationClip*> mAnimationClipVec;

	// Frame
	float mFrame;
	int	  mFrameIdx;

	// Delay
	float mDelay;
	float mTempDelay;

	// 재생 관련
	bool mbPlaying;
	bool mbPause;
	bool mbLoop;

public:
	Renderer* mpCurrentRenderer;

public:
	virtual void Start() override;
	virtual void Update(float deltaTime) override;

public:
	void AddAnimationClipByStr(wstring clipName);
	void SetAnimationClipByStr(wstring clipName);
	void SetAnimationClipByIdx(int index);

	void Play(bool isLoop = false);
	void PlayWithDelay(float delay, bool isLoop = false);
	void Pause();
	void Resume();
	void Stop();
};


class TestClass : public BTBehaviour
{
public:
	long long  a1;
	long long  a2;
	long long  a3;
	long long  a4;
	long long  a5;
	long long  a6;
	long long  a7;
	long long  a8;
	long long  a9;
	long long  a10;
	long long  a11;
	long long  a12;
	long long  a13;
	long long  a14;
	long long  a15;
	long long  a16;
	long long  a17;
	long long  a18;
	long long  a19;
	long long  a20;
	long long  a21;
	long long  a22;
	long long  a23;
	long long  a24;
	long long  a25;
	long long  a26;
	long long  a27;
	long long  a28;
	long long  a29;
	long long  a30;
	long long  a31;
	long long  a32;
	long long  a33;
	long long  a34;
	long long  a35;
	long long  a36;
	long long  a37;
	long long  a38;
	long long  a39;
	long long  a41;
	long long  a42;
	long long  a43;
	long long  a44;
	long long  a45;
	long long  a46;
	long long  a47;
	long long  a48;
	long long  a49;
	long long  a50;
	long long  a51;
	long long  a52;
	long long  a53;
	long long  a54;
	long long  a55;
	long long  a56;
	long long  a57;
	long long  a58;
	long long  a59;
	long long  a60;
};