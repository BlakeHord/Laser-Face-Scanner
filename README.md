# Laser-Face-Scanner

Code for project that uses facial recognition on a Raspberry Pi Camera to display a face outline using a custom laser scanner

## Parts Used:
- Raspberry Pi 3
- Raspberry Pi Camera V2.1
- Teensy 4.0
- Galvanometer Scanners
- 1x laser
- 2x 2-channel Op-Amps
- 1x 2-channel DAC
- 2x voltage regulators
- Whole bunch of wires, breadboards, and cords

## Software Requirements:
- Raspberry Pi: Python packages in requirements.txt (imutils, datetime, time, dlib, cv2, serial, numpy, RPi.GPIO)
	- Follow [this tutorial](https://www.pyimagesearch.com/2017/03/27/how-to-install-dlib/) to install imutils, dlib, and cv2. All others should be able to be installed with 'pip install'
- Teensy: Increase input serial buffer size from 64 bytes to 1024. Go to the file /Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/cores/teensy4/HardwareSerial3.cpp and change '#define SERIAL3_RX_BUFFER_SIZE 64' to '#define SERIAL3_RX_BUFFER_SIZE 1024'. 
	- This is needed to store the entire sequence message in the teensy's serial buffer so it can all be transmitted at once.

## Layout of code:
- main.ino: runs on teensy, receives input over serial from Raspberry Pi, sends output over SPI to DAC to control galvanometer mirror angles
- laser_face.py: runs on Raspberry Pi, receives input from camera, uses facial landmark predictor to output 68 points of your face and communicate them over serial to the teensy
	- You'll need to download the predictor file from [here](https://www.pyimagesearch.com/2017/04/10/detect-eyes-nose-lips-jaw-dlib-opencv-python/)


## Hardware layout: 
insert picture here


## ToDo:
- Make it much faster. The major bottleneck is the python facial recognition so maybe convert it to C++?
- Fix streaking when drawing the face
- Make a game with it? Run on Raspberry Pi and make an "eat the cookie" game or something like that
