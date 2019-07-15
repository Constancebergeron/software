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

char rData[4] = {0};
char data[2] = {0};
char config[1] = {0x00};
int channel = 1;
int file;
int raw_adc;
int rawRegister;
int minVal1 = 3000;
int minVal2 = 3000;
int delay1;
int delay2;
int isReady = 0;
int disable1 = 0;
int disable2 = 0;
int res01;
int res01low = 0;
int res02;
int res02low = 0;

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

int ready(){
// cheque si la convertion est finie
if(read(file, rData, 4) != 4){
printf("Error while reading Register \n");
}
else{
rawRegister = rData[3];
printf("The value of MSB register is %d \n",rawRegister);

	if(rawRegister >= 128){
	isReady = 0;
	}
	else{
	isReady = 1;
	}
printf(" isReady = %d \n",isReady);
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
//		if(raw_adc > 2047)
//		{
//			raw_adc -= 4096;
//		}

		// Output data to screen
		printf("Digital value of Analog Input %d : %d \n",channel, raw_adc);
	}
}

int temp01(){	

    	config[0] = 0x80; // Config = 0X80 chan 01 one shot 12 bits new convertion
	write(file, config, 1);
while(isReady == 0){
	ready();
	usleep(1000);
}
	isReady = 0;    
	rread();
    res01 = raw_adc;
}

int temp02(){	
    config[0] = 0xa0; // Config = 0X80 chan 01 one shot 12 bits new convertion
	write(file, config, 1);
while(isReady == 0){
	ready();
	usleep(1000);
}
	isReady = 0;    
	rread();
    res02 = raw_adc;
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
        
//////////////////////// checking 01 ///////////////////////////////
	if(disable1 == 0){    //keep but not neede
    eventType = "ZapCh01";
    printf("Atively monitoring 01!!!!!!!! \n");
	channel = 1;
	temp01();
printf("value of channel 01 = %d \n",res01);
        if(res01 < 600){
            res01low = 1;
            while(res01low == 1) {
            temp01(); //initiane new convertion and wait for result
            printf("value of channel 01 = %d \n",res01);
            if(res01 > 600){
                getTime();
                writeCsv();
                res01low = 0;}
            else{
                res01low = 1;
                }
                                }
            }

	}
//////////////////////////////checking 02///////////////////////////////////
	if(disable2 == 0){

    eventType = "ZapCh02";
    printf("Atively monitoring 02!!!!!!!! \n");
	channel = 2;
	temp02();
printf("value of channel 02 = %d \n",res02);
        if(res02 < 600){
            res02low = 1;
            while(res02low == 1) {
            temp02(); //initiane new convertion and wait for result
            printf("value of channel 02 = %d \n",res02);
            if(res02 > 600){
                getTime();
                writeCsv();
                res02low = 0;}
            else{
                res02low = 1;
                }
                                }
            }

	}   
              }	// end de Whie
}
