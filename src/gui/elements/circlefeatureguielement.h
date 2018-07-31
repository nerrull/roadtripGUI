#ifndef CIRCLEFEATUREGUIELEMENT_H
#define CIRCLEFEATUREGUIELEMENT_H

#include "ofMain.h"

class CircleFeatureGuiElement
{
public:
    CircleFeatureGuiElement();
    CircleFeatureGuiElement(int w, int h, int xOffset, int yOffset);

    void drawName();
    void setName(string n);
    void setTextOnTop(bool);
    void setCircleOffset(int);

    virtual void draw();


    virtual void setRange(int range);
    virtual void setPosition(int x, int y);
    virtual void setSize(int w, int h);
    virtual void setWeight(float w){currentWeight = w;}
    virtual void setValue(float v){currentValue = v;}
    virtual void setTarget(float v){targetValue = v;}
    virtual void setActive(bool v){active = v;}

    virtual void update(){}
    virtual void reset(){}



protected:
    ofPolyline exteriorPath;
    ofPolyline interiorPath;

    int xOffset, yOffset;
    int width, height;
    int rotationMin, rotationMax, rotationRange;
    int circleOffset;
    float circleOuterRadius;
    float circleInteriorRadius;

    float textOffset;
    float currentValue;
    float targetValue;
    float currentWeight;
    bool textOnTop;
    bool active;
    string name;
    ofTrueTypeFont font;

};

#endif // CIRCLEFEATUREGUIELEMENT_H
