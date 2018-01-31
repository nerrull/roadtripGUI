#ifndef AUDIOWAVEFORM_H
#define AUDIOWAVEFORM_H

#include "ofMain.h"
#include <mutex>


#define IN_AUDIO_BUFFER_LENGTH 2048
#define INTERNAL_BUFFER_LENGTH IN_AUDIO_BUFFER_LENGTH*8
class AudioWaveform
{
public:
   AudioWaveform();


   void receiveBuffer(ofSoundBuffer& buffer);
   void draw(int,int,int,int);
   void draw2D(int,int,int,int);



private:
   void copyBuffer();
   void copyLeftRightBuffer();


    std::vector<float> soundBuffer;
    std::vector<float> drawBuffer;

    std::vector<float> leftDrawBuffer;
    std::vector<float> rightDrawBuffer;

    std::mutex bufferMutex;
    bool toggle;
    ofSoundBuffer leftBuffer;
    ofSoundBuffer rightBuffer;
};

#endif // AUDIOWAVEFORM_H
