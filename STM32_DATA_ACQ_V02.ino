/*

Jean-Pierre Redonnet
inphilly@gmail.com
Version 0.2 - July 26th,2015

Fast dual conversion with ADC1 + ADC2

Licence: GNU GPL 2

*/

#define BUFFERSIZE 1000

int sensorPin1 = PA1; 
int sensorPin2 = PA2; 
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
  
  pinMode(PA1,INPUT_ANALOG);
  pinMode(PA2,INPUT_ANALOG);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

// We configure the ADC1 and ADC2

//1.5µs sample time  
adc_set_prescaler(ADC_PRE_PCLK2_DIV_2);
adc_set_sample_rate(ADC1, ADC_SMPR_1_5);
adc_set_sample_rate(ADC2, ADC_SMPR_1_5);

//6=0110 for dual regular simultaneous mode    
ADC1->regs->CR1 |= 6 << 16;

 //only one input in the sequence for both ADC
adc_set_reg_seqlen(ADC1, 1);
adc_set_reg_seqlen(ADC2, 1);

//channel 1 (PA1) on ADC1
ADC1->regs->SQR1|=0;
ADC1->regs->SQR2=0;
ADC1->regs->SQR3=1; //00000.00000.00001
//channel 2 (PA2) on ADC2
ADC2->regs->SQR1|=0;
ADC2->regs->SQR2=0; //00000.00000.00000
ADC2->regs->SQR3=2; //00000.00000.00010

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
    //start conversion
    ADC1->regs->CR2 |= ADC_CR2_SWSTART;
    // Wait the end of the conversion
    while (!(ADC1->regs->SR & ADC_SR_EOC)) ;
    while (!(ADC2->regs->SR & ADC_SR_EOC)) ;
    //get the values converted
    val1[i]=(int16)(ADC1->regs->DR & ADC_DR_DATA);
    val2[i]=(int16)(ADC2->regs->DR & ADC_DR_DATA);
    //next
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
