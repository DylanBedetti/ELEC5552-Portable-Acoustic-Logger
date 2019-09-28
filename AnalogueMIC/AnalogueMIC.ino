#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>
int cal=0;

#include <SD.h>
#include <SPI.h>

#define SD_ChipSelectPin 9
#define buffSize 128
#define ENABLE_RECORDING
TMRpcm audio;
int audiofile = 0;
unsigned long i = 0;
bool recmode = 0;
char filename[]="1.WAV";
long mic_loop_timer;
int count;
int auCount;
void setup(){
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(6, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  if (!SD.begin(SD_ChipSelectPin)) {
     delay(4000);
    Serial.println("Card failed, or not present");
    // don't do anything more:
   
  }  
  delay(4000);
  Serial.println("card initialized.");
  audio.CSPin = SD_ChipSelectPin;
  Serial.println("Finished Calibrating");

}

void loop(){
  
   if(mic_loop_timer==0|millis()-mic_loop_timer>=15000){//create a new wave file every 1/4 minutes
    if(count!=0){
     Serial.println("Recording Stopped: ");
     Serial.println(filename);
     audio.stopRecording(filename); 
    }
    delay(100000);
    count=1;
    auCount++;
    filename[0]=auCount+'0';
    Serial.println("Recording Started: ");
    Serial.println(filename); 
    audio.startRecording(filename, 40000, A0); 
    mic_loop_timer=millis();
  }
}
