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
#include "GPIO.h"
using namespace exploringBB;
using namespace std;

int Hour;
int Min;
int Sec;
int Date;
int Month;
int probeRes = 0;
int regVal = 0;
int updateComplete = 0;
int updateCount = 0;
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

GPIO inGPIO(115);
inGPIO.setDirection(INPUT);
probeRes = inGPIO.getValue();
//cout << "the val is: " << inGPIO.getValue() << endl; // for testing only delete
           }

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

int resetReg() {
eventType = "reseting reg";		
writeCsv();				
ofstream myfile;
myfile.open ("/Sterno/Beagle/Reg/.reg01");
    if (myfile.is_open()){
    myfile << "0";
    myfile.close();
    updateComplete = 0;
    updateCount = 0;                     }
    else printf ("Unable to open File");
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

///////////////////// Main Function /////////////////////////
int main(){

int init = 0;

int pwrGood = 0; 	//0 = ac present
int pwrTrig = 0;

while (init <= 10, init ++){
probe();
}

getTime();
eventType = "MS-Rst";
writeCsv();
resetReg();

int regTime = Hour;
int updateTime = Min;
int watchTime = Min;
int powerTime = Min;
int regCount = 0;
int watchCount = 0;
int powerCount = 0;


while (1) {

getTime();

// Start of Hourly Registration
if (Min != watchTime){
    watchCount = (watchCount + 1);
    watchTime = Min;
                     }

if (watchCount == 60) {	//modified remettre a 60
    getTime();
    eventType = "MH-Reg";
    writeCsv();
    watchCount = 0;
                     }
// End of Hourly Registration


//Power Good
//printf ("checking power");
/// Let's test it only once a Minute, also eliminates glitches from Hydro
/// it will be ofset from the other timers?
if (powerCount == 1){
powerCount = 0;
probe();

if (pwrGood != probeRes && pwrTrig == 0) {
getTime();
eventType = "PWR-Out";
//if (init == 10){
writeCsv();
pwrTrig = 1;
//              }
                                          }

if (pwrTrig == 1 && probeRes == 0){
getTime();
eventType = "PWR-Bon";
writeCsv();
pwrTrig = 0;
                                  }
 }// end of powerCount if
getTime();
if (Min != powerTime){
powerCount = (powerCount + 1);
powerTime = Min;
}

printf ("Testing in Progress\n");   //Dummy, delete


////// Start of Update Sequence /////////

if (updateComplete == 0){
	getTime();
	if (Hour >= 7 && Hour <= 16){ 
		readReg();
	if (regVal == 1){updateComplete = 1;}	// On fait ca pcq on veut pas caller readReg tout le temps
    	if (regVal == 0 && updateCount == 0){   //delai pour l'execution de "system"
   	 updateCount = 10; // will try to update every  [value] minutes
//    eventType = "UPDTT"; // for testing purpose only delete
//    writeCsv();   	 // for testing purpose only delete
    system("/Sterno/Software/gitUpdateRes.sh"); //if  succesfful reg wil be 1
  }
    else {
//    printf ("no avail\n");
    getTime();
        if (Min != updateTime){	//La minute a changee
	updateCount = (updateCount - 1);
	updateTime = Min;
                              }
         }
                               }
                          }
///// Reset the Registery every 24 Hours
if (regCount == 24){	//Reset le registry a toutes le 24 heures
regCount = 0;
resetReg(); // reset le reg a toutes le 24 heures
                  }
getTime();
if (Hour != regTime){
regCount = (regCount + 1);
regTime = Hour;
                   }
        }  /// end of main While
return 0;

}


