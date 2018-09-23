#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "gui/imagemanager.h"
#include "gui/segnetcolourinspector.h"
#include "gui/audiowaveform.h"
#include "gui/pointcloudrenderer.h"
#include "util/featureKNN.h"
#include "util/databaseloader.h"
#include "util/communication.h"
#include "gui/elements/uielements.h"
#include "util/behaviour.h"

#define NUM_MSG_STRINGS 20



class ofApp : public ofBaseApp{

public:
    //Init functions
    void initAudio();
    void initNames();
    void setLayout();

    //Draw/update functions
    void drawColors();
    void drawControls(int,int);

    //Coms
    void updateOSC();
    void updatePlayingVideo(string video);
    void handlePythonMessages();


    void setSpeed(int value);
    void toggleFeatureTarget(int index);
    void incrementSpeed(int step);
    void incrementFeatureTarget(int index, float step);
    void incrementSearchRadius(int step);


    float getColorValue(string id);
    void handleKnobInput(ofxOscMessage m);
    void handleButtonInput(int index);
    void updateActiveFeature(int index);

    bool vectorsAreEqual(vector<string>v1, vector<string> v2);
    void toggleLanguage();
    void playRandomVideo();



    //Default functions
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


    //GUI elements
    ImageManager* imageManager;
    SegnetColourInspector colourInspector;
    AudioWaveform waveform;
    DatabaseLoader databaseLoader;
    FeatureKNN fKNN;
    PointCloudRenderer pointCloudRender;
    Behaviour behaviour;

    vector<unique_ptr<CircleFeatureGuiElement>> featureGuiElements;

    CommunicationManager coms;
    ofTrueTypeFont font;
    ofSoundStream soundStream;

    int rectangleWidth;
    int rectangleMaxHeight;
    int rectangleOffset;
    int spaceRemainder;
    int rectangleTop;
    int minHeight;
    int numKnobs;

    int incomingPlayerMessageCounter;
    int incomingControllerMessageCounter;

    int activityTimer;
    int playingFileDuration =1000;
    int featureTimeout;
    int lastActiveFeatureIndex;
    float featureDecayRate;
    vector<bool> featureActive;
    vector<float> featureWeights;
    vector<float> lastFeatureWeights;

    vector<int> inactiveCounter;
    vector<float> targetFeatureValues;
    vector<int> videoIndexes;

    vector<float>featureValues;
    vector<float>lastFeatureValues;
    vector<string> featureNamesEn;
    vector<string> featureNamesFr;
    vector<string> featureNames;
    string durationName, neighbourName;

    bool input_activity = false;
    bool audioToggle;
    bool desireChanged = false;
    bool speedChanged = false;
    bool DEV_MODE;
    bool languageIsEnglish;

    uint64_t search_timer;
    int speedSetting;
    string currentPlayingVideo;
    vector<string> lastVideos;


    int SPEEDS [19]= {-1, 4000, 3000, 2000, 1500, 1000,900,800,700,600, 500,400, 300, 250, 200, 150, 100, 66, 33};


};
