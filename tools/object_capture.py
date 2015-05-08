import capture_sequence
import driver.cilscanner as cscan  #import the scanner driver
import driver.cameracapture as camcap
import os,os.path,shutil
import json

def capture_calibration(camera,scanner,focal_length=135,do_background=True):
	if os.path.exists('calib'):
		shutil.rmtree('calib')
    	os.makedirs('calib')
	camera.cwd='calib'
	if(do_background):
		raw_input("Remove all objects from the scene except background ones.")
		camera.filename='background'
		
		capture_sequence.capture_sequence(camera,scanner,[0xFFFF])

	raw_input("Put the calibration object (red ball) into the scene")
	camera.filename='calib'
	
	calibsequence=[0xFFFF]+[ (1 << x) for x in range(16)]
	capture_sequence.capture_sequence(camera,scanner,calibsequence)

	settings={'focal_length':focal_length,'background':do_background}
	f=open(os.path.join('calib','settings.json'),'w')
	json.dump(settings,f)
	

if(__name__=="__main__"):
	camera=camcap.Camera()
	scanner=cscan.Scanner()
	flen=int(raw_input("What is the focal length?"))
	capture_calibration(camera,scanner,focal_length=flen,do_background=True)
