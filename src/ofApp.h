#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "imagemanager.h"
#include "segnetcolourinspector.h"
#include "audiowaveform.h"
#include "databaseloader.h"
#include "pointcloudtreesearch.h"

#define PLAYING_RECEIVE_PORT 44445
#define PYTHON_CONTROL_RECEIVE_PORT 44444
#define CONTROL_RECEIVE_PORT 9002

#define SEND_PORT 33333

#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp{

public:

    ImageManager* imageManager;
    SegnetColourInspector colourInspector;
    AudioWaveform waveform;
    DatabaseLoader databaseLoader;
    PointCloudTreeSearch* pointCloudTree;

    ofxOscReceiver receiver_python_controller;
    ofxOscReceiver receiver_controller;


    ofxOscReceiver receiver_playing;

    ofxOscSender sender;

    ofTrueTypeFont font;
    ofSoundStream soundStream;

    int rectangleWidth;
    int rectangleMaxHeight;
    int rectangleOffset;
    int spaceRemainder;
    int rectangleTop;
    int minHeight;
    int activeIndex;
    bool *activeIndexes;
    int * inactiveCounter;
    int activityTimer;
    vector<float>featureValues;
    vector<float>lastFeatureValues;

    float* desiredFeatureValues;
    bool audioToggle;
    bool desireChanged = false;
    bool speedChanged = false;
    bool RELEASE_MODE;
    uint64_t search_timer;
    int speedSetting;

    vector<string> lastVideos;
    std::vector<string> featureNames;



    void getOscMessage();
    void drawColors();
    float getColorValue(string id);
    void initAudio();
    bool vectorsAreEqual(vector<string>v1, vector<string> v2);
    void publishVideos(vector<string> v1);
    void publishSpeed();
    void drawControls(int,int);

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
