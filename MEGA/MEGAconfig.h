//USER DEFINED SETTINGS
//=====================
#define MIC_TIME   30000 //30 seconds recording for each mic turned on (One at a time)
#define MIC_RATE   40000 //40kHz sampling
#define IMU_TIME   30000 //30 seconds recording acc (interleved between MICs)
#define IMU_PERIOD 4000  //4000uS or 250Hz
#define SLEEP      30000 //30 seocnd wait time between records of all data

//ADVANCED SETTINGS
//=====================-
#define MIC_OFF     0 //To turn the MIC off change this to 1
#define ACC_OFF     0 //To turn the ACC off change this to 2
#define MIC_2OFF    1 //Turn on a second MIC
#define MIC_3OFF    1 //Turn on a third MIC
