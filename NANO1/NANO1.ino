
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
//MPU6050
#include <Wire.h>

//SD Card
#define SD_ChipSelectPin 53

//USER DEFINED SETTINGS
//=====================
#define MIC_TIME 30000 //30 seconds recording mic
#define MIC_RATE   40000// 40kHz sampling
#define IMU_TIME 30000 //30 seconds recording acc
#define IMU_PERIOD 4000// 4000uS or 250Hz


TMRpcm audio1;
int auCount;
//static const int RXPin = 19, TXPin = 18;
static const uint32_t GPSBaud = 9600;

//Declaring some global variables IMU
//***********************************
int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z;
int temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal,acc_x_cal,acc_y_cal,acc_z_cal;
int angle_x, angle_y,angle_z,aX,aY,aZ;
long IMU_timer;
long IMU_delay;

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
    break;
    }  
   }
  }
 //digitalWrite(6, HIGH);
 Serial.println("GPS WORKED");
 //*******CALIBRATING THE ACC AND GYRO********************
 setup_mpu_6050_registers(); //setup accelerometer 
 calibrate_acc_gyros();
 Serial.println("finished calabrating acc");   
}


void loop(){//***************MAIN LOOP************
  GPS_write();
  String time1=update_time();
  MIC_opperate(time1);
  time1=update_time();//update time again
  IMU_delay=millis();
  
    while(IMU_delay==0||millis()-IMU_delay<=IMU_TIME){
    if(IMU_timer==0||micros()-IMU_timer>=IMU_PERIOD){
    IMU_opperate(time1);
    IMU_timer=micros();
    }
  }
}


String update_time(){
  TinyGPSPlus gps;
  while(true){
   if(Serial1.available()>0){
   gps.encode(Serial1.read());
   if( gps.time.isUpdated()&&gps.date.isUpdated()){
    String temp;
    String val;
    temp.reserve(12);
    val.reserve(2);
    temp=(String)gps.date.day();
    if(temp.length()<2){
      temp="0"+temp;
     }
    val=(String)gps.time.hour();
    if(val.length()<2){
      val="0"+val;
     }
     temp+=val;
    val=(String)gps.time.minute();
    if(val.length()<2){
      val="0"+val;
     }
     temp+=val;
    val=(String)gps.time.second();
    if(val.length()<2){
      val="0"+val;
     }
    temp+=val;
    Serial.println(temp);
    return temp;
    break;
    }  
   }
  }
}

void MIC_opperate(String time1){
    String filename;
    filename.reserve(13);
    filename=time1+".MC1";
    char file[13]; //DDHHMMSS.MC1
    filename.toCharArray(file,13);
    audio1.startRecording(file,MIC_RATE, A0); 
    delay(MIC_TIME);
    audio1.stopRecording(file); 
    delay(10000);
}

void GPS_write(){
 char fileName[]="gps.log";
 File gpsFile = SD.open(fileName,FILE_WRITE);
 TinyGPSPlus gps;
  while(true){
   if(Serial1.available()>0){
   gps.encode(Serial1.read());
   if( gps.location.isUpdated()&&gps.date.isUpdated()&&gps.time.isUpdated()){
    String date2;
    String time2;
    String lat2;
    String lng2;
    date2.reserve(6);
    time2.reserve(8);
    date2=(String) gps.date.value();
    time2=(String) gps.time.value();
    lat2=(gps.location.rawLat().negative ? "-" : "+")+(String) gps.location.rawLat().deg+"."+(String)gps.location.rawLat().billionths;
    lng2=(gps.location.rawLng().negative ? "-" : "+")+(String) gps.location.rawLng().deg+"."+(String)gps.location.rawLng().billionths;
    gpsFile.print(date2+","+time2+",");
    gpsFile.print(lat2+",");
    gpsFile.print(lng2+"\n");
    gpsFile.close();
    break;
    }  
   }
  } 
}

void IMU_opperate(String time2){
  read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050
  
  String filename;
  filename.reserve(13);
  filename=time2+".ACC";
  char file[13];
  
  gyro_x -= gyro_x_cal;                                                //Subtract the offset calibration value from the raw gyro_x value
  gyro_y -= gyro_y_cal;                                                //Subtract the offset calibration value from the raw gyro_y value
  gyro_z -= gyro_z_cal;                                                //Subtract the offset calibration value from the raw gyro_z value
  
  acc_x -= acc_x_cal;
  acc_y -= acc_y_cal;
  acc_z -= acc_z_cal;
  
  //Gyro angle calculations
  //0.0000611 = 1 / (250Hz / 65.5)
  //Note not dividing by 65/5 do this later !!
  angle_x = gyro_x;                                   //Calculate the traveled angular velocity for x and add this to the angle_y variable
  angle_y = gyro_y;                                   //Calculate the traveled angular velocity for y angle and add this to the angle_y variable
  angle_z = gyro_z;                                  // Calculate the traveled angular velocity for z and add this to angle_Z

  //Note not dividing by 16384 do this later !! Post proccessing
  aX=acc_x;
  aY=acc_y;
  aZ=acc_z;
  
  //  //Write accelerometer values to file
  ////Serial.println("Writing");
  File accFile = SD.open(filename, FILE_WRITE);
  write_binary_int(angle_x,accFile);
  write_binary_int(angle_y,accFile);
  write_binary_int(angle_z,accFile);
  write_binary_int(aX,accFile);
  write_binary_int(aY,accFile);
  write_binary_int(aZ,accFile);
  accFile.close();
}

void write_binary_int(int value, File file ){
  // converts the integer into 16bits 8 high and 8 low
  byte low = lowByte(value);
  byte high = highByte(value);
  file.write(high);
  file.write(low);
}

//accelerometer reading and writing serial
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

void calibrate_acc_gyros(){
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++){                  //Run this code 2000 times
    read_mpu_6050_data();                                              //Read the raw acc and gyro data from the MPU-6050
    gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
    gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
    gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
    
    acc_x_cal  += acc_x;                                               
    acc_y_cal  += acc_y;
    acc_z_cal  += acc_z;
    delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
  }
  gyro_x_cal /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 2000;                                                  //Divide the gyro_z_cal variable by 2000 to get the avarage offset
  
  acc_x_cal/=2000;
  acc_y_cal/=2000;
  acc_z_cal/=2000;
  
  }
