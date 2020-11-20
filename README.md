# Laser-Face-Scanner

Code for project that uses facial recognition on a Raspberry Pi Camera to display a face outline using a custom laser scanner

## Parts Used:
- [Raspberry Pi 3]()
- Raspberry Pi [Camera V2.1](https://www.digikey.com/en/products/detail/raspberry-pi/913-2664/6152810s)
- [Teensy 4.0](https://www.digikey.com/en/products/detail/sparkfun-electronics/DEV-16997/13158150)
- [Galvanometer Scanners](https://www.aliexpress.com/item/1901995485.html?spm=a2g0o.productlist.0.0.20cc784aRRpdWG&algo_pvid=0dd55446-5275-4fb7-b43f-9c5ecc3d0831&algo_expid=0dd55446-5275-4fb7-b43f-9c5ecc3d0831-1&btsid=0bb0622e16004588429046424ec629&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
- 1x [laser](https://www.amazon.com/532MD-30-532nm-4-2-5VDC-Green-Module/dp/B00S3DX5G2/ref=sr_1_4?dchild=1&keywords=green+laser+ttl&qid=1600451311&sr=8-4)
- 2x 2-channel Op-Amps ([TL082CP](https://www.digikey.com/en/products/detail/texas-instruments/TL082CP/277630))
- 1x 2-channel DAC ([MCP4822-E/P](https://www.digikey.com/en/products/detail/microchip-technology/MCP4822-E-P/951465))
- 2x voltage regulators ([L7805CV](https://www.digikey.com/en/products/detail/stmicroelectronics/L7805CV/585964))
- Whole bunch of wires, breadboards, resistors, and cords

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
