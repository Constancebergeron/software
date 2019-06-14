#!/bin/bash
cd /Sterno/results/test.csv
git add -A
if [ $# -eq 0 ]
then 
git commit -m "Updated Automaticaly by the Beagle"
else 
git commit -m "$*"
fi
git status
git push origin master
