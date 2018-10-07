#ifndef TILTELEMENT_H
#define TILTELEMENT_H
#include "circlefeatureguielement.h"

class TiltElement: public CircleFeatureGuiElement
{
public:
    TiltElement(): CircleFeatureGuiElement(){

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

        //Draw control triangle
        if (active){
            ofPushMatrix();
            ofFill();
            ofSetColor(255);
            int target_rot = this->targetValue*rotationRange;
            ofRotateDeg(rotationMin +target_rot);
            ofTranslate(0, circleOuterRadius+3);
            ofVec2f t1(0,0);
            ofVec2f t2(5,8);
            ofVec2f t3(-5,8);
            ofDrawTriangle(t1, t2,t3);
            ofPopMatrix();
        }


        //Fill up to current value
        ofPushMatrix();
        ofRotateDeg(this->rotationMin);
        ofDrawLine(0,circleInteriorRadius, 0, 0,circleOuterRadius, 0);

        ofRotateDeg(rotationRange/2);

        if (this ->currentValue >= 0.5){
            int current_rot = this->currentValue*(rotationRange/2 -6);

            for(int i =0; i< current_rot; i++ ){
                ofRotateDeg(1);
                ofDrawLine(0,circleInteriorRadius+3,0,circleOuterRadius-3);
            }
        }


        else if (this ->currentValue < 0.5){
            int current_rot = this->currentValue*(rotationRange/2 -6);

            for(int i =0; i< current_rot; i++ ){
                ofRotateDeg(-1);
                ofDrawLine(0,circleInteriorRadius+3,0,circleOuterRadius-3);
            }
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

        //Draw weight circle
        ofFill();
        float radius = (circleInteriorRadius-10)*currentWeight;
        ofSetColor(weightCircleColor);
        ofDrawCircle(0,0,0,radius);

        ofPopStyle();
        ofPopMatrix();

    }

};
#endif // TILTELEMENT_H
