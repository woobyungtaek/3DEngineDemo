#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();

public:
	float GameTime() const;
	float DeltaTime() const;
	float TotalTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double mSecondsPerCount;
	double mDeltaTIme;

	__int64 mBaseTime;
	__int64 mPauseTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

