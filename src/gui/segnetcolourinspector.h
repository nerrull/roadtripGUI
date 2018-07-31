#ifndef SEGNETCOLOURINSPECTOR_H
#define SEGNETCOLOURINSPECTOR_H
#include "ofMain.h"

class SegnetColourInspector
{
public:
    SegnetColourInspector();
    vector<ofColor> colors;
    vector<vector<ofColor>> color_pairs;

    string* colorNamePointer;

    void draw();
    void setLayout(int, int, int,int);


private:
    int xOffset, yOffset, width, height;
};

#endif // SEGNETCOLOURINSPECTOR_H
