#!/usr/local/cs/execline-2.1.4.5/bin/execlineb -P
time
redirfd -r 0 a
redirfd -w 1 c
pipeline { sort }
pipeline { cat b - }
tr A-Z a-z