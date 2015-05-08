import subprocess
import signal

#set appropriate settings here as well, like exposure and whatever
#also set raw mode or not, etc.
#also error checking with suggestions on how to fix it.
class Camera(object):
	def __init__(self,d=""):
		
		self.gp=None
		self.filename=d
		self.cwd=None
	def capture(self,last=False):
		if(self.filename == "" or not self.filename):
			farg=[""]
		else:
			farg=["--filename",self.filename+"%03n.%C"]
	
		if(not self.gp):
			if(last):
				subprocess.Popen(["gphoto2","--capture-image-and-download"]+farg,cwd=self.cwd)
			else:
				self.gp=subprocess.Popen(["gphoto2","-I","-1","--capture-image-and-download"]+farg,cwd=self.cwd)
		else:
			if(last):
				self.gp.send_signal(signal.SIGUSR2)
			self.gp.send_signal(signal.SIGUSR1)
			if(last):
				self.gp=None

		
