#ifndef COLORCIRCLE_H
#define COLORCIRCLE_H

#include "ofMain.h"
#include "circlefeatureguielement.h"

class ColorCircle: public CircleFeatureGuiElement
{
public:
    ColorCircle();

    void draw();
    void setValue(float);

    ofColor currentColor;

};

#endif // COLORCIRCLE_H
