#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>

//SD Card

#include <SPI.h>

//MPU6050
#include <Wire.h>
SdFat SD;
int index=0;
byte buff[512];

//Declaring some global variables

long acc_loop_timer,mic_loop_timer,minTime;

int count=0;

File micFile;
//Define sampling rates 
#define ACC_PERIOD 4000 //Set the accelerometer period to 4000 uS or (250 Hz freq)
#define MIC_PERIOD 25   //Set the microphone period to 25 uS or (40 000 Hz freq)
String file2="FASTEST2.bin";

 
 File f;
void setup() {
  Wire.begin();                                                        //Start I2C as master
  //Serial.begin(9600);                                               //Use only for debugging
  //pinMode(13, OUTPUT);                                                 //Set output 13 (LED) as output                                                    // Set chip select on the SD
  pinMode(A0,INPUT);// set the analog pin A0 as input
  //=======================================================
  //**********SETING UP SD CARD****************************
  //=======================================================
 
  if (!SD.begin(4)) {
    //Serial.println("initialization failed!");
    while (1);
  }
  //Serial.println("initialization done.");+

}

void loop(){
//
//if(minTime==0|(millis()-minTime)>=36000){
if(count==0){
 micFile=SD.open(file2,O_WRITE|O_CREAT);
 //Serial.println("Writting to binary...");
 count++;
}


//=====================================================
//Start The microphone main loop
//=====================================================
if(mic_loop_timer==0|micros()-mic_loop_timer>=25){
  unsigned int val=analogRead(0);
  
  byte low= lowByte(val);
  byte high = highByte(val);
  buff[index++]=high;
  buff[index++]=low;
 
  
  if(index>=511) {
    micFile.write(buff,index);
    micFile.flush();
    index=0;
  }
 
  
  mic_loop_timer=micros();
}
                                                    
}
