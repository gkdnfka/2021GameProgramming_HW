#pragma once

#include "stdafx.h"

// Base Animation Class

template <typename T>
class Animation
{
public:
	Animation(T start, T end, T duration) :
		m_Start(start),
		m_End(end),
		m_Duration(duration)
	{
	}

	void SetStart(T start)
	{
		m_Start = start;
	}

	T GetStart()
	{
		return m_Start;
	}

	void SetEnd(T end)
	{
		m_End = end;
	}

	T GetEnd()
	{
		return m_End;
	}

	void SetDuration(T duration)
	{
		m_Duration = max(0, duration);
	}

	T GetDuration()
	{
		return m_Duration;
	}

	T GetValue(T time)
	{
		time = min(max(time, 0), m_Duration);
		return ComputeValue(time);
	}

protected:
	virtual T ComputeValue(T time) = 0;

	T m_Duration;
	T m_Start;
	T m_End;
};

template <typename T>
class MeteorAnimation : public Animation<T> {
public:
	MeteorAnimation(T start = 0, T end = 0, T duration = 0) :
		Animation<T>(start, end, duration) {
	}

protected:
	T ComputeValue(T time) {
		return (T)(this->GetStart() + (this->GetEnd() - this->GetStart()) * (time / this->GetDuration()));
	}
};