

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
#define SD_ChipSelectPin 53
#define MIC_TIME 30000 //30 seconds recording
#define MIC_RATE   40000// 40kHz sampling



TMRpcm audio1;
int auCount;
//static const int RXPin = 19, TXPin = 18;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object

String DATE;
// The serial connection to the GPS device
//SoftwareSerial Serial1(RXPin, TXPin);
void setup() {  
  Serial.begin(9600);
  Serial1.begin(GPSBaud);
  TinyGPSPlus gps;
  //Use only for debugging                                                           // Set chip select on the SD
  pinMode(5, OUTPUT);// LED PIN
  pinMode(6, OUTPUT);// LED PIN
  pinMode(A1, INPUT);// MIC IN
  analogReference(EXTERNAL);//set analouge ref voltage to external source.
  //**********SETING UP SD CARD****************************
  // see if the card is present and can be initialized:
  //Serial.println("TEST");
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while(true){
      digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);      
    }
  }  
  Serial.println("card initialized.");
  audio1.CSPin = SD_ChipSelectPin;// set pin for audio

  while(true){
   if(Serial1.available()>0){
    
   gps.encode(Serial1.read());
   if( gps.date.isUpdated()){
    digitalWrite(6, LOW);
    DATE=(String)gps.date.value(); 
    
    SD.mkdir(DATE);
    break;
    }  
   }
  }
 //digitalWrite(6, HIGH);
 Serial.println("GPS WORKED");
     
}


void loop(){//***************MAIN LOOP************
  String time1=update_time();
 delay(15000);//15 second delay while the buffer closes
 MIC_opperate(time1);
}


String update_time(){
  TinyGPSPlus gps;
  while(true){
   if(Serial1.available()>0){
   gps.encode(Serial1.read());
   if( gps.time.isUpdated()&&gps.date.isUpdated()){
    String temp;
    temp=(String)gps.date.day();
    if(temp.length()<2){
      temp="0"+temp;
     }
    temp+=(String)gps.time.hour();
    if(temp.length()<4){
      temp="0"+temp;
     }
    temp+=(String)gps.time.minute();
    if(temp.length()<6){
      temp="0"+temp;
     }
    temp+=(String)gps.time.second();
    if(temp.length()<8){
      temp="0"+temp;
     }
    Serial.println(temp);
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
    char file[19]; //DDMMYY/HHMMSSCC.wav
    filename.toCharArray(file,19);
    audio1.startRecording(file,MIC_RATE, A0); 
    delay(MIC_TIME);
    audio1.stopRecording(file); 
    delay(10000);
}
