#!/bin/bash

source ./config.sh

if [ "$1" == "" ]; then
   echo Enter channel number
   exit
fi

if [ "$2" == "" ]; then
   echo Enter pixel index
   exit
fi

if [ "$3" == "" ]; then
   echo Enter RGB value
   exit
fi


channel=$1
pixel=$2
rgb=$3

curl $CURL_OPTIONS -X GET $URL_API/led/set_pixel/$channel/$pixel/$rgb | jq

