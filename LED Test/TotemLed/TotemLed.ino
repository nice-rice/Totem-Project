#include <Wire.h>

#define LED_PIN 10
#define SLAVE_ADDRESS 0x04
#define MAX_BRIGHTNESS 250


word m_iBrightness =0; //current brightness of LED (0-MaxBrightness)
word m_iTargetBrightness =0; //brightness based on current sensor readings
float buffer = 0; //buffer value used to store intermediate colour values

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
	

	

	

	//Timer used for FPS
	frameCount++; 
	//after every second, print the frame count and reset the timer
	if(elapsedTime>1000){ 
		Serial.print("FPS: ");
		Serial.println(frameCount);
		
		elapsedTime = 0; // reset timer
		frameCount =0;  //reset frame count

	}
	
	m_iStartTime = time;	//resets the time
}


void receiveData(int byteCount){
	if(Wire.available()){
		ledState = Wire.read();
		CalculateBrightness((float)ledState/100);
	}
}
void sendData(){
 Wire.write(ledState);
}


void CalculateBrightness(float target){
	
	m_iTargetBrightness = floor(target);


	if(m_iTargetBrightness != m_iBrightness){
		if(m_iTargetBrightness > m_iBrightness){
			buffer += float(dt)/15;		
			if(buffer > MAX_BRIGHTNESS){
				buffer = MAX_BRIGHTNESS;
			}
		}
		else if (m_iTargetBrightness < m_iBrightness){
			buffer -= float(dt)/30;
			if(buffer < 0)
				buffer = 0;
		}
		
		m_iBrightness = floorf(buffer+0.5f);

	}
	else{
		m_iBrightness = m_iTargetBrightness;
		buffer = m_iBrightness;
	}

	analogWrite(LED_PIN, m_iBrightness);
}
