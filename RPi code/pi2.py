import serial
import time
import sys
from picamera import PiCamera
import subprocess
import shlex

port = '/dev/ttyACM0' # may need to be updated.
#ttyAMA1
#ttyS0
arduino = serial.Serial(port, 9600,timeout=1)

camera = PiCamera(resolution=(1280,720), framerate=30)

def takepic(n):
	'''
	Takes a picture with the digital and thermal cameras,
	waits a second, then takes another digital
	'''
#	camera.start_preview() # initialize color balance
	time.sleep(1)
	name = str(2*n)
	camera.capture(name+'.jpg') # capture first image, then thermal
#	subprocess.call(shlex.split('./pylepton_capture '+name+'thermal.jpg'))
	time.sleep(1) # wait for arm to move
	camera.capture(str(2*n+1)+'.jpg')
#	camera.stop_preview()



##################
'''Main Program'''
##################

print('BEGIN')
n = 1

while True:
	try:
		time.sleep(0.01) # psuedo refresh rate
		if arduino.inWaiting()>0:
			if arduino.read()=='T':
				print('Capturing pictures')
				takepic(n)
				n = n+1
				
	except KeyboardInterrupt: # ctrl+c to stop
		print('Exiting')
		arduino.close()
		break


	
	
	
