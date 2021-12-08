#!/bin/bash

if ps -ef | grep -v grep | grep "node main.js" > /dev/null 
then
	echo "Process is running."
	screen -x tuyaserver
else
	echo "Process is not running."
	screen -dmS tuyaserver node main.js
	screen -x tuyaserver
fi


