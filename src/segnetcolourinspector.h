#ifndef SEGNETCOLOURINSPECTOR_H
#define SEGNETCOLOURINSPECTOR_H
#include "ofMain.h"

class SegnetColourInspector
{
public:
    SegnetColourInspector();
    ofColor* colorPointer;
    string* colorNamePointer;
    void draw(int, int, int,int);
};

#endif // SEGNETCOLOURINSPECTOR_H
