#!/bin/bash
git add -A
if [ $# -eq 0 ]
then 
git commit -m "Updated Automaticaly by the Beagle"
else 
git commit -m "$*"
fi
git status
git push origin master
