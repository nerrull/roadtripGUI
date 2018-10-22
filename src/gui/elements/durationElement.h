#ifndef DURATIONELEMENT_H
#define DURATIONELEMENT_H

#include "ofMain.h"
#include "circlefeatureguielement.h"
#include "featurecontrol.h"


class DurationElement: public CircleFeatureGuiElement
{
public:
//    static const int NUM_SPEEDS = 16;
//    static const string SPEEDSTRINGS [17]= {"Max", "4", "3", "2", "1.5", "1", "2/3", "1/2","1/3","1/4","1/6", "1/8", "1/10", "1/12", "1/16", "1/24", "1/32"};
    const string SPEEDSTRINGS [17]= {"Max", "4", "3", "2", "1.5", "1", "2/3", "1/2","1/3","1/4","1/6", "1/8", "1/10", "1/12", "1/16", "1/24", "1/32"};
    const int NUM_SPEEDS = 16;


    DurationElement():CircleFeatureGuiElement(){
        timeText = "Max";
        triangle = true;
        setActive(true);
    }

    void drawText(){
        CircleFeatureGuiElement::drawText();
        ofPushMatrix();
        ofPushStyle();
        translateToCenter();
        //Draw inner text
        float fontHeight = font.stringHeight(timeText);
        float fontWidth = font.stringWidth(timeText);
        int offset = -fontWidth/2;

        ofSetColor(255);
        ofPushMatrix();
        ofTranslate(offset,6 );
        font.drawString(this->timeText,0,0);
        ofPopMatrix();

        ofPopStyle();
        ofPopMatrix();
    }

    void setValue(float d){
        this->currentValue = this->currentValue*this->duration_frames /(d*60);
        float duration_seconds = d;
        duration_frames = duration_seconds*60.;
        this->incrementValue = 1./duration_frames;

//        ostringstream os;
//        os.precision(2);
//        if (d >4.){
//            os<< "MAX";
//        }
//        else if (d >= 1.){
//            os << "" <<  float(d/1000.) <<"s";
//        }

//        else if (d < 1.){
//        }
//        os <<d<<"s";
//        timeText = os.str();
    }

    void setSecondary(int i){
        this->targetValue =1.- float(i)/NUM_SPEEDS;
        timeText = SPEEDSTRINGS[i];
    }


    void reset(){
        this->currentValue = 0.;
    }

    void update(){
        this->currentValue = CLAMP(this->currentValue +this->incrementValue,0., 1.);
        updateFillLine();
    }

    ofColor currentColor;
    float currentRadius= 0.;
    float incrementValue = 1.;
    float minRadius = 3.;
    string timeText ="";
    int duration_frames;

};

#endif // DURATIONELEMENT_H
