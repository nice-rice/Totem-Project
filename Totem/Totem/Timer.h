#pragma once
#include <time.h>
#include <math.h>


#define SCALE 1000000

class CTimer 
{
public:
	CTimer();
	~CTimer();
	void Start();
	double Elapsed();


private:

	bool m_bStarted;
	double dt;
	struct timespec start, end, frq;
};