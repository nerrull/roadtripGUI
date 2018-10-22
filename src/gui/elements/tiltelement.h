#ifndef TILTELEMENT_H
#define TILTELEMENT_H
#include "circlefeatureguielement.h"

class TiltElement: public CircleFeatureGuiElement
{
public:
    TiltElement(): CircleFeatureGuiElement(){

    }

    void drawTriangle(){
        ofPushMatrix();
        ofPushStyle();
        translateToCenter();
        //Draw control triangle
        if (active && triangle){
            ofPushMatrix();
            ofFill();
            int target_rot = this->targetValue*(rotationRange-2*fillOffset);
            ofRotateDeg(rotationMin);

            ofRotateDeg(rotationRange/2 +target_rot+fillOffset);
            ofTranslate(0, circleOuterRadius+4);
            ofVec2f t1(0,0);
            ofVec2f t2(5,10);
            ofVec2f t3(-5,10);
            ofDrawTriangle(t1, t2,t3);
            ofPopMatrix();
        }
        ofPopStyle();
        ofPopMatrix();
    }

    void updateFillLine(){
        ofPushStyle();
        ofSetLineWidth(fillWidth);
        float offset = currentValue;
        float diff =  rotationMin+ rotationRange/2+offset*(rotationRange- 2*fillOffset);
        fillArc.clear();
        if( offset >0){
            fillArc.arc(ofVec3f(0,0,0), fillRadius, fillRadius, rotationMin +rotationRange/2, diff, int(diff));
        }
        else if( offset <0){
            fillArc.arc(ofVec3f(0,0,0), fillRadius, fillRadius, diff, rotationMin +rotationRange/2,  int(abs(diff)));
        }
        else{
            fillArc.arc(ofVec3f(0,0,0), fillRadius, fillRadius, rotationMin +rotationRange/2 -1, rotationMin +rotationRange/2 +1,  int(abs(diff)));

        }
        fillArc.rotateDeg(90, ofVec3f(0,0,1));

        fillLine.clear();
        fillLine.setFromPolyline(fillArc, ofFloatColor(fillColor/255.));

        ofPopStyle();
    }

};
#endif // TILTELEMENT_H
