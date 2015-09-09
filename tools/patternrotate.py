import driver.cilscanner as cscan  #import the scanner driver
import sys
import time
#sequence is a list of list of bools

if(__name__=="__main__"):
	scanner=cscan.Scanner()
	for a in range(600):
		scanner.pattern(1 << (2* (a % 8)))
		time.sleep(3.0/30.0)

	
