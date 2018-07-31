#include "audiowaveform.h"

AudioWaveform::AudioWaveform()
{
    spectrogramOffset=0;
    soundBuffer.reserve(INTERNAL_BUFFER_LENGTH);
    drawBuffer.reserve(INTERNAL_BUFFER_LENGTH);
    toggle = false;
    for (int i =0; i<INTERNAL_BUFFER_LENGTH; i++){
        soundBuffer.push_back(0.0f);
        drawBuffer.push_back(0.f);
    }

    fft = ofxFft::create(INTERNAL_BUFFER_LENGTH, OF_FFT_WINDOW_HAMMING);

    spectrogram.allocate(ofGetWidth()/3, ofGetHeight()/3, OF_IMAGE_GRAYSCALE);

    spectrogram.setColor(ofColor::black);

    audioBins.resize(fft->getBinSize());


}

void AudioWaveform::receiveBuffer(ofSoundBuffer& buffer){


    int lastChunkStart = INTERNAL_BUFFER_LENGTH -IN_AUDIO_BUFFER_LENGTH ;

    std::lock_guard<std::mutex> lock(bufferMutex);
    buffer.getChannel(leftBuffer,0 );
    buffer.getChannel(rightBuffer,1 );
    //leftBuffer.addTo(rightBuffer);
    std::copy(soundBuffer.begin()+IN_AUDIO_BUFFER_LENGTH, soundBuffer.end(), soundBuffer.begin());
    std::copy(std::begin(rightBuffer.getBuffer()),std::end(rightBuffer.getBuffer()), std::begin(soundBuffer)+lastChunkStart);



    fft->setSignal(&soundBuffer[0]);

    float* curFft = fft->getAmplitude();
    memcpy(&audioBins[0], curFft, sizeof(float) * fft->getBinSize());

    float maxValue = 0.;
    for(int i = 0; i < fft->getBinSize(); i++) {
        if(abs(audioBins[i]) > maxValue) {
            maxValue = abs(audioBins[i]);
        }
    }
    maxValue = max(maxValue, 0.1f);
    for(int i = 0; i < fft->getBinSize(); i++) {
        audioBins[i] /= maxValue;
    }

    int spectrogramWidth = (int) spectrogram.getWidth();

    int n = (int) spectrogram.getHeight();


    shiftSpectrogram();

    for(int i = 0; i < n; i++) {
        int j =(n - i - 1) * spectrogramWidth +  spectrogramWidth-1;
        int logi = ofMap(powFreq(i), powFreq(0), powFreq(n), 0, n);
        spectrogram.setColor(j, (unsigned char) (255. * audioBins[logi]));
    }

}

void AudioWaveform::shiftSpectrogram(){
    int spectrogramWidth = (int) spectrogram.getWidth();
    int spectrogramHeight= (int) spectrogram.getHeight();

    for (int i = 1 ; i < spectrogramWidth; i++){
        for (int j = 0 ; j < spectrogramHeight; j++){
            ofColor color = spectrogram.getColor( i,j );
            spectrogram.setColor( i-1,j, color );
        }
    }
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

void AudioWaveform::draw2D(){
    ofPushMatrix();
//    ofTranslate(cx, cy);

    //Copy the internal buffer
    copyLeftRightBuffer();
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, width, height);

    ofSetColor(0, 255, 0);

    ofSetLineWidth(1);

    //ofBeginShape();
    for (unsigned int i = 0; i < IN_AUDIO_BUFFER_LENGTH; i++){
        ofSetColor(0, i *255.0/IN_AUDIO_BUFFER_LENGTH,0);

        ofCircle(width/2 + leftDrawBuffer[i]*2000, height/2 -rightDrawBuffer[i]*2000, 0, 1);
        //ofVertex(width/2 - leftDrawBuffer[i]*4000, height/2 -rightDrawBuffer[i]*4000);
    }
   //   ofEndShape();


    ofPopMatrix();
}

void AudioWaveform::setLayout(int x, int y, int w, int h){
    this->xOffset = x;
    this->yOffset = y;
    this->width =w;
    this->height =h;
}

void AudioWaveform::drawWaveform(int w, int h){
    ofNoFill();
    ofSetLineWidth(1);
    //ofDrawRectangle(0, 0, width, height);

    ofSetColor(255, 255, 255);

    ofSetLineWidth(1);

    ofBeginShape();
    int step = INTERNAL_BUFFER_LENGTH/w;
    for (unsigned int i = 0; i < w; i++){
        ofVertex(i, h/2 -drawBuffer[step*i]*h/2);
    }
    ofEndShape();
}

void AudioWaveform::drawSpectrum(int width, int height){

    //std::lock_guard<std::mutex> lock(spectroMutex);
    spectrogram.update();
    spectrogram.draw(0, 0,width, height);

}

void AudioWaveform::draw(){
    //Copy the internal buffer
    copyBuffer();
    ofPushMatrix();
    ofTranslate(xOffset, yOffset);

    drawWaveform(width/2, height);
    ofTranslate(width/2, yOffset);
    drawSpectrum(width/2, height);


    ofPopMatrix();

}


float AudioWaveform::powFreq(float i) {
    return powf(i, 3);
}

