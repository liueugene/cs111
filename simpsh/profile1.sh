#!/bin/execlineb -P
time 
redirfd -r 0 a
redirfd -w 1 b
pipeline { head -c 100m /dev/urandom }
pipeline { base64 }
sort
