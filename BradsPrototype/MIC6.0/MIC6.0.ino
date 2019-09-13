#include <pcmConfig.h>
#include <pcmRF.h>
#include <TMRpcm.h>

/*
Steps:
1. Edit pcmConfig.h
    a: On Uno or non-mega boards, #define buffSize 128. May need to increase.
    b: Uncomment #define ENABLE_RECORDING and #define BLOCK_COUNT 10000UL

2. Usage is as below. See https://github.com/TMRh20/TMRpcm/wiki/Advanced-Features#wiki-recording-audio for
   additional informaiton.
*/

#include <SD.h>
#include <SPI.h>
#define SD_ChipSelectPin 10
TMRpcm audio;
char auFile[]="audio0.wav";
long mic_loop_timer;
int auCount;
int count;

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
  SD.begin(SD_ChipSelectPin);
  audio.CSPin = SD_ChipSelectPin;
  Serial.flush();
}

void loop() {
  if(mic_loop_timer==0|millis()-mic_loop_timer>=15000){//create a new wave file every 1/4 minutes
    if(count!=0){
     Serial.println("Recording Stopped: ");
     Serial.println(auFile);
     audio.stopRecording(auFile); 
     Serial.flush();
    }
    count=1;
    auCount++;
    auFile[5]=auCount+'0';
    Serial.println("Recording Started: ");
    Serial.println(auFile); 
    audio.startRecording(auFile, 40000, A0); 
    mic_loop_timer=millis();
  }
 
  
}
