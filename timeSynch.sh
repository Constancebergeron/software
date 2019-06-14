#!/bin/bash

printf "\n\nBoot Sequence Initiated\n\n"
sleep 1
	printf "Testing Wi-Fi connection.....\n\n"
ping -c1 google.com
if  [ $? -eq 0 ]
then
	printf "\n\n\nWifi and Internet Connection Status O.K\n\n\nProceeding....\n\n\n"
	printf "Synchronizing with Time Server\n\n"
ntpdate -b -s -u pool.ntp.org
	printf "Current Time is:"
	printf "\n"
date
	printf "\n"
else
	clear
	printf "\n\\nWARNING!!!!\n\n\nYou are not connected to the Internet\n\n\n"
	printf "The Beagleboard Industrial Developpment Platform does not have\n"
	printf "internal battery operated RTC and needs to be synchronized at startup\n"
	printf "\nAny Log Entries WILL have incorect Dates and Time\n\n"
	printf "Please refer to /Sterno/Documentation/Wifi_Setup.txt\n"
	printf "(pico /Sterno/Documentation/Wifi_Setup.txt)\n\n" 
fi
