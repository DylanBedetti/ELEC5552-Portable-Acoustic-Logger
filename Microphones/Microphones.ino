#include <SD.h>
#include <ArduinoSound.h>

AmplitudeAnalyzer amp;

void setup(){
// A baud rate of 115200 is used instead of 9600 for a faster data rate
  // on non-native USB ports
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
}

void loop(){

 // check if a new analysis is available
  if (amp.available()) {
    // read the new amplitude
    int in = amp.read();

    // print out the amplititude to the serial monitor
    Serial.println(in>>16);
    
  }
  
}
