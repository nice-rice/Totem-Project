#pragma once
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>



// The Pi board i2c address
#define ADDRESS 0x04
// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0
static const char *devName = "/dev/i2c-1";

class CLED{

public: 
	CLED();
	~CLED();

	void Initialise();
	void Update(int state);

private:
	int conn;
	int led_state;
	float LedFrequency;

};