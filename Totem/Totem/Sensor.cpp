#include "Sensor.h"

CSensor::CSensor(){
	
}
CSensor::~CSensor(){}

void CSensor::Initialise(int pin){
	m_iPin = pin;	
	pinMode(m_iPin, INPUT);
}


bool CSensor::Update(double m_dt){
	
	static double elapsedTime = 0.0f;
	static bool timerStart = false;
	static bool active = false;

	if(timerStart){
		elapsedTime += m_dt;
	}
	if(elapsedTime > 1000.0f){
		elapsedTime = 0;
		timerStart = false;
		m_bPressed = false;
	}

	m_iValue = GetMode(digitalRead(m_iPin));

	if(m_iValue && !m_bPressed){
			m_bPressed = true; 
			timerStart = false;
			elapsedTime = 0.0f;

	}
	if(!m_iValue && m_bPressed){
			timerStart = true;
	}
	
	return m_bPressed;
}
bool CSensor::GetMode(int sample){
	static int count = 1; //Keeps track of the mode frequency
	static int SampleMarker = 0;//points to sample array position, and resets to 0 from MODE_SAMPLE

	SampleList[SampleMarker] = sample; // adds the new sensor reading to the array
	--count;

	//If the new value is different to the mode, check for new mode
	if(m_iMode != sample){
		int newCount = 0; //Keeps track of how many times a number other than the mode arises

		
		//Counts the frequency of the new sensor reading in the sample
		for( int i = 0; i < MODE_SAMPLE; ++i){
			if(sample == SampleList[i]){
				newCount++;
			}
		}
		//if there are more of the new sensor values than old, it  becomes the new mode
		if(newCount >= count){
			m_iMode = sample;
			count = newCount;
		}
	}else{ //Mode and distance are equal
		if(count < MODE_SAMPLE){ //count cannot become greater than the max sample size
			count++;
		}
	}
	//Increments and resets the sample marker if necessary
	SampleMarker++;
	if(SampleMarker > MODE_SAMPLE-1){
		SampleMarker = 0;
	}

	return m_iMode;

}
bool CSensor::GetSensor(){
	return m_bPressed;
}