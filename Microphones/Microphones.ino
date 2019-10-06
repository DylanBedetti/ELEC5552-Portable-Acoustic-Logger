#include <I2S.h>
#include <SD.h>
#include <SPI.h>

#define TIME_LIMIT 30 // (seconds)
#define DC_BLOCKING_FACTOR                  0.995f

File rec;
uint8_t buffer[512],buffer1[512];
volatile int available = 0;
volatile int read = 0;
unsigned long recordStart = 0;
bool recordInProgress = false;

unsigned long fileSize = 0L;
unsigned long waveChunk = 16;       //16 for PCM
unsigned int waveType = 1;
unsigned int numChannels = 1;         //1:Mono 2:Stereo
unsigned long sampleRate = 15625;   
unsigned long bytesPerSec = 46875;    //(Sample Rate * BitsPerSample * Channels) / 8
unsigned int blockAlign = 3;          //(BitsPerSample * Channels) / 8
unsigned int bitsPerSample = 24;      
unsigned long dataSize = 0L;
unsigned long recByteSaved = 0L;
byte byte1, byte2, byte3, byte4;
byte bufferToWrite[2048];
int lenOfBufferToWrite = 0;
int temp = 0;


void setup()
{

    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    // see if the card is present and can ben initialized
    if (!SD.begin())
    {
        //Serial.println("Card SD failed or not present");
        return; // do nothing
    }

    I2S.onReceive(onI2SReceive);

    int sampleRateForI2S = 31250;    
    if (!I2S.begin(I2S_PHILIPS_MODE, sampleRateForI2S, 32))
    {
        //Serial.println("Failed to initialize I2S!");
        while (1)
            ; // do nothing
    }

    StartRec();
    I2S.read(); // Needed to start the interrupt handler
}

void onI2SReceive()
{
    // This function will run at a frequency of (sampleRate / 64)
    // At 31.25khz, this is every 1962.5 microseconds so make sure any processing here takes less
    // time than that

    I2S.read(buffer, 512); // Will actually read 256 bytes
    available = 1;
}

void loop()
{
    if (millis() - recordStart <= TIME_LIMIT * 1000)
    {
        if (available)
        {
            for (int i = 0; i < 32; i++)
            {
                // buffer[0] to buffer[3] are zeros (it is the other channel), so we skip those
                // buffer[4] is always zero
                // buffer[5], [6] and [7] are the interesting data, on 24 bits (6 of which are always zeros)
                // Etc
                // We only want the 256 first values, hence we stope at 8 * 31 + 7 = 255
                // More info on robsgreen's great post: https://forums.adafruit.com/viewtopic.php?f=19&t=115089&start=15#wrap
                filter(&buffer[(8 * i) + 5],&buffer1[(8 * i) + 5]);
                filter(&buffer[(8 * i) + 6],&buffer1[(8 * i) + 6]);
                filter(&buffer[(8 * i) + 7],&buffer1[(8 * i) + 7]);                  
                memcpy(&bufferToWrite[lenOfBufferToWrite + 3 * i], &buffer1[(8 * i) + 5], 3);      
            }
            lenOfBufferToWrite += 96;
            if (lenOfBufferToWrite == 2016)
            {
                // Write on SD card by batches of 2048 bits for better performance
                rec.write(bufferToWrite, 2016);
                lenOfBufferToWrite = 0;
                recByteSaved += 2016;
            }
            available = 0;
        }
    }
    else if (recordInProgress)
    {
        //Serial.println(recByteSaved); // Generally, we record 14000 to 15000 bytes per second
        StopRec();
    }
}

void StartRec()
{ // begin recording process
    writeWavHeader();
    //Serial.println(F("Starting recording"));
    recordStart = millis();
    recordInProgress = true;
}

void StopRec()
{ // stop recording process, update WAV header, close file
    //Serial.println(F("Ending recording"));
    writeOutHeader();
    recordInProgress = false;
}

void writeOutHeader()
{ // update WAV header with final filesize/datasize

    // rec.seekSet(4);
    rec.seek(4);
    byte1 = recByteSaved & 0xff;
    byte2 = (recByteSaved >> 8) & 0xff;
    byte3 = (recByteSaved >> 16) & 0xff;
    byte4 = (recByteSaved >> 24) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    rec.write(byte3);
    rec.write(byte4);
    // rec.seekSet(40);
    rec.seek(40);
    rec.write(byte1);
    rec.write(byte2);
    rec.write(byte3);
    rec.write(byte4);
    rec.close();
}

void writeWavHeader()
{ // write out original WAV header to file

    recByteSaved = 0;
    // rec.open("rec00000.wav", O_CREAT | O_TRUNC | O_RDWR);
    String filename = "rec00000.wav";
    if (SD.exists(filename))
    {
        SD.remove(filename);
    }
    rec = SD.open(filename, FILE_WRITE);
    rec.write("RIFF");
    byte1 = fileSize & 0xff;
    byte2 = (fileSize >> 8) & 0xff;
    byte3 = (fileSize >> 16) & 0xff;
    byte4 = (fileSize >> 24) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    rec.write(byte3);
    rec.write(byte4);
    rec.write("WAVE");
    rec.write("fmt ");
    byte1 = waveChunk & 0xff;
    byte2 = (waveChunk >> 8) & 0xff;
    byte3 = (waveChunk >> 16) & 0xff;
    byte4 = (waveChunk >> 24) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    rec.write(byte3);
    rec.write(byte4);
    byte1 = waveType & 0xff;
    byte2 = (waveType >> 8) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    byte1 = numChannels & 0xff;
    byte2 = (numChannels >> 8) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    byte1 = sampleRate & 0xff;
    byte2 = (sampleRate >> 8) & 0xff;
    byte3 = (sampleRate >> 16) & 0xff;
    byte4 = (sampleRate >> 24) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    rec.write(byte3);
    rec.write(byte4);
    byte1 = bytesPerSec & 0xff;
    byte2 = (bytesPerSec >> 8) & 0xff;
    byte3 = (bytesPerSec >> 16) & 0xff;
    byte4 = (bytesPerSec >> 24) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    rec.write(byte3);
    rec.write(byte4);
    byte1 = blockAlign & 0xff;
    byte2 = (blockAlign >> 8) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    byte1 = bitsPerSample & 0xff;
    byte2 = (bitsPerSample >> 8) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    rec.write("data");
    byte1 = dataSize & 0xff;
    byte2 = (dataSize >> 8) & 0xff;
    byte3 = (dataSize >> 16) & 0xff;
    byte4 = (dataSize >> 24) & 0xff;
    rec.write(byte1);
    rec.write(byte2);
    rec.write(byte3);
    rec.write(byte4);
}


static void filter(uint8_t *source, uint8_t *dest) 
{
    uint8_t previousFilterOutput =0;
    uint8_t filteredOutput =0;
    uint8_t scaledPreviousFilterOutput=0;
    int index = 0;
    uint8_t sample = 0;
    uint8_t previousSample =0;

    sample = *source;
       
    scaledPreviousFilterOutput = (uint8_t)(DC_BLOCKING_FACTOR * previousFilterOutput);

    filteredOutput = sample - previousSample + scaledPreviousFilterOutput;

    *dest = (uint8_t)filteredOutput;

    previousFilterOutput = filteredOutput;

    previousSample = (uint8_t)sample;
    
}
