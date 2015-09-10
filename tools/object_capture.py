import capture_sequence
import driver.cilscanner as cscan  #import the scanner driver
import driver.cameracapture as camcap
import os,os.path,shutil
import json
import datetime
import argparse

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
	
def capture_object(camera,scanner,sequence):

	name=raw_input("What is the name of the object you would like to scan? ")
	if os.path.exists(name):
		shutil.rmtree(name)
	os.makedirs(name)
	camera.cwd=name
	camera.filename='object'

	raw_input("\n\nPut the object '%s' into the scene\n\n" % (name))
	capture_sequence.capture_sequence(camera,scanner,sequence)

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

	parser = argparse.ArgumentParser(description='Capture some objects.')
	parser.add_argument('--calibrate', help='Run calibration')
	parser.add_argument('--focal_length',type=int,required=True,help='The focal length (zoom setting) of the camera') #default=135 instead of required=True
	parser.add_argument('--sequence_file',type=argparse.FileType('r'),help='The file containing the sequence to use as a whitespace seperated list of 16-bit hex masks')
	args = parser.parse_args()
	
	raw_input("Make sure that the camera is ejected from the OS")
	ts=get_timestamp()
	os.makedirs(ts)
	os.chdir(ts)
	
	if(args.calibrate):
		capture_calibration(camera,scanner,focal_length=args.focal_length,do_background=True)

	capsequence=calibsequence
	if(args.sequence_file):
		capsequence=[int(x,16) for x in args.sequence_file.read().split()]
	
	settings={	'focal_length':args.focal_length,
			'background':bool(args.calibrate),
			'calibration':bool(args.calibrate),
			'sequence':["%02X" % (x) for x in capsequence] }
		
	with f=open(os.path.join('settings.json'),'w'):
		json.dump(settings,f)

	while(capture_object(camera,scanner,capsequence)):
		pass
