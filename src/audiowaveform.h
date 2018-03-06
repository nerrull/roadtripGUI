#ifndef AUDIOWAVEFORM_H
#define AUDIOWAVEFORM_H

#include "ofMain.h"
#include <mutex>
#include "ofxFft.h"

#define IN_AUDIO_BUFFER_LENGTH 1024
#define INTERNAL_BUFFER_LENGTH IN_AUDIO_BUFFER_LENGTH*4
class AudioWaveform
{
public:
   AudioWaveform();


   void receiveBuffer(ofSoundBuffer& buffer);
   void draw(int,int,int,int);
   void draw2D(int,int,int,int);
   void drawSpectrum(int, int, int, int );
   void shiftSpectrogram();



private:
   void copyBuffer();
   void copyLeftRightBuffer();
   float powFreq(float i);


   std::vector<float> soundBuffer;
   std::vector<float> drawBuffer;

   std::vector<float> leftDrawBuffer;
   std::vector<float> rightDrawBuffer;

   std::mutex bufferMutex;
   ofMutex spectroMutex;
   bool toggle;
   ofSoundBuffer leftBuffer;
   ofSoundBuffer rightBuffer;
   ofxFft* fft;
   vector<float> drawBins, middleBins, audioBins;

   ofImage spectrogram;

   int spectrogramOffset;



};

#endif // AUDIOWAVEFORM_H
