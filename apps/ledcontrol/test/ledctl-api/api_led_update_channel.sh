#!/bin/bash

source ./config.sh

if [ "$1" == "" ]; then
   echo Enter channel number
   exit
fi

if [ "$2" == "" ]; then
   echo Enter RGB value
   exit
fi


channel=$1
rgb=$2

curl $CURL_OPTIONS -X GET $URL_API/led/update_channel/$channel/$rgb | jq

