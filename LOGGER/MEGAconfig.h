//USER DEFINED SETTINGS
//=====================
#define MIC_TIME      30000 //30 seconds recording for each mic turned on (One at a time)[units s]
#define MIC_RATE      40000 //40kHz sampling [units Hz]
#define IMU_TIME      30000 //30 seconds recording acc (interleved between MICs)
#define IMU_PERIOD    4000  //4000uS or 250Hz [units uS]
#define SLEEP_TIME    4 //the number of 8s sleep intervals eg 4x8 =32 seconds of sleeping inbetween records [units x8 S]
#define SLEEP_PERIOD  60000 //sleep after every 1 minute of recording (roughly).[units uS]

//ADVANCED SETTINGS
//=====================-
#define MIC_OFF     0 //To turn the MIC off change this to 1 (to turn the MIC on change to 0)
#define ACC_OFF     0 //To turn the ACC off change this to 2 (to turn the ACC on change to 0)
#define MIC_2OFF    1 //Turn on a second MIC
#define MIC_3OFF    1 //Turn on a third MIC
#define SLEEP_OFF   0 //Get rid of sleeps DO NOT RECOMEND YOU WILL RUN OUT OF MEMORY
