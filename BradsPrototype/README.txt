
Folders

*********************************
MIC5.0 was writing to microphone using 512 bit buffer and basic sdFat

MIC6.0 Records microphone using the TMRpcm .WAV library

SD_MPU2.7 is the combined SD card and MPU6500 which writes to a binary file

ReadMPU_BIN.py is a python srcipt for reading the binary file. You may need to open it up and change the name of the file.

*********************************
Arduino Conections!!!
============================
MPU-6050
============================
COMPONENT ->ARDUINO
-----------------------
SDA ->A4
SCL ->A5
GND ->GND
VCC->3.3V
=============================
SD Card Reader
=============================

CS->D4
SCk->D13
MOSI->D11
MISO->D12
VCC->5V
GND->GND

=============================
MAX4466 MIC
=============================
OUT->A0 
VCC->5V
GND->GND