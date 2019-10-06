

//Audio
#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>


//SD Card
#include <SPI.h>
#include <SD.h>
//GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//SD Card
#define SD_ChipSelectPin 10
#define MIC_TIME 30000 //30 seconds recording
#define MIC_RATE   40000// 40kHz sampling



TMRpcm audio1;
int auCount;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object

String DATE;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
void setup() {  
  ss.begin(GPSBaud);
  TinyGPSPlus gps;
  //Use only for debugging                                                           // Set chip select on the SD
  pinMode(5, OUTPUT);// LED PIN
  pinMode(6, OUTPUT);// LED PIN
  pinMode(A1, INPUT);// MIC IN
  analogReference(EXTERNAL);//set analouge ref voltage to external source.
  //**********SETING UP SD CARD****************************
  // see if the card is present and can be initialized:
  //Serial.println("TEST");
  delay(4000);
  if (!SD.begin(SD_ChipSelectPin)) {
     delay(4000);
    //Serial.println("Card failed, or not present");
    // don't do anything more:
    while(true){
      digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);      
    }
  }  
  delay(4000);
  //Serial.println("card initialized.");
  audio1.CSPin = SD_ChipSelectPin;// set pin for audio

  while(true){
   if(ss.available()>0){
    
   gps.encode(ss.read());
   if( gps.date.isUpdated()){
    digitalWrite(6, LOW);
    DATE=(String)gps.date.value(); 
    
    SD.mkdir(DATE);
    break;
    }  
   }
  }
 digitalWrite(6, HIGH);
     
}


void loop(){//***************MAIN LOOP************
  String time1=update_time();
 delay(15000);//15 second delay while the buffer closes
 MIC_opperate(time1);
}


String update_time(){
  TinyGPSPlus gps;
  while(true){
   if(ss.available()>0){
   gps.encode(ss.read());
   if( gps.time.isUpdated()){
    digitalWrite(6, LOW);
    String temp;
    temp.reserve(9);
    temp=(String)gps.time.value();
    return temp;
    break;
    }  
   }
  }
}

void MIC_opperate(String time1){
    String filename;
    filename.reserve(19);
    filename=time1+".WAV";
    char file[12]; //DDMMYY/HHMMSSCC.wav
    filename.toCharArray(file,19);
    audio1.startRecording(file,MIC_RATE, A0); 
    delay(MIC_TIME);
    audio1.stopRecording(file); 
}
