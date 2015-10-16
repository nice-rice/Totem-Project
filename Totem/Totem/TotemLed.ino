#include <Wire.h>
#include <math.h>
#define LED_PIN 10
#define SLAVE_ADDRESS 0x04
#define MAX_BRIGHTNESS 250


float m_fBrightness =0.0f; //current brightness of LED (0-MaxBrightness)
float m_fPeriod = 1.0f;

long int m_iStartTime; //must be a long as it takes return of millis()
int ledState;
word dt;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	Serial.println("Running setup");


	byte LEDPins[1] = {LED_PIN}; 
	pinMode(LED_PIN, OUTPUT);

	// initialize i2c as slave
	Wire.begin(SLAVE_ADDRESS);

	// define callbacks for i2c communication
	Wire.onReceive(receiveData);
	Wire.onRequest(sendData);
    
	
	Serial.println("Done setup");
	m_iStartTime = millis(); //Start timer for dt calculations
	 //delay for one second for sensor readings
	delay(1000);

}

// the main loop function
void loop() {
	
	static int frameCount  = 0; //tracks number of times the loop updates
	static int elapsedTime = 0; //time between start time and current time

	
	long int time = millis(); //gets the current time
	dt = time- m_iStartTime; 

	elapsedTime += dt; //increment elapsed by time between frames
	

	LogPulse();

	

	//Timer used for FPS
	frameCount++; 
	//after every second, print the frame count and reset the timer
	if(elapsedTime>1000){ 
		//Serial.print("FPS: ");
		//Serial.println(frameCount);
		
		elapsedTime = 0; // reset timer
		frameCount =0;  //reset frame count

	}
	
	m_iStartTime = time;	//resets the time
}


void receiveData(int byteCount){
	if(Wire.available()){
		ledState = Wire.read();
		SetPeriod((float)ledState);
	}
}
void sendData(){
 Wire.write(ledState);
}
void SetPeriod(float p){
	m_fPeriod = p;
}

void LinearPulse(){
	
	static float elapsed = 0.0f;
	static bool increase = true;
	static bool hold = false;

	
		if(increase && !hold) m_fBrightness += (float)dt/m_fPeriod;
		else if (!increase && !hold) m_fBrightness -= (float)dt/m_fPeriod*1.3;

		if(m_fBrightness >= MAX_BRIGHTNESS){
			m_fBrightness = MAX_BRIGHTNESS;
			hold = true;
			increase = false;
		}
		if(m_fBrightness <= 0.0f){
			m_fBrightness = 0.0f;
			//hold = true;
			increase = true;
		}
	
		if(hold)
			elapsed += (float)dt;

		if(elapsed > 50*m_fPeriod){
			elapsed = 0.0f;
			hold = false;
		}
	
	analogWrite(LED_PIN, m_fBrightness);
	Serial.println(m_fBrightness);
}

void SinePulse(){

	static float elapsed = 0.0f;
	static bool hold = false;
	static float x = 0.0;
	
		if(!hold){

			x += (float)dt/(m_fPeriod*1000);
			m_fBrightness = abs(sin(x)*MAX_BRIGHTNESS);

		}

		if(m_fBrightness >= MAX_BRIGHTNESS){
			m_fBrightness = MAX_BRIGHTNESS;
			hold = true;
		}

	
		if(hold)
			elapsed += (float)dt;

		if(elapsed > 50*m_fPeriod){
			elapsed = 0.0f;
			hold = false;
		}

	//Sets the colour value directly to the pin
	analogWrite(LED_PIN, m_fBrightness);


}

void LogPulse(){
	static float elapsed = 0.0f;
	static bool increase = true;
	static bool hold = false;
	static float x = 0.0f;
	
		if(increase && !hold){
		
			x += (float)dt/(m_fPeriod*1000);
			m_fBrightness = (log(x)+0.5)*MAX_BRIGHTNESS;

		}else if (!increase && !hold){
		
			x -= (float)dt/(m_fPeriod*1000);
			m_fBrightness = (log(x)+0.5)*MAX_BRIGHTNESS;
		
		}

		if(m_fBrightness >= MAX_BRIGHTNESS){
			m_fBrightness = MAX_BRIGHTNESS;
			hold = true;
			increase = false;
		}
		if(m_fBrightness <= 0.0f){
			m_fBrightness = 0.0f;
			//hold = true;
			increase = true;
		}
	
		if(hold)
			elapsed += (float)dt;

		if(elapsed > 50*m_fPeriod){
			elapsed = 0.0f;
			hold = false;
		}
	
	


	//Sets the colour value directly to the pin
	analogWrite(LED_PIN, m_fBrightness);
	Serial.println(m_fBrightness);


}