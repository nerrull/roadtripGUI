#ifndef DURATIONELEMENT_H
#define DURATIONELEMENT_H

#include "ofMain.h"
#include "circlefeatureguielement.h"

class DurationElement: public CircleFeatureGuiElement
{
public:
    DurationElement():CircleFeatureGuiElement(){
        timeText = "MAX";
    }

    void draw(){
        ofPushMatrix();
        ofPushStyle();
        ofSetLineWidth(1);
        ofTranslate(xOffset, yOffset);
        drawName();

        ofTranslate(0, circleOffset);
        ofTranslate(width/2, height/2);
        //Draw outer circle
        ofSetColor(255);
        exteriorPath.draw();

        //Fill up to current value
        int current_rot = this->currentValue*(rotationRange -6);
        ofPushMatrix();
        ofRotateDeg(this->rotationMin);
        ofDrawLine(0,circleInteriorRadius, 0, 0,circleOuterRadius, 0);
        ofRotateDeg(3);

        for(int i =0; i< current_rot; i++ ){
            ofRotateDeg(1);
            ofDrawLine(0,circleInteriorRadius+3,0,circleOuterRadius-3);
        }
        ofPopMatrix();

        //Draw max boundary line
        ofPushMatrix();
        ofRotateDeg(this->rotationMax);
        ofDrawLine(0,circleInteriorRadius, 0,0,circleOuterRadius,0);
        ofPopMatrix();

        //Draw interior circle
        ofSetColor(255);
        interiorPath.draw();

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
        float duration_seconds = d/1000.;
        float duration_frames = duration_seconds*60.;
        this->incrementValue = 1./duration_frames;

        ostringstream os;
        if (d >4000){
            os<< "MAX";
        }
        else if (d >= 1000){
            os << "" <<  float(d/1000.) <<"s";
        }

        else if (d < 1000){
            os <<float(d/1000.)<<"s";
        }
        timeText = os.str();
    }


    void reset(){
        this->currentValue = 0.;
    }

    void update(){
        this->currentValue = CLAMP(this->currentRadius +this->incrementValue,0., 1.);
    }

    ofColor currentColor;
    float currentRadius= 0.;
    float incrementValue = 1.;
    float minRadius = 3.;
    string timeText ="";

};

#endif // DURATIONELEMENT_H
