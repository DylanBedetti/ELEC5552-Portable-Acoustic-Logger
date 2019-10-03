#include <SD.h>
#include <ArduinoSound.h>

AmplitudeAnalyzer amp;
File micRead ;
short count;
byte micBuffer[512];
short shortIn;
int MicIndex;
const int chipSelect = SS1;

void setup(){
// A baud rate of 115200 is used instead of 9600 for a faster data rate
  // on non-native USB ports
  micRead = SD.open("microphone_test.bin", FILE_WRITE);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // setup the I2S audio input for 44.1 kHz with 32-bits per sample
  if (!AudioInI2S.begin(44100, 32)) {
    Serial.println("Failed to initialize I2S input!");
    while (1); // do nothing
  }

  // configure the I2S input as the input for the amplitude analyzer
  if (!amp.input(AudioInI2S)) {
    Serial.println("Failed to set amplitude analyzer input!");
    while (1); // do nothing
  }

  Serial.println("TEST");
  delay(4000);
  if (!SD.begin(chipSelect)) {
     delay(4000);
    Serial.println("Card failed, or not present");
    // don't do anything more:
   
  }  
  delay(4000);
  Serial.println("card initialized.");
}

void loop(){
    if(count == 0) micRead = SD.open("microphone_test.bin", FILE_WRITE);
 // check if a new analysis is available
  if (amp.available()) {
    // read the new amplitude
    int in = amp.read();
    shortIn = in>>16;

    if(MicIndex<=511){
      byte high=highByte(shortIn);
      byte low=lowByte(shortIn);
     micBuffer[MicIndex++]=high;
     micBuffer[MicIndex++]=low;
     
     if(MicIndex==512){
     Serial.println(micRead.write(micBuffer,MicIndex));
     micRead.close(); //write to SD card
     MicIndex=0;
     }
    }
    
    // print out the amplititude to the serial monitor
    //Serial.println(in>>16);
    
    
  }
  count = 1;
}
