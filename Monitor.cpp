/*!
 \File Monitor.cpp

 \Brief		This progam is the watchdog for the Zap Bug Dataloger
		It Register the folowing events and log them:
		-First start of the Program 	(MS-Rst)
		-Hourly registration		(MH-Reg
		-Power Outage			(PW-Out)
		-Power Back On			(PW-Bon)

		It also tries to update a file on Git Hub once a day
		if network is available, between 9 am and 4 pm.

 \Hardware	Intended to run On Beagle Black Wireless

 \Author	Marcel Bergeron (Creative Beagle Technologies)
 \Revision	1.0
 \date		2019-6-13

*/



#include <iostream>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <string>
#include <pthread.h>

using namespace std;

int Hour;
int Min;
int Sec;
int Date;
int Month;

int probeRes =0;

int regVal = 0;
int readDelay = 0;
int updateComplete = 0;
//int bglTime;
//int bglCount = 0;

//int pwrGood = 0;	// 0 means power present, 1 power not present

string eventType = "blank";


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

int probe(){
// do some stuff
probeRes = 0;
}

int writeCsv(){
	ofstream myfile;
	myfile.open ("/Sterno/results/bigul.csv", ios::in | ios::app);
	if (myfile.is_open()){
		printf ("Registering Event \n");
		myfile << Month << "," << Date << "," << Hour << "," << Min << "," << Sec << "," << eventType <<  "\n";
		myfile.close();}
	else printf("Unable to open file\n");
	return 0; 
}

int resetReg() {
// Must Do some stuff
// write 0 to reg01
}

int readReg(){
string line;
ifstream myfile ("/Sterno/Beagle/Reg/.reg01");
if (myfile.is_open()){
while (getline (myfile,line)){
printf ("Checking Registry\n");
	if (line == "0"){
	regVal = 0;
	}
	else regVal = 1;
	}
myfile.close();
}
else cout << "Unable to open File";
}


int main(){

//int updatComplete = 0;

int pwrGood = 0; 	//0 = ac present
int pwrTrig = 1;

int bglCount = 0;
int bglTime;
int bglTime2;

getTime();
eventType = "MS-Rst";
writeCsv();
bglTime = Min;

while (1) {
getTime();

// Start of Hourly Registration
if (Min != bglTime){
    bglCount = (bglCount + 1);
    bglTime = Min;
}

if (bglCount == 1) {	//modified remettre a 60
    getTime();
    eventType = "MH-Reg";
    writeCsv();
    bglCount = 0;
}
// End of Hourly Registration
//Power Good
//printf ("checking power"); 	// dummy, probe if beagle pin is 0 or 1
				// if ac present relay open = pin float
				// if ac not present relay closed = pin 5V
				// if relay not installed, always float and program runs 
probe();		// some code to probe Beagle pin
if (pwrGood != probeRes) {
getTime();
eventType = "PWR-Out";
writeCsv();
pwrGood = 1;
pwrTrig = 0;
}

if (pwrTrig == 0 && probeRes == 0){
getTime();
eventType = "PWR-Bon";
writeCsv();
pwrTrig = 1;

} 

printf ("Testing in Progress\n");   //Dummy, delete

//}

if (updateComplete == 0){
  getTime();
  if (Hour >= 7 && Hour <= 16){ 
  readReg();
    if (regVal == 0 && readDelay == 0){
    readDelay = 1; // modified replacer par 10 pour 10 min
    eventType = "UPDTT"; // for testing purpose only delete
    writeCsv();   	 // for testing purpose only delete
    bglTime2 = Min;
    printf ("reg 0");  // remplacer par execute git synch
    }
    else {
    printf ("no avail\n");
    getTime();
        if (Min != bglTime2){
	bglTime2 = Min;
	readDelay = (readDelay - 1);
        }
    }

  }
}

// update files on Github
// check if between 9 and 4
// check if try counter = 0

// check if reg01 = 0   /  try counter =10 min
// if yes start update
// if no do nothing
// clock for the try counter
// at 4 01 pm reset reg01
}
return 0;

}


