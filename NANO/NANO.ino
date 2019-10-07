//Audio
#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>


//SD Card
#include <SPI.h>
#include <SD.h>

//MPU6050
#include <Wire.h>
//SD Card
#define SD_ChipSelectPin 53
#define IMU_TIME 30000 //30 seconds recording 
#define MIC_TIME 30000 //30 seconds recording
#define IMU_PERIOD 4000// 4000uS or 250Hz
#define MIC_RATE   40000// 40kHz sampling

#define GYRO_X_CAL 200
#define GYRO_Y_CAL 200
#define GYRO_Z_CAL 200
#define ACC_X_CAL 200
#define ACC_Y_CAL 200
#define ACC_Z_CAL 200


//Declaring Audio Variables
//***********************
TMRpcm audio1;
long mic_loop_timer; //loop timer
int auCount;
int count;
//Declaring some global variables IMU
//***********************************
int gyro_x, gyro_y, gyro_z;
int acc_x, acc_y, acc_z;
long IMU_timer;
long IMU_delay;



//File reset
long FILE_timer;

void setup() {
  //***********SETTING UP WRITES***************************
  
  Wire.begin();                                                        //Start I2C as master
  Serial.begin(9600);                                               //Use only for debugging                                                           // Set chip select on the SD
  pinMode(5, OUTPUT);// LED PIN
  pinMode(A1, INPUT);// MIC IN
  analogReference(EXTERNAL);//set analouge ref voltage to external source.
  //**********SETING UP SD CARD****************************
  // see if the card is present and can be initialized:
  //Serial.println("TEST");
  delay(4000);
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
  audio1.CSPin = SD_ChipSelectPin;// set pin for audio
  setup_mpu_6050_registers();                                          //Setup the registers of the MPU-6050 (500dfs / +/-2g) and start the gyro

  IMU_timer = micros(); 
}

void loop(){//***************MAIN LOOP************
//Serial.println("IMU_Start");
IMU_delay=millis();
while(IMU_delay==0||millis()-IMU_delay<=IMU_TIME){
  if(IMU_timer==0||micros()-IMU_timer>=IMU_PERIOD){
  IMU_opperate("ACC.bin");
  IMU_timer=micros();
  }
 }
 //Serial.println("IMU_Finished");
 if(mic_loop_timer==0|millis()-mic_loop_timer>=MIC_TIME){//create a new wave file every 1/4 minutes
    char filenameA[] ="A1.WAV";
    if(count!=0){
     Serial.println("Recording Stopped: ");
     Serial.println(filenameA);
     audio1.stopRecording(filenameA); 
    }
    count=1; // make sure you start stopping files
    auCount++;//make sure you increase file number
    filenameA[1]=auCount+'0';
    
    Serial.println("Recording Started: ");
    Serial.println(filenameA); 
    audio1.startRecording(filenameA, MIC_RATE, A0); 
    mic_loop_timer=millis();
  }
}

void MIC_opperate(){
    char filenameA[]="A0.WAV"; //Audio Input file 1
    auCount++;//make sure you increase file number
    filenameA[1]=auCount+'0';
    audio1.startRecording(filenameA,MIC_RATE, A0); 
    delay(MIC_TIME);
    audio1.stopRecording(filenameA);
}


void IMU_opperate(String fileName){
  read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050

  //Gyro angle calculations
  //0.0000611 = 1 / (250Hz / 65.5)
  //Note not dividing by 65/5 do this later !!
  gyro_x -= GYRO_X_CAL;                                                //Subtract the offset calibration value from the raw gyro_x value
  gyro_y -= GYRO_Y_CAL;                                                //Subtract the offset calibration value from the raw gyro_y value
  gyro_z -= GYRO_Z_CAL;                                                //Subtract the offset calibration value from the raw gyro_z value
  
  //Note not dividing by 16384 do this later !! Post proccessing
  acc_x -= ACC_X_CAL;
  acc_y -= ACC_Y_CAL;
  acc_z -= ACC_Z_CAL;
  
  //  //Write accelerometer values to file
  ////Serial.println("Writing");
  File accFile = SD.open(fileName, FILE_WRITE);
  write_binary_int(gyro_x,accFile);
  write_binary_int(gyro_y,accFile);
  write_binary_int(gyro_z,accFile);
  write_binary_int(acc_x,accFile);
  write_binary_int(acc_y,accFile);
  write_binary_int(acc_z,accFile);
  accFile.close();
}


void write_binary_int(int value, File file ){
  // converts the integer into 16bits 8 high and 8 low
  byte low = lowByte(value);
  byte high = highByte(value);
  file.write(high);
  file.write(low);
}

int read_binary_int(File file){
  //reads 2 bytes at a time to form 16 bit integers
 int val1=file.read()<<8|file.read();
 return val1;
}

void read_mpu_6050_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  acc_x = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable
  acc_y = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable
  acc_z = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
  Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
  gyro_x = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_x variable
  gyro_y = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_y variable
  gyro_z = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_z variable

}
  
void setup_mpu_6050_registers(){
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-2g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x08);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
}
