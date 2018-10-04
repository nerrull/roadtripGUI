#ifndef SEGNETCOLOURINSPECTOR_H
#define SEGNETCOLOURINSPECTOR_H
#include "ofMain.h"
const string segnetSingleFeatures[6]= {"building", "pavement", "road","sky", "tree", "vehicle"};
const string segnetPairedFeatures[3]= {"roadSign","fencePole", "pedestrianBike"};

class SegnetColourInspector
{
public:
    SegnetColourInspector();
    map<string, ofColor> colorMap;
    map<string, pair<ofColor,ofColor>> colorPairMap;

    void draw();
    void setLayout(int, int, int,int);


private:
    int xOffset, yOffset, width, height;
};

#endif // SEGNETCOLOURINSPECTOR_H
