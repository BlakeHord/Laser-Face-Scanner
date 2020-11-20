from imutils.video import VideoStream
from imutils import face_utils
import datetime
import imutils
import time
import dlib
import cv2
import serial
import numpy as np
import RPi.GPIO as gpio
 
def reface (shape):
	chin = shape[0:17]
	Reyebrow = shape[17:22]
	Leyebrow = shape[22:27]
	nose = shape[27:36]
	Reye = shape[36:42]
	Reye = np.append(Reye, [shape[36]], axis = 0)
	Leye = shape[45:48]
	Leye = np.append(Leye, shape[42:46], axis = 0)
	mouth = shape[48:68]
	
	newface = chin #add 17 points (0-16)
	newface = np.append(newface, Leye, axis = 0) #add 7 points (17-23)
	newface = np.append(newface, np.flip(Leyebrow, 0), axis = 0) #add 5 points (24-28)
	newface = np.append(newface, np.flip(Reyebrow, 0), axis = 0) #add 5 points (29-33)
	newface = np.append(newface, Reye, axis = 0) #add 7 points (34-40)
	newface = np.append(newface, nose, axis = 0) #add 9 points (41-49)
	newface = np.append(newface, mouth, axis = 0) #add 20 points (50-69)
	
	return newface
 
def sendSequence(shape, port):
	start_points_i = [17,24,29,34,41,50,70]
	toWrite = "<"
	
	newface = reface(shape)
	
	for i, (x,y) in enumerate(newface):
		if (i in start_points_i):
			toWrite += "5000 "
			#print("break")
		if i < 69:
			toWrite += str(x*10) + ' '
			toWrite += str(4096-y*10) + ' ' 
		else:
			break
				
	toWrite += ">"
	print(toWrite)
	
	port.write(toWrite.encode())
	time.sleep(0.000016*len(toWrite))
	
	ctr = 1
	while 1:
		if ser.in_waiting:
			#print(ser.in_waiting)
			print("received")
			ser.reset_input_buffer()
			ser.reset_output_buffer()
			break
		else:
			port.write(toWrite.encode())
			time.sleep(0.000016*len(toWrite))
			print("sent " + str(ctr))
			ctr = ctr + 1
	
 
baud = 2000000

ser = serial.Serial("/dev/ttyS0", baudrate=baud)#, bytesize=sm.serial.EIGHTBITS)#, parity=sm.serial.PARITY_EVEN)
time.sleep(0.1) #delay to open serial
ser.flushInput()
ser.flushOutput()
#ser.write("0 0 0 4000 4000 4000 4000 0 0 0 ".encode())

rts = 11 #gpio pin 17
cts = 36 #gpioin 16

gpio.setmode(gpio.BOARD)
gpio.setup(rts, gpio.OUT)
gpio.setup(cts, gpio.IN, pull_up_down=gpio.PUD_UP)

verbose = False
 
# initialize dlib's face detector (HOG-based) and then create
# the facial landmark predictor
print("[INFO] loading facial landmark predictor...")
detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor("predictor.dat")

# initialize the video stream and allow the cammera sensor to warmup
print("[INFO] camera sensor warming up...")
vs = VideoStream(usePiCamera=True).start()
time.sleep(0.5) #delay to open camera

# loop over the frames from the video stream
while True:
	# grab the frame from the threaded video stream, resize it to
	# have a maximum width of 400 pixels, and convert it to
	# grayscale
	frame = vs.read()
	frame = imutils.resize(frame, width=400) #shrink image
	frame = imutils.rotate(frame, 180) #flip image to make it rightside-up
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

	# detect faces in the grayscale frame
	rects = detector(gray, 0)

	# loop over the face detections
	for rect in rects:
		# determine the facial landmarks for the face region, then
		# convert the facial landmark (x, y)-coordinates to a NumPy
		# array
		shape = predictor(gray, rect)
		shape = face_utils.shape_to_np(shape)

		# loop over the (x, y)-coordinates for the facial landmarks
		# and draw them on the image
		x_1 = -1
		y_1 = -1
		start_points_i = [0,17,22,27,36,42,48,68]
		for i, (x, y) in enumerate(shape):
			#print(x, y);
			#cv2.circle(frame, (x, y), 1, (0, 0, 255), -1)
			if not (i in start_points_i):
				cv2.line(frame, (x,y), (x_1, y_1), (255,0,0), 1)	
			x_1 = x
			y_1 = y
		
		while gpio.input(cts):
			time.sleep(0.000001) #sleep until cts pin is pulled down
		
		#then send the sequence until something is received
		sendSequence(shape, ser)
		print("sent")
		
	# show the frame
	if (verbose == True):
		cv2.imshow("Frame", frame)
		key = cv2.waitKey(1)
 
	# if the `q` key was pressed, break from the loop
	#if key == ord("q"):
	#	break
 
# do a bit of cleanup
cv2.destroyAllWindows()
vs.stop()
