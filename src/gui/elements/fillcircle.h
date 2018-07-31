#ifndef FILLCIRCLE_H
#define FILLCIRCLE_H

#include "ofMain.h"
#include "circlefeatureguielement.h"

class FillCircle: public CircleFeatureGuiElement
{
public:
    FillCircle(){
        setRange(360);
    }

    void draw(){
       ofPushMatrix();
       ofTranslate(xOffset, yOffset);
       drawName();
       ofTranslate(0, circleOffset);
       ofTranslate(width/2, height/2);
       ofSetColor(255);
       ofNoFill();
       ofDrawCircle(0,0, circleOuterRadius);
       ofFill();
       ofDrawCircle(0,0, currentRadius);
       ofPopMatrix();

    }

    void setValue(float v){
        this->incrementValue =v * this->circleOuterRadius;
    }

    void reset(){
        this->currentRadius = minRadius;
    }

    void update(){
        this->currentRadius = CLAMP(this->currentRadius +this->incrementValue,minRadius, this->circleOuterRadius);
    }

    ofColor currentColor;
    float currentRadius= 0.;
    float incrementValue = 1.;
    float minRadius = 3.;

};

#endif // FILLCIRCLE_H
