#include <Wire.h>
#include <math.h>
#define LED_PIN 10
#define SLAVE_ADDRESS 0x04
#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 30
#define NUM_BITS 4

static unsigned int m_iState = 0;

unsigned int m_iBrightness = 0; //current brightness of LED (0-MaxBrightness)
float m_fPeriod = 2.0f;

long int m_iStartTime; //must be a long as it takes return of millis()


word dt;
int led_state[5] ={0,1,0,0,1};

int led_index = 0;

bool finished_sequence = true;

static float elapsed = 0.0f;
static bool increase = true;
static bool hold = true;
static float buffer = 0.0f;



// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	delay(4000);
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
	

	SinePulse();
	SetBrightness();
	

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
	int temp = 0;
	

	Serial.print("Recieving \n");
	while(Wire.available()){
		finished_sequence = false;
		m_iState = Wire.read();
		Serial.println(m_iState);
		temp = m_iState;

		
		//Check for touch activation on left side (side 4)
		if(temp >= 8){
			temp -=8;
			led_state[4] = 1;
		}else{
			led_state[4] = 0;
		}
	
		//Check back side 3
		if(temp >= 4){
			temp -= 4;
			led_state[3] = 1;		
		}else{
			led_state[3] = 0;
			}

		//Check right side (side 2)
		if(temp >= 2){
			temp -= 2;
			led_state[2] = 1;
		} 
		else{
			led_state[2] = 0;
		}
	
		//Check front side (side 1)
		if(temp >= 1){
			temp -=1;
			led_state[1] = 1;
		}
		else{
			led_state[1] = 0;
		}
		led_state[0] = 0;
		
	}

	
}
void sendData(){
	Wire.write(m_iState);
}
void SetPeriod(float p){
	m_fPeriod = p;
}

void LinearPulse(){
	
	if(increase && !hold){ 
		buffer += (float)dt/m_fPeriod;

	}
	else if (!increase && !hold) {
		buffer -= (float)dt/(m_fPeriod*1.3f);

	}

}

void SinePulse(){
	static float x = 0.0f;
	if(increase && !hold){
		x += (float)dt/(m_fPeriod*1000);
		buffer = abs(sin(x));
		buffer *= MAX_BRIGHTNESS;

			
	}
	else if(!increase && !hold){
		x -= (float)dt/(m_fPeriod*1000);
		if(x < 0.0f)
			x = 0.0f;
		buffer = abs(sin(x));
		buffer *= MAX_BRIGHTNESS;

	}

}

void LogPulse(){
	static float x = 0.0f;
		if(increase && !hold){
			x += (float)dt/(m_fPeriod*1000);
			buffer = (log(x+1)+0.5)*(MAX_BRIGHTNESS);
			

		}else if (!increase && !hold){
			x -= (float)dt/(m_fPeriod*1000);
			if(x < 0.0f)
				x = 0.0f;
			buffer = (log(x+1)+0.5)*(MAX_BRIGHTNESS);
		
		}
		

}
void SetBrightness(){
	
	
	m_iBrightness = (int)buffer+1;

	//Serial.print("Brightness:");
	//	Serial.println(m_iBrightness);

	if(m_iBrightness >= MAX_BRIGHTNESS){
		m_iBrightness = MAX_BRIGHTNESS;
		buffer = MAX_BRIGHTNESS;
		hold = true;
	
	}
	if(m_iBrightness <= MIN_BRIGHTNESS && !increase){
		m_iBrightness = MIN_BRIGHTNESS;
		buffer = MIN_BRIGHTNESS;
		hold = true;
	}
	

	if(hold)
		elapsed += (float)dt;


	
	if(elapsed > 2000){
		elapsed = 0.0f;
		++led_index;

		if(led_index > 4){
			
			led_index = 0;
			finished_sequence = true;
			increase = false;
		}	
		
		increase = led_state[led_index];
		Serial.print(led_index);
		Serial.print(" : ");
		Serial.println(led_state[led_index]);
		hold = false;
	}

	//Sets the colour value directly to the pin
	analogWrite(LED_PIN, m_iBrightness);
	delay(10);
	//Serial.println(m_iBrightness);
}