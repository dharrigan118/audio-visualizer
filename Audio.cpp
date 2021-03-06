/*
 * Audio.cpp - class encapsulates all audio activities in the program
 *
 * David Harrigan
 */

#include "Audio.h"
#include <string>
#include <stdio.h>
#include <math.h>
#include <algorithm>

static const float notefreq[120] = {
      16.35f,   17.32f,   18.35f,   19.45f,    20.60f,    21.83f,    23.12f,    24.50f,    25.96f,    27.50f,    29.14f,    30.87f, 
      32.70f,   34.65f,   36.71f,   38.89f,    41.20f,    43.65f,    46.25f,    49.00f,    51.91f,    55.00f,    58.27f,    61.74f, 
      65.41f,   69.30f,   73.42f,   77.78f,    82.41f,    87.31f,    92.50f,    98.00f,   103.83f,   110.00f,   116.54f,   123.47f, 
     130.81f,  138.59f,  146.83f,  155.56f,   164.81f,   174.61f,   185.00f,   196.00f,   207.65f,   220.00f,   233.08f,   246.94f, 
     261.63f,  277.18f,  293.66f,  311.13f,   329.63f,   349.23f,   369.99f,   392.00f,   415.30f,   440.00f,   466.16f,   493.88f, 
     523.25f,  554.37f,  587.33f,  622.25f,   659.26f,   698.46f,   739.99f,   783.99f,   830.61f,   880.00f,   932.33f,   987.77f, 
    1046.50f, 1108.73f, 1174.66f, 1244.51f,  1318.51f,  1396.91f,  1479.98f,  1567.98f,  1661.22f,  1760.00f,  1864.66f,  1975.53f, 
    2093.00f, 2217.46f, 2349.32f, 2489.02f,  2637.02f,  2793.83f,  2959.96f,  3135.96f,  3322.44f,  3520.00f,  3729.31f,  3951.07f, 
    4186.01f, 4434.92f, 4698.64f, 4978.03f,  5274.04f,  5587.65f,  5919.91f,  6271.92f,  6644.87f,  7040.00f,  7458.62f,  7902.13f, 
    8372.01f, 8869.84f, 9397.27f, 9956.06f, 10548.08f, 11175.30f, 11839.82f, 12543.85f, 13289.75f, 14080.00f, 14917.24f, 15804.26f
};

// maximum number of FMODSoundPlayer is 8192
float fftValues_[8192];			
float fftInterpValues_[8192];	
float fftSpectrum_[8192];

// Global static variables
static FMOD::ChannelGroup *channelGroup;
static FMOD::System *sys;

// Global functions
//----------------------------------------------------------- 

//
// Returns the next power of 2
//
int ofNextPow2(int x) {
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

//
// Return sound spectrum with specific range. Useful if you want to
// combine different frequencies with the same musical note 
//
void getSoundSpectrum(int range, float* output) {
    int num = 8192;
    float* left = new float[num];
    float* right = new float[num];
    sys->getSpectrum(left, num, 0, FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS);
    sys->getSpectrum(right, num, 1, FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS);

    // average the left and right channels
    for (int i=0; i<num; i++) {
        fftInterpValues_[i] = (left[i] + right[i]) / 2.0f;
//        fftInterpValues_[i] = 10.0f * (float)log10(1 + fftInterpValues_[i]) * 2.0f;
    }

    for (int i=0; i<num; i++) {
        if (fftInterpValues_[i] > 0.01 ) {
            //output[curNote % range] += (left[i] > right[i]) ? left[i] : right[i]; 
            output[i % range] += fftInterpValues_[i];
            if (output[i % range] - fftInterpValues_[i] != 0)
                output[i % range] /= 2.0;
       }
    }

    // normalize volume
    auto maxIterator = std::max_element(&output[0], &output[range]);
    float maxVol = *maxIterator;
    maxVol *= 1.6;

    if (maxVol != 0)
        std::transform(&output[0], &output[range], &output[0], [maxVol] (float dB) -> float {return dB / maxVol;});

    delete left;
    delete right;
}


// Constructor
//-------------------------------------------
Audio::Audio() {
    pan = 0;
    curFile = 0;
    volume = 1.0;
    sampleRate = 44100;
}

// Destructor
//-------------------------------------------
Audio::~Audio() {
    delete channel;
    delete sound;
}


//
// Initialize FMOD system
//
void Audio::initialize() {
    result = System_Create(&sys);
    sys->init(32, FMOD_INIT_NORMAL, NULL);
    sys->getMasterChannelGroup(&channelGroup);
}

//
// Close FMOD system
//
void Audio::close() {
    result = sys->close();
}

//
// Save filename to the file vector
//
void Audio::saveFile(std::string filename) {
    files.push_back(filename);    
}

//
// load audio file
//
bool Audio::loadFile() {
    printf("%s\n", files[curFile].c_str());
    result = sys->createSound( files[curFile].c_str(), FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);

    if (result != FMOD_OK) {
        printf("Error loading %s\n", files[curFile].c_str());
        printf("(%d) %s\n", result, FMOD_ErrorString(result));
        return false;
    }
    sound->getLength(&length, FMOD_TIMEUNIT_PCM);
    return true;
}

//
// Play the loaded file
//
void Audio::play() {
   sys->playSound( FMOD_CHANNEL_FREE, sound, 0, &channel);
   channel->setVolume(volume);
   channel->setPan(pan);
   channel->setFrequency(sampleRate);
   channel->setMode(FMOD_LOOP_OFF);
   sys->update();
}

//
// Stop the loaded file
//
void Audio::stop() {
    channel->stop();
    sound->release();
}

//
// Play the next song in the filename vector
// 
void Audio::next() {
    stop();
    curFile = (curFile == files.size()-1) ? 0 : curFile+1;
    loadFile();
    play();
}
