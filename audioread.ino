#include "FFT.h"
#include "math.h"

//work offline ( doesnt try to connect to cloud)
SYSTEM_MODE(MANUAL);



/*Sampling variables*/
#define MICROPHONE_PIN B3
// #define MICROPHONE_PIN B2
#define DELAY 0 //in millisec
#define SAMPLES 1024 //has to be multiple of 2 
#define OUTPUT_SAMPLES 20

double samplingRate = 5120;
const double samplingInterval = 1/samplingRate; //seconds
unsigned long samplingIntervalMicroS = 1000000*samplingInterval;
const int freq_bin_size = samplingRate / (SAMPLES-1);
int audioStartIdx = 0, audioEndIdx = 0;
unsigned long nextsampletime;

/*
2560Hz sampling rate taken from 512 samples provides freq bins of 5 hz apart,
and a freq range of between 0-1280Hz //(2560/2)//

5120Hz sampling rate taken from 1024 samples provides freq bins of 5 hz apart,
and a freq range of between 0-2560Hz //(5120/2)//
*/

// version without timers
// unsigned long lastRead = millis();
unsigned long lastRead = micros();

/*data arrays*/
double vReal[SAMPLES];
double vImag[SAMPLES];
double output[OUTPUT_SAMPLES]; // for a range of 0-bin_size*20 //ie 0-100 for bin size of 5Hz 



void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(MICROPHONE_PIN, INPUT);
  
  // 1/8000th of a second is 125 microseconds
  lastRead = micros();
  
  delay(DELAY);


}

void loop() {

    //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows, Real part just to make sure. 
    for(uint16_t i = 0; i<SAMPLES; i++) {
        vReal[i] = 0.0;
        vImag[i] = 0.0; 
    }

    //collect input from mic 
    nextsampletime = micros() + samplingIntervalMicroS;
    for(uint16_t i =0;i<SAMPLES;i++) {
        listenAndSend(nextsampletime);
        nextsampletime += samplingIntervalMicroS;
    }
    
    /* Compute desired windowing */
    Windowing(vReal,SAMPLES,FFT_WIN_TYP_HAMMING, FFT_FORWARD);

    /* Compute FFT */
    Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    
    /* Compute magnitudes, stores in vReal */
    ComplexToMagnitude(vReal, vImag, SAMPLES);


    String output = "";
    for (int i=0; i < SAMPLES; i++) {
        output += String(vReal[i]) + " "; 
    }
    Serial.println(output);
 
    //  delay(5000);   
}



/************************************************************
Reading the Audio Mic Input
************************************************************/


void listenAndSend(int nextsampletime) {
    // spin until we've reached the sample time
    while (micros() < nextsampletime) {
        readMic();
    }
    audioEndIdx++;
}

void readMic(void) {
    double value = analogRead(MICROPHONE_PIN);
    if (audioEndIdx >= SAMPLES) {
        audioEndIdx = 0;
    }
    vReal[audioEndIdx] = value;

}

/************************************************************

************************************************************/


