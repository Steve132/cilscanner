import capture_sequence
import driver.cilscanner as cscan  #import the scanner driver
import driver.cameracapture as camcap
import os,os.path,shutil
import json
import datetime

calibsequence=[0xFFFF]+[ (1 << x) for x in range(16)]

def capture_calibration(camera,scanner,focal_length=135,do_background=True):
	if os.path.exists('calib'):
		shutil.rmtree('calib')
    	os.makedirs('calib')
	camera.cwd='calib'
	if(do_background):
		raw_input("Remove all objects from the scene except background ones.")
		camera.filename='background'
		
		capture_sequence.capture_sequence(camera,scanner,[0xFFFF])

	raw_input("\n\nPut the calibration object (red ball) into the scene\n\n")
	camera.filename='calib'
	
	capture_sequence.capture_sequence(camera,scanner,calibsequence)

	settings={'focal_length':focal_length,'background':do_background}
	f=open(os.path.join('calib','settings.json'),'w')
	json.dump(settings,f)
	
def capture_object(camera,scanner):

	name=raw_input("What is the name of the object you would like to scan? ")
	if os.path.exists(name):
		shutil.rmtree(name)
	os.makedirs(name)
	camera.cwd=name
	camera.filename='object'

	raw_input("\n\nPut the object '%s' into the scene\n\n" % (name))
	capture_sequence.capture_sequence(camera,scanner,calibsequence)

	exit=raw_input("Would you like to scan another object? [Y/n]")
	return not (exit == "n" or exit == "N")

def get_timestamp():
	today=datetime.date.today().isoformat()
	test=1
	attempt=today+'-'+str(test)
	while(os.path.exists(attempt)):
		test+=1
		attempt=today+'-'+str(test)
	return attempt

if(__name__=="__main__"):
	camera=camcap.Camera()
	scanner=cscan.Scanner()

	raw_input("Make sure that the camera is ejected from the OS")
	ts=get_timestamp()
	os.makedirs(ts)
	os.chdir(ts)
	flen=int(raw_input("What is the focal length? "))
	capture_calibration(camera,scanner,focal_length=flen,do_background=True)
	while(capture_object(camera,scanner)):
		pass
