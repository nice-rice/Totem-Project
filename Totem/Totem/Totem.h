#pragma once
#include <vector>

class CTimer;
class CSensor;
class CLED;
class CConnection;

class Totem{
public:
	Totem();
	~Totem();
	int Execute();
	static Totem& GetInstance();

private:
	void Initialise();
	void Update();
	
	CTimer *m_pTimer;
	CConnection *m_pConn;
	CLED *m_pLED;

	typedef std::vector<CSensor*> SensorList;
	SensorList m_vSensorList;

	int led_state[4];
	int state;

	double m_dt;
	long double elapsedTime;

};