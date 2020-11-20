#define HWSERIAL Serial3
#include <SPI.h>

long baudrate = 2000000;
const int ctspin = 19, rtspin = 18;
char incomingByte;
int block_size = 75;
int16_t* instruction = NULL, *temp = NULL;
const int chipSelectPin = 10, resolution = 12, clockspeed = 20000000;
const int16_t frame = 4096;
const int16_t div_factor = frame/2;
int blankpin = 17;

int16_t* receiveInput(bool verb);
void printSequence(int16_t* instruction);
int runSequence(int16_t *instruction);
void laser_control(int onoff);
void toPoint(int16_t x, int16_t y);
int drawLin (int16_t start[], int16_t end[]);
void set_voltage(double voltage1, double voltage2);

void setup() {
  HWSERIAL.begin(baudrate);
  Serial.begin(baudrate);

  HWSERIAL.attachRts(rtspin); //output
  HWSERIAL.attachCts(ctspin); //input

  pinMode(chipSelectPin, OUTPUT);
  pinMode(blankpin, OUTPUT);
  digitalWrite(blankpin, LOW);  

  digitalWrite(rtspin, HIGH);

    // initialize SPI:
  SPI.begin(); 

  // Start at fixed value of 0 (should be bottom left)
  set_voltage(0.0, 0.0);

  delay(2000);
}


void loop() {
  //Serial.println();
  digitalWrite(rtspin, LOW); //ready to receive data
  //Serial.println("RTS pin low");
  //delayMicroseconds(2000); //short delay to register rtspin write

  temp = receiveInput(false);
  if (temp != NULL) {
    HWSERIAL.print(1);
    //printSequence(temp);
    instruction = temp;
  }


  //Serial.println();
  digitalWrite(rtspin, HIGH); //STOP receiving data
  //Serial.println("RTS pin high");
  HWSERIAL.clear();
  delayMicroseconds(4000); //short delay to register rtspin write
  runSequence(instruction);
}


int16_t* receiveInput(bool verb) {
  int incomingByte;
  bool startRec = false;
  if (HWSERIAL.available() > 0) {
    while(1) {
      incomingByte = HWSERIAL.read();
      if (incomingByte == '<') {
        if(verb) {Serial.print((char)incomingByte);}
        startRec = true;
        break;
      } else if (incomingByte == -1) {
        if(verb) {Serial.println("broken1");}
        return NULL;
      }
    }
  }

  int16_t *ptr = NULL, fullData = 0;
  ptr = (int16_t*)malloc(block_size*sizeof(int16_t));
  *ptr = 0; // set first value in ptr array to be 0 (size of sequence)s
  int ctr = 1, current_size = block_size;

  while(startRec) {
    incomingByte = HWSERIAL.read();

    if(incomingByte == '>') {
      if(verb) {Serial.print((char)incomingByte);}
      return ptr;
    } else if (incomingByte == -1) {
      if(verb) {Serial.println("broken");}
      free(ptr);
      return NULL;
    } else if (isDigit(incomingByte)) {
      fullData = fullData * 10 + (incomingByte - '0');
      if(verb) {Serial.print((char)incomingByte);}
    } else if (incomingByte == ' ') {
      *(ptr + ctr) = fullData; //store data in array
      ctr++;
      (*ptr)++; //increment value count
      if(verb) {Serial.print(fullData);}
      if(verb) {Serial.print(" ");}
      fullData = 0;
    }

    // allocate more data for array if necessary
    if ((*ptr + 2) == (current_size)) {
      int newsize = current_size + block_size;
      ptr = (int16_t*)realloc(ptr, newsize*sizeof(int16_t));
      current_size += block_size;
    }
  }

  free(ptr);
  return NULL;
}


void printSequence(int16_t* instruction) {
  int16_t len = *instruction;
  int16_t *temp = instruction;
  temp++;

  if (len == 0) {
    Serial.println("empty sequence");
  } else {
    Serial.println();
    Serial.print(len);
    Serial.print("Sequence: ");
  }

  
  for (int i = 0; i < len; i++) {
    Serial.print(*(temp+i));
    Serial.print(' ');
  }
}


// function to draw lines between a specific sequence of coordinates
int runSequence(int16_t *instruction) {
  if (instruction == NULL) {
    return 1;
  }
  int16_t sequence_size = *instruction; // collect size of sequence from first value in array
  instruction++; // move to next position (the actual first value in sequence)

  bool flag = true; // start with true so it initializes with previous value

  int16_t current[2], prev[2];
  
  for (int i = 0; i < sequence_size; i++) {
    if (*(instruction+i) == 5000) { //special code for skipping a line draw
      flag = true;
      //laser_control(0);
      // change flag to 1 so line is not drawn next increment
    } else if (flag == true) { 
      // skip drawline
      prev[0] = *(instruction+i);
      i++;
      prev[1] = *(instruction+i);

      //toPoint(prev[0], prev[1]);
      //Serial.println(prev[0]);
      //Serial.println(prev[1]);
      //Serial.println();
      //delayMicroseconds(10);
      flag = false;
    } else {
      // draw line from previous point to this point
      current[0] = *(instruction+i);
      i++;
      current[1] = *(instruction+i);

      delayMicroseconds(200);
      drawLin(prev, current);
      //laser_control(1);
      //delayMicroseconds(200);

      prev[0] = current[0];
      prev[1] = current[1];
    }
  } 
  //laser_control(0);
  //delayMicroseconds(100);
  return 0;
}


void laser_control(int onoff) {
  if(onoff == 1) {
    digitalWrite(blankpin, HIGH);
  } else if (onoff == 0) {
    digitalWrite(blankpin, LOW);
  }
}


void toPoint(int16_t x, int16_t y) {
  int delay_val = 200;
  laser_control(0);
  delayMicroseconds(delay_val);
  set_voltage(x/div_factor, y/div_factor);
  delayMicroseconds(delay_val);
  laser_control(1);
}


int drawLin (int16_t start[], int16_t end[]) {
  if (start[0] < 0 || start[1] < 0 || start[0] > frame || start[1] > frame)  {
    return 1;
  } else if (end[0] < 0 || end[1] < 0 || end[0] > frame || end[1] > frame) {
    return 1;
  } else {
    //delayMicroseconds(100);
    //toPoint(start[0], start[1]);
    laser_control(1);
    double dist = sqrt(pow(end[1]-start[1],2) + pow(end[0] - start[0],2));
    int n = round(dist/3); //change factor of 3 to something higher for coarser resolution
    for (double i = 0; i <= n; i++) {
      double x_voltage = (double)(start[0] + i/n * (end[0]-start[0]))/div_factor;
      double y_voltage = (double)(start[1] + i/n * (end[1]-start[1]))/div_factor;
      set_voltage(x_voltage, y_voltage);
    }
    //delayMicroseconds(10);
    return 0;
  }
}


void set_voltage(double voltage1, double voltage2) {
    int val1 = voltage1 * pow(2, resolution)/2.048;
    int val2 = (2-voltage2) * pow(2, resolution)/2.048;
    SPI.beginTransaction(SPISettings(clockspeed, MSBFIRST, SPI_MODE0));
    digitalWrite(chipSelectPin, LOW);

    unsigned char data1 = 48 | (val1 >> 8); //0011 + first four bits of number
    unsigned char data2 = (val1 & 255); //last 8 bits of number
    
    SPI.transfer(data1);
    SPI.transfer(data2);
    
    digitalWrite(chipSelectPin, HIGH);
    SPI.endTransaction();

    SPI.beginTransaction(SPISettings(clockspeed, MSBFIRST, SPI_MODE0));
    digitalWrite(chipSelectPin, LOW);
    
    unsigned char data3 = 176 | (val2 >> 8); //0011 + first four bits of number
    unsigned char data4 = (val2 & 255); //last 8 bits of number
    
    SPI.transfer(data3);
    SPI.transfer(data4);

    digitalWrite(chipSelectPin, HIGH);
    SPI.endTransaction();
}
