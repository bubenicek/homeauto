#!/bin/bash

source ./config.sh

ID=051724052cf432cb1ad4
STATE=$1

curl $CURL_OPTIONS -X GET $URL_API/device/set/$ID/$STATE


