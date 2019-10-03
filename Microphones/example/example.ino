/*
 This example reads audio data from an Invensense's ICS43432 I2S microphone
 breakout board, and prints out the amplitude to the Serial console. The
 Serial Plotter built into the Arduino IDE can be used to plot the audio
 amplitude data (Tools -> Serial Plotter)

 Circuit:
 * Arduino/Genuino Zero, MKRZero or MKR1000 board
 * ICS43432:
   * GND connected GND
   * 3.3V connected 3.3V (Zero) or VCC (MKR1000, MKRZero)
   * WS connected to pin 0 (Zero) or pin 3 (MKR1000, MKRZero)(LRCLK)- word select 
   * CLK connected to pin 1 (Zero) or pin 2 (MKR1000, MKRZero) (BCLK)
   * SD connected to pin 9 (Zero) or pin A6 (MKR1000, MKRZero) (DOUT)

 created 23 November 2016
 by Sandeep Mistry
 */

#include <ArduinoSound.h>

// create an amplitude analyzer to be used with the I2S input
AmplitudeAnalyzer amplitudeAnalyzer;

void setup() {
  // Open serial communications and wait for port to open:
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  // on non-native USB ports
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("starting!!");

  // setup the I2S audio input for 44.1 kHz with 32-bits per sample
  if (!AudioInI2S.begin(30000, 32)) { //44100 32
    Serial.println("Failed to initialize I2S input!");
    while (1); // do nothing
  }

  // configure the I2S input as the input for the amplitude analyzer
  if (!amplitudeAnalyzer.input(AudioInI2S)) {
    Serial.println("Failed to set amplitude analyzer input!");
    while (1); // do nothing
  }
  Serial.print("sample rate = ");
  Serial.println(AudioInI2S.sampleRate()); // prints out the sample rate used in begin(...)
  Serial.print("bit per sample = ");
  Serial.println(AudioInI2S.bitsPerSample()); // prints out the bits per sample rate used  in begin(...)
  Serial.print("channels = ");
  Serial.println(AudioInI2S.channels()); // prints out 2, I2S always has 2 channels
}

void loop() {
  // check if a new analysis is available
  if (amplitudeAnalyzer.available()) {
    // read the new amplitude
    int amplitude = amplitudeAnalyzer.read();
//    int amplitude_1 = amplitudeAnalyzer.read();

    // print out the amplititude to the serial monitor
//    Serial.println(amplitude, BIN);
    int a = 0;
    for (int i = 27; i >= 0; i--){
      a += bitRead(amplitude, i);
      a = a << 1;
    }
    a = a >> 1;
    
//    Serial.print(a, BIN);
//    Serial.print("  ");
    Serial.println(a);
//    Serial.print("  ");
//    Serial.println(amplitude, BIN);
  }

}