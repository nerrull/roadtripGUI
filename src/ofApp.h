#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "imagemanager.h"
#include "segnetcolourinspector.h"
#include "audiowaveform.h"

#define PORT 44444
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp{

public:

    int rectangleWidth;
    int rectangleMaxHeight;
    int rectangleOffset;
    int spaceRemainder;
    int rectangleTop;
    int minHeight;
    int activeIndex;
    float* featureValues;
    bool audioToggle;
    ImageManager imageManager;
    SegnetColourInspector colourInspector;
    ofxOscReceiver receiver;
    AudioWaveform waveform;
    ofSoundStream soundStream;


    void getOscMessage();


    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void audioIn(ofSoundBuffer & input);



};
