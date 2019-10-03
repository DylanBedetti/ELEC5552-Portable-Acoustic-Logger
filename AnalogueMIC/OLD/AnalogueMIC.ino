#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>
int cal=0;

#include <SD.h>
#include <SPI.h>

#define SD_ChipSelectPin 8

TMRpcm audio1;
TMRpcm audio2;
TMRpcm audio3;


char filenameA[]="A1.WAV"; //Audio Input file 1
char filenameB[]="B1.WAV"; //Audio Input file 2
char filenameC[]="C1.WAV"; //Audio Inout file 3
long mic_loop_timer; //loop timer

int count;//generic counter
int auCount; //integer to itterate audio files


void setup(){
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP); //setting audio analoginput A
  pinMode(A1, INPUT_PULLUP); //setting audio analog input B
  pinMode(A2, INPUT_PULLUP); //setting audio analog input C

  // Setting Up SD CARD
  
  if (!SD.begin(SD_ChipSelectPin)) {
     delay(4000);
    Serial.println("Card failed, or not present");
    // don't do anything more:
   
  }  
  delay(4000);
  Serial.println("card initialized.");
  audio1.CSPin, audio2.CSPin, audio2.CSPin = SD_ChipSelectPin;//setting audio object select pins
  
  Serial.println("Finished Calibrating");
  int sum;
  
}

void loop(){
                                 
   if(mic_loop_timer==0|millis()-mic_loop_timer>=15000){//create a new wave file every 1/4 minutes
    if(count!=0){ 
     Serial.println("Recording Stopped: ");
     Serial.println(filenameA);
     audio1.stopRecording(filenameA); 
     Serial.println(filenameB);
     audio2.stopRecording(filenameB);
     Serial.println(filenameC);
     audio3.stopRecording(filenameC); 
    }
    delay(50000);
    count=1; // make sure you start stopping files
    auCount++;//make sure you increase file number
    filenameA[1]=auCount+'0';
    filenameB[1]=auCount+'0';
    filenameC[1]=auCount+'0';
    
    Serial.println("Recording Started: ");
    Serial.println(filenameA); 
    audio1.startRecording(filenameA, 20000, A0); 
    Serial.println(filenameB); 
    audio2.startRecording(filenameB, 20000, A1);
    Serial.println(filenameC); 
    audio3.startRecording(filenameB, 20000, A2);
    mic_loop_timer=millis();
  }
}
