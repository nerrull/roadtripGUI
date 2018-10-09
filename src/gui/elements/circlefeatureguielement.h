#ifndef CIRCLEFEATUREGUIELEMENT_H
#define CIRCLEFEATUREGUIELEMENT_H

#include "ofMain.h"
#include "ofxFatLine.h"
class CircleFeatureGuiElement
{
public:
    CircleFeatureGuiElement();
    CircleFeatureGuiElement(int w, int h, int xOffset, int yOffset);

    void setName(string n);
    void setTextOnTop(bool);
    void setCircleOffset(int);
    void setWeightColor(ofColor c);
    void setWeightColors(ofColor c1, ofColor c2);

    virtual void draw();
    virtual void drawFill();
    virtual void drawShell();
    virtual void drawText();


    virtual void setRange(int range);
    virtual void setPosition(int x, int y);
    virtual void setSize(int w, int h);
    virtual void setWeight(float w){currentWeight = w;}
    virtual void setValue(float v);
    virtual void setValue(int v){currentValue = v;}

    virtual void setTarget(float v){targetValue = v;}
    virtual void setActive(bool v){active = v;}

    virtual void setMax(float v){return;}

    virtual void update(){}
    virtual void reset(){}


protected:
    ofxFatLine fillLine;
    ofPolyline fillArc;
    ofPolyline exteriorPath;
    ofPolyline interiorPath;


    int xOffset, yOffset;
    int width, height;
    int rotationMin, rotationMax, rotationRange;
    int circleOffset;
    int fillWidth;
    int fillOffset;
    float circleOuterRadius;
    float circleInnerRadius;
    float fillRadius;


    float textOffset;
    float currentValue;
    float targetValue;
    float currentWeight;
    bool textOnTop;
    bool active;
    string name;
    ofTrueTypeFont font;
    ofColor weightCircleColor;
    ofColor secondWeightCircleColor;
    bool dualColor;

    void translateToCenter();
    virtual void updateFillLine();


};

#endif // CIRCLEFEATUREGUIELEMENT_H
