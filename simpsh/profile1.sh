#!/bin/execlineb -P
redirfd -r 0 a
redirfd -w 1 b
time 
pipeline { head -c 100m /dev/urandom }
pipeline { base64 }
sort
