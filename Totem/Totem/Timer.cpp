#include "Timer.h"

CTimer::CTimer() : m_bStarted(false){

	start.tv_sec = start.tv_nsec = 0;
	end.tv_sec = end.tv_nsec = 0;
	dt = 0.0;

}
CTimer::~CTimer(){}

void CTimer::Start(){
	m_bStarted = true;
	clock_gettime(CLOCK_MONOTONIC, &start);
}

double CTimer::Elapsed(){

	if(!m_bStarted)
		return 0.0;

	clock_gettime(CLOCK_MONOTONIC, &end);
	long double a  = end.tv_nsec;
	long double b = start.tv_nsec;

	if(a > b){

		dt =  ((a-b)*0.001f);
		long sec = end.tv_sec - start.tv_sec;
		if(sec > 0)
			dt += sec*SCALE;
	
	}
	else{
		
		dt = SCALE-((b-a)*0.001f);
		long sec = end.tv_sec-start.tv_sec-1;
		if(sec > 0)
			dt += sec*SCALE;

		
	}	
	dt *= 0.001f;
	return dt;
}