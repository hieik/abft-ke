#!/bin/bash


nodeperswitch=$1  
corepernode=$2

#
switchlat=0.0000001
#
cablebw=40000000000
#
power=100000000000

inputfile=$3
outputfile=$4

ruby generate_platform.rb --cfg nodeperswitch:$nodeperswitch --cfg corepernode:$corepernode --cfg power:$power --cfg switchlat:$switchlat --cfg cablebw:$cablebw $inputfile $outputfile
