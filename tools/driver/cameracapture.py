import subprocess
import signal

#set appropriate settings here as well, like exposure and whatever
#also set raw mode or not, etc.
#also error checking with suggestions on how to fix it.
class Camera(object):
	def __init__(self,d=""):
		if(d == ""):
			farg=[""]
		else:
			farg=["--filename",d+"%03n.%C"]
		self.fargs=farg
		self.gp=None
	def capture(self,last=False):
		if(not self.gp):
			if(last):
				subprocess.Popen(["gphoto2","--capture-image-and-download"]+self.fargs)
			else:
				self.gp=subprocess.Popen(["gphoto2","-I","-1","--capture-image-and-download"]+self.fargs)
		else:
			if(last):
				self.gp.send_signal(signal.SIGUSR2)
			self.gp.send_signal(signal.SIGUSR1)

		
