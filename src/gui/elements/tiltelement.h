#ifndef TILTELEMENT_H
#define TILTELEMENT_H
#include "circlefeatureguielement.h"

class TiltElement: public CircleFeatureGuiElement
{
public:
    TiltElement(): CircleFeatureGuiElement(){

    }

    void updateFillLine(){
        ofPushStyle();
        ofSetLineWidth(fillWidth);
        float offset = (currentValue-0.5);
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
