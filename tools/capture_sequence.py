import sys
import time
import driver.cilscanner as cscan  #import the scanner driver
import driver.cameracapture as camcap
#sequence is a list of list of bools
def capture_sequence(sequence,scanport=None,destination=""):
	if(scanport):
		scanner=cscan.Scanner(scanport)
	else:
		scanner=cscan.Scanner()
	time.sleep(4)
	cam=camcap.Camera(destination)
	for i in range(len(sequence)):
		scanner.pattern(sequence[i])
		time.sleep(0.5)
		cam.capture(last=(i==(len(sequence)-1)))
		time.sleep(2.0)


if(__name__=="__main__"):
	f=sys.stdin.read()
	seq=[int(x,16) for x in f.split()]
	capture_sequence(seq,scanport='/dev/ttyACM0')

