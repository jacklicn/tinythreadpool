/*
 *  Project   : TinyThreadPool
 *  File      : Timer.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef TIMER_H_
#define TIMER_H_
#include <stdlib.h>
#include <time.h>

namespace TTP
{

class Timer
{
public:
	Timer();
	virtual ~Timer();
	void start();
	void end();
	static long long getCurrentTime();
	static long long getTimestamp();
	long long elapsedMicroSeconds();
	long long elapsedMilliSeconds();
	long long elapsedNanoSeconds();
	long long elapsedSeconds();
public:
	long long m_elap;
private:
    timespec m_st;
};

} // namespace TTP
#endif /* TIMER_H_ */
