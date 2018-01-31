#include "audiowaveform.h"

AudioWaveform::AudioWaveform()
{
    soundBuffer.reserve(INTERNAL_BUFFER_LENGTH);
    drawBuffer.reserve(INTERNAL_BUFFER_LENGTH);
    toggle = False;
    for (int i =0; i<INTERNAL_BUFFER_LENGTH; i++){
        soundBuffer.push_back(0.0f);
        drawBuffer.push_back(0.f);
    }
}

void AudioWaveform::receiveBuffer(ofSoundBuffer& buffer){


    int lastChunkStart = INTERNAL_BUFFER_LENGTH -IN_AUDIO_BUFFER_LENGTH ;

    std::lock_guard<std::mutex> lock(bufferMutex);
    buffer.getChannel(leftBuffer,0 );
    buffer.getChannel(rightBuffer,1 );
    //leftBuffer.addTo(rightBuffer);
    std::copy(soundBuffer.begin()+IN_AUDIO_BUFFER_LENGTH, soundBuffer.end(), soundBuffer.begin());
    std::copy(std::begin(rightBuffer.getBuffer()),std::end(rightBuffer.getBuffer()), std::begin(soundBuffer)+lastChunkStart);
}
void AudioWaveform::copyBuffer(){
    std::lock_guard<std::mutex> lock(bufferMutex);
    drawBuffer = soundBuffer;
}


void AudioWaveform::copyLeftRightBuffer(){
    std::lock_guard<std::mutex> lock(bufferMutex);
    leftDrawBuffer = leftBuffer.getBuffer();
    rightDrawBuffer = rightBuffer.getBuffer();
}

void AudioWaveform::draw2D(int cx,int cy,int width, int height){
    ofPushMatrix();
    ofTranslate(cx, cy);
    int end = INTERNAL_BUFFER_LENGTH*4;

    //Copy the internal buffer
    copyLeftRightBuffer();
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, width, height);

    ofSetColor(0, 255, 0);

    ofSetLineWidth(1);

    //ofBeginShape();
    int step = INTERNAL_BUFFER_LENGTH/width;
    for (unsigned int i = 0; i < IN_AUDIO_BUFFER_LENGTH; i++){
        ofSetColor(0, i *255.0/IN_AUDIO_BUFFER_LENGTH,0);

        ofCircle(width/2 + leftDrawBuffer[i]*4000, height/2 -rightDrawBuffer[i]*4000, 0, 1);
        //ofVertex(width/2 - leftDrawBuffer[i]*4000, height/2 -rightDrawBuffer[i]*4000);
    }
   //   ofEndShape();


    ofPopMatrix();
}

void AudioWaveform::draw(int cx,int cy,int width, int height){

    ofPushMatrix();
    ofTranslate(cx, cy);
    int end = INTERNAL_BUFFER_LENGTH*4;

    //Copy the internal buffer
    copyBuffer();

    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, width, height);

    ofSetColor(255, 255, 255);

    ofSetLineWidth(1);

    ofBeginShape();
    int step = INTERNAL_BUFFER_LENGTH/width;
    for (unsigned int i = 0; i < width; i++){
        ofVertex(i, height/2 -drawBuffer[step*i]*height/2*5);
    }
    ofEndShape();


    ofPopMatrix();

}

