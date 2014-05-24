import os
import sys

def parseGprof(path):
	f = open(path,'r')
	lines = f.readlines()
	for l in lines:
		


paths = os.listdir('./')
for path in paths:
	if '.gprof' in path:
		parseGprof(path)


