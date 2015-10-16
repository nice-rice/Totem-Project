#pragma once
#include "Totem.h"

#include "Connection.h"
#include "Sensor.h"
#include "Timer.h"
#include "LED.h"

Totem::Totem(): m_dt(0.0f), elapsedTime(0), state(0){
	
	m_pTimer = NULL;
	m_pConn = NULL;
	m_pLED = NULL;

}
Totem::~Totem(){
	
	delete m_pLED;
	delete m_pConn;
	delete m_pTimer;

	for(SensorList::iterator it = m_vSensorList.begin(); it != m_vSensorList.end(); ++it){
		delete *it;
	}

}

void Totem::Initialise(){

	m_pTimer = new CTimer;
	m_pConn = new CConnection;
	m_pLED = new CLED;

	int SensorPinList[4] = {SENSOR_0, SENSOR_1, SENSOR_2, SENSOR_3};

	for(unsigned int i = 0; i < NUMBER_SENSORS; ++i){
		CSensor* sensor;
		sensor = new CSensor;
		sensor->Initialise(SensorPinList[i]);
		m_vSensorList.push_back(sensor);
	}


	m_pConn->Intialise();
	
	wiringPiSetup();
	for(int i =0; i < NUMBER_SENSORS; ++i){
		
	}
	m_pLED->Initialise();
	m_pLED->Update(state);


}

void Totem::Update(){
	int temp = 0;
	unsigned int i = 0;
	for(SensorList::iterator it = m_vSensorList.begin(); it != m_vSensorList.end(); ++it){
		led_state[i] = (*it)->Update(m_dt);
		if(led_state[i])
			temp += pow(2,i);
		++i;
	}

	if(temp != state){
		
		state = temp;
		m_pLED->Update(state);
	}
		

	usleep(1000);
}

int Totem::Execute(){
	
	Initialise();
	while(1){
		m_pTimer->Start();
		Update();
		m_dt = m_pTimer->Elapsed();
	}
	return 0;
}
Totem& Totem::GetInstance(){
	static Totem instance;
	return instance;
}
int main(int argc, char *argv[])
{
	Totem &totem = Totem::GetInstance();
	return totem.Execute();
}