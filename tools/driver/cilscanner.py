import serial
import time

class Scanner(object):
	def __init__(self,port='/dev/ttyACM0'):
		self.serport=serial.Serial(port,9600)
		time.sleep(1.75)
	def pattern(self,pattern):
		binpattern=pattern
		if(not isinstance(binpattern,(int, long))):
			binpattern=0
			for i in range(16):
				binpattern|= (1 if pattern[i] else 0) << i
		msg='\x01'+chr(binpattern >> 8)+chr(binpattern & 0xFF)
		self.serport.write(msg)
