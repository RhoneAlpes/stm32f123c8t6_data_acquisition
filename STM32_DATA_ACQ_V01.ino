/*

Jean-Pierre Redonnet
inphilly@gmail.com
Version 0.1 - July 25th,2015

Licence: GNU GPL 2

Change adc_set_sample_rate(...) to decrease conversion time,
setting:
dev->regs->SMPR1 = 0; 
dev->regs->SMPR2 = 0;
(new conversion time is 2.5µs)

Go to:
Arduino_STM32-master/STM32F1/cores/maple/libmaple/adc.c
void adc_set_sample_rate(...) 
{
    dev->regs->SMPR1 = 0;
    dev->regs->SMPR2 = 0;
}

 */

#define BUFFERSIZE 1000

int sensorPin1 = PA4; 
int sensorPin2 = PA5; 
int i;
int val1[BUFFERSIZE+1];
int val2[BUFFERSIZE+1];
int led1 = PC13; 
int led2 = PC14; 
int led3 = PC15; 
char serialReadString[50];
boolean waitFlag;

int serialReader() {
  int makeSerialStringPosition;
  int inByte;
  
  if ( Serial.available() < 3 ) return 0; //nothing
  
  const int terminatingChar = 13; // Terminate lines with CR

  inByte = Serial.read();
  makeSerialStringPosition=0;

  if (inByte > 0 && inByte != terminatingChar) { //If we see data (inByte > 0) and that data isn't a carriage return
    delay(10); //Allow serial data time to collect

    while (inByte != terminatingChar && Serial.available() > 0){ // As long as EOL not found and there's more to read, keep reading
      serialReadString[makeSerialStringPosition] = inByte; // Save the data in a character array
      makeSerialStringPosition++; // Increment position in array
      inByte = Serial.read(); // Read next byte
    }

    if (inByte == terminatingChar) { //If we terminated properly
      serialReadString[makeSerialStringPosition] = 0; //Null terminate the serialReadString (Overwrites last position char (terminating char) with 0
      return(makeSerialStringPosition);
    }
    else return(0);
  }
}
 
 
void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

void loop() {
  //wait for start
  digitalWrite(led3, HIGH);
  waitFlag=1;  
  while(waitFlag) {
    if (serialReader() > 0 ) {
      if (strncmp("startADC",serialReadString,2) == 0) {
        waitFlag=0;
      }
    }
  }
  digitalWrite(led3, LOW);
  //read analog inputs
  i=0;
  digitalWrite(led1, HIGH);
  int t1=micros();
  while (i < BUFFERSIZE) {
    //TODO: new function able to start both ADC
    val1[i]=analogRead(sensorPin1);
    val2[i]=analogRead(sensorPin2);
    i++;
  }
  int t2=micros();
  digitalWrite(led1, LOW);
  // Send data
  digitalWrite(led2, HIGH);
  Serial.println();
  Serial.println("Duration");
  Serial.println(t2-t1);
  i=0;
  while (i < BUFFERSIZE) {
    Serial.print(val1[i]);
    Serial.print("/");
    Serial.print(val2[i]);
    Serial.print("-");
    i++;
  }
  Serial.println(); //terminate the line
  Serial.println();
  Serial.flush();
  digitalWrite(led2, LOW);
}
