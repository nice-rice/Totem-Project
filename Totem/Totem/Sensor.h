#pragma once
#include <wiringPi.h>
#include <string.h>
#include <stdio.h>

#define MODE_SAMPLE 30
#define NUMBER_SENSORS 1
#define SENSOR_0 1
#define SENSOR_1 4
#define SENSOR_2 5
#define SENSOR_3 6

class CSensor{
public:
	CSensor();
	~CSensor();

	void Initialise(int pin);
	bool Update(double m_dt);
	bool GetSensor();
private:
	bool GetMode(int sample);

private:
	int m_iPin;
	int m_iValue;
	int m_iMode;
	int SampleList[MODE_SAMPLE]; //Sample of sensor readings
	bool m_bPressed;
	bool m_bActive;

};