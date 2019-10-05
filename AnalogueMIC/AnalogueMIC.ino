#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>

#define SD_ChipSelectPin 10

TMRpcm audio1;



char filenameA[]="A1.WAV"; //Audio Input file 1
long mic_loop_timer; //loop timer

int count;//generic counter
int auCount; //integer to itterate audio files


void setup(){
  Serial.begin(9600);
  pinMode(A1, INPUT_PULLUP); //setting audio analoginput A
  pinMode(5, OUTPUT);// LED PIN
  // Setting Up SD CARD
  
  if (!SD.begin(SD_ChipSelectPin)) {
     delay(4000);
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while(true){
      digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);      
    }
   
  }  
  delay(4000);
  Serial.println("card initialized.");
  audio1.CSPin = SD_ChipSelectPin;//setting audio object select pins
  
  Serial.println("Finished Calibrating");
  //analogReference(EXTERNAL);
}

void loop(){
                                 
   if(mic_loop_timer==0|millis()-mic_loop_timer>=15000){//create a new wave file every 1/4 minutes
    if(count!=0){ 
     Serial.println("Recording Stopped: ");
     Serial.println(filenameA);
     audio1.stopRecording(filenameA); 
    }
    delay(20000);
    count=1; // make sure you start stopping files
    auCount++;//make sure you increase file number
    filenameA[1]=auCount+'0';
    
    Serial.println("Recording Started: ");
    Serial.println(filenameA); 
    audio1.startRecording(filenameA, 40000, A0); 
    mic_loop_timer=millis();
  
  }
}
