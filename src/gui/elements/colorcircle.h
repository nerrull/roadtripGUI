#ifndef COLORCIRCLE_H
#define COLORCIRCLE_H

#include "ofMain.h"
#include "circlefeatureguielement.h"

class ColorCircle: public CircleFeatureGuiElement
{
public:
    ColorCircle();

    void drawFill();
    void setValue(float);
    void updateFillLine();
    void setSize(int w, int h);
    void setColorLimits(float, float);

    ofColor currentColor;
    float minColor, maxColor;

};

#endif // COLORCIRCLE_H
