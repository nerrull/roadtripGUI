#ifndef DURATIONELEMENT_H
#define DURATIONELEMENT_H

#include "ofMain.h"
#include "circlefeatureguielement.h"

class DurationElement: public CircleFeatureGuiElement
{
public:
    DurationElement():CircleFeatureGuiElement(){
        timeText = "MAX";
        triangle = false;
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
        ofTranslate(offset,fontHeight/2. );
        font.drawString(this->timeText,0,0);
        ofPopMatrix();


        ofPopStyle();
        ofPopMatrix();

    }

    void setValue(float d){
        float duration_seconds = d;
        float duration_frames = duration_seconds*60.;
        this->incrementValue = 1./duration_frames;

        ostringstream os;
        os.precision(2);
//        if (d >4.){
//            os<< "MAX";
//        }
//        else if (d >= 1.){
//            os << "" <<  float(d/1000.) <<"s";
//        }

//        else if (d < 1.){
//        }
        os <<d<<"s";
        timeText = os.str();
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

};

#endif // DURATIONELEMENT_H
