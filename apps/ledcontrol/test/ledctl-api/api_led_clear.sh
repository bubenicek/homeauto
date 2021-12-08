#!/bin/bash

source ./config.sh

channel=$1

if [ "$1" == "" ]; then
   channel=255
fi

curl $CURL_OPTIONS -X GET $URL_API/led/clear/$channel | jq

