import driver.cilscanner as cscan  #import the scanner driver
import sys
#sequence is a list of list of bools

if(__name__=="__main__"):
	
	p=int(sys.argv[1],16)
	scanner=cscan.Scanner()
	scanner.pattern(p)

