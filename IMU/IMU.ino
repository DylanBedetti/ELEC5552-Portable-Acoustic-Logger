//SD Card
#include <SPI.h>
#include <SD.h>

//MPU6050
#include <Wire.h>

//Declaring some global variables
short gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
short temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal,acc_x_cal,acc_y_cal,acc_z_cal;
long IMU_timer;
short lcd_loop_counter;
float angle_x, angle_y,angle_z,aX,aY,aZ;
short angle_x_buffer, angle_y_buffer;
boolean set_gyro_angles;
short angle_y_acc, angle_x_acc;
float angle_x_output, angle_y_output;
int count=0;
File accFile;
//SD CARD 
const int chipSelect = SS1;

void setup() {
  //***********SETTING UP WRITES***************************
  
  Wire.begin();                                                        //Start I2C as master
  Serial.begin(115200);                                               //Use only for debugging                                                           // Set chip select on the SD
  
  //**********SETING UP SD CARD****************************
  // see if the card is present and can be initialized:
  Serial.println("TEST");
  delay(4000);
  if (!SD.begin(chipSelect)) {
     delay(4000);
    Serial.println("Card failed, or not present");
    // don't do anything more:
   
  }  
  delay(4000);
  Serial.println("card initialized.");
  setup_mpu_6050_registers();                                          //Setup the registers of the MPU-6050 (500dfs / +/-2g) and start the gyro

  //*******CALIBRATING THE ACC AND GYRO********************
  calibrate_acc_gyros();
  Serial.println("finished calabrating acc");
  IMU_timer = micros(); 
}

void loop(){//***************MAIN LOOP************
if(IMU_timer==0||micros()-IMU_timer>=4000){
  IMU_timer=micros();
  }    
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


void IMU_opperate(char* fileName){
  read_mpu_6050_data();                                                //Read the raw acc and gyro data from the MPU-6050

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
  accFile = SD.open(fileName, FILE_WRITE);
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
  temperature = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
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
