#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "imagemanager.h"
#include "segnetcolourinspector.h"
#include "audiowaveform.h"
#include "databaseloader.h"
#include "pointcloudtreesearch.h"
#include "communication.h"

#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp{

public:



    void drawColors();
    float getColorValue(string id);
    void initAudio();
    bool vectorsAreEqual(vector<string>v1, vector<string> v2);
    void drawControls(int,int);
    void initNames();
    void updatePlayingVideo(string video);
    void incrementSpeed(int step);
    void incrementFeatureTarget(int index, float step);
    void updateOSC();
    void handleKnobInput(ofxOscMessage m);
    void handleButtonInput(int index);

    void setSpeed(int value);
    void toggleFeatureTarget(int index);



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

    ImageManager* imageManager;
    SegnetColourInspector colourInspector;
    AudioWaveform waveform;
    DatabaseLoader databaseLoader;
    PointCloudTreeSearch* pointCloudTree;
    CommunicationManager coms;

    ofTrueTypeFont font;
    ofSoundStream soundStream;

    int rectangleWidth;
    int rectangleMaxHeight;
    int rectangleOffset;
    int spaceRemainder;
    int rectangleTop;
    int minHeight;
    int num_knobs;

    vector<float> featureWeights;
    vector<float>lastFeatureWeights;

    vector<int> inactiveCounter;
    vector<float>desiredFeatureValues;
    vector<int> video_indexes;

    int activityTimer;
    vector<float>featureValues;
    vector<float>lastFeatureValues;
    vector<string> feature_names_en;
    vector<string> feature_names_fr;

    bool input_activity = false;
    bool audioToggle;
    bool desireChanged = false;
    bool speedChanged = false;
    bool DEV_MODE;

    uint64_t search_timer;
    int speedSetting;
    string current_playing_video;
    vector<string> lastVideos;
    std::vector<string> featureNames;

};
