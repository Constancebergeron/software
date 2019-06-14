#!/bin/bash

ping -c1 google.com
if [ $? -eq 0 ]
then
	cd /Sterno/results/
	git add -A
	if [ $# -eq 0 ]
	then 
	git commit -m "Updated Automaticaly by the Beagle"
	else 
	git commit -m "$*"
	fi
	git status
	git push origin master
	cd /Sterno/Beagle/Reg
	echo "1" > .reg01
else
cd /Sterno/Beagle/Reg
echo "0" > .reg01
fi
