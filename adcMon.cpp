/*!
 \File adcMon.cpp

 \Brief		This program monitors the 2 channels of an I2C ADC
		MCP3426 sequentially
		When the voltage falls bellow a certain treshold, and then
		commes back over the same thresshold, the programs write an
		event on an independent .csv File

 \Hardware	Intended to run on the Beagle Black Wireless

 \Author	Marcel Bergeron (Creative Beagle Technologies)
 \Revision 	1.0
 \Date		2019-07-11

*/

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <pthread.h>

using namespace std;

char data[2] = {0};
char config[1] = {0x00};
int channel = 1;
int file;
int raw_adc;
int minVal1 = 3000;
int minVal2 = 3000;
int delay1;
int delay2;

string eventType = "blank";

int Hour;
int Min;
int Sec;
int Date;
int Month;

int zapOn1 = 0;
int zapOff1 = 0;
int zapOn2 = 0;
int zapOff2 = 0;	

int writeCsv(){
	ofstream myfile;
	myfile.open ("/Sterno/results/zapResults.csv", ios::in | ios::app);
	if (myfile.is_open()){
		printf ("Registering Event \n");
		myfile << Month << "," << Date << "," << Hour << "," << Min << "," << Sec << "," << eventType <<  "\n";
		myfile.close();  }
	else printf("Unable to open file\n");
	return 0;
}
int getTime(){
time_t currentTime;
struct tm *localTime;

time( &currentTime);				// Get Current Time
localTime = localtime( &currentTime );		// Convert to Local Time
 
Hour = localTime->tm_hour;
Min = localTime->tm_min;
Sec = localTime->tm_sec;
Date = localTime->tm_mday;
Month = localTime->tm_mon;
Month = (Month + 1);
}

int evaluate1(){
	printf("evaluating 1 \n");
	if(raw_adc < minVal1){
	minVal1 = raw_adc;
	}
//	printf("The Minimum value = %d \n",minVal1);  //For debug purpose only
	
	if(raw_adc < 600 && zapOn1 == 0){
	zapOn1 = 1;
	getTime();
	delay1 = Sec;
	} 
	getTime();
	if(zapOn1 == 1 && raw_adc > 600 && Sec != delay1){
	zapOff1 = 1;
	}
	if(zapOn1 == 1 && zapOff1 == 1){
	eventType = "ZapCh01";
	getTime();
	writeCsv();
	zapOn1 = 0;
	zapOff1 =0;
	}
}

int evaluate2(){
	printf("evaluating 2 \n");
	if(raw_adc < minVal2){
	minVal2 = raw_adc;
	}
//	printf("The Minimum value ch 02 = %d \n",minVal2);  // For debug purpose only

	if(raw_adc < 600 && zapOn2 == 0){
	zapOn2 = 1;
	getTime();
	delay2 = Sec;
	} 
	getTime();
	if(zapOn2 == 1 && raw_adc > 600 && Sec != delay2){
	zapOff2 = 1;
	}
	if(zapOn2 == 1 && zapOff2 == 1){
	eventType = "ZapCh02";
	getTime();
	writeCsv();
	zapOn2 = 0;
	zapOff2 =0;
	}
}

int rread(){
	// Read 2 bytes of data from register(0x00)
	// raw_adc msb, raw_adc lsb
	// char data[2] = {0};
	if(read(file, data, 2) != 2)
	{
		printf("Error : Input/output Error \n");
	}
	else
	{
		// Convert the data to 12-bits
		raw_adc = ((data[0] & 0x0F) * 256 + data[1]);
		if(raw_adc > 2047)
		{
			raw_adc -= 4096;
		}

		// Output data to screen
		printf("Digital value of Analog Input %d : %d \n",channel, raw_adc);
	}
}


int main() 
{
	// Create I2C bus
	const char *bus = "/dev/i2c-2";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, MCP3426 I2C address is 0x68(104)
	ioctl(file, I2C_SLAVE, 0x68);
    
	while(1) {
        
    
	channel = 2;
	config[0] = 0x80; // Config = 0X00 chan 01 one shot 12 bits new convertion
	write(file, config, 1);    
	rread();
	evaluate2();
	usleep(3500);
	channel = 1;
	config[0] = 0xa0; // channel 2 one shot 12 bits new convertion
	write(file, config, 1);
	rread();
	evaluate1();
	usleep(3500);
	//channel = 1;

              }	// end de Whie
}
