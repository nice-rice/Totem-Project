#include "LED.h"

CLED::CLED(){

}
CLED::~CLED(){
	close(conn);
	
}



void CLED::Initialise(){
	
	
  if ((conn = open(devName, O_RDWR)) < 0) {
    fprintf(stderr, "I2C: Failed to access %d\n", devName);

  }
 
  printf("I2C: acquiring buss to 0x%x\n", ADDRESS);
 
  if (ioctl(conn, I2C_SLAVE, ADDRESS) < 0) {
    fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
    
  }

}
void CLED::Update(int state){

	if (write(conn, &state, 1) == 1) {
		
      // As we are not talking to direct hardware but a microcontroller we
      // need to wait a short while so that it can respond.
      //
      // 1ms seems to be enough but it depends on what workload it has
    usleep(10000);
	printf("Sending %d\n", state);
	   char buf[1];
      if (read(conn, buf, 1) == 1) {
		int temp = (int) buf[0];
 
		printf("Received %d\n", temp);	
      }

	}
	usleep(10000);
	
}