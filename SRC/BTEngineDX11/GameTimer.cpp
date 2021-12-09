#include "windows.h"
#include "GameTimer.h"

GameTimer::GameTimer() :
	mDeltaTIme(-1.0),mBaseTime(0),
	mPauseTime(0), mPrevTime(0), mCurrTime(0),mStopTime(0),mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

GameTimer::~GameTimer()
{
}

float GameTimer::GameTime() const
{
	return 0.0f;
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTIme;
}

float GameTimer::TotalTime() const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPauseTime) - mBaseTime) * mSecondsPerCount);
	}
	
	return (float)(((mCurrTime -mPauseTime) - mBaseTime) * mSecondsPerCount);
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	if (mStopped)
	{
		mPauseTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mStopTime = currTime;
	mStopped = true;
}

void GameTimer::Tick()
{
	if (mStopped) { mDeltaTIme = 0.0; return; }

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTIme = (mCurrTime - mPrevTime) * mSecondsPerCount;

	mPrevTime = mCurrTime;

	if (mDeltaTIme < 0.0) { mDeltaTIme = 0.0; }
}
