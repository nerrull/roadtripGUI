#include "colorcircle.h"

ColorCircle::ColorCircle()
{
    setRange(360);
}

void ColorCircle::setValue(float v){
    currentColor = ofColor::fromHsb(v*255,200,200);
    currentValue = v;
}

void ColorCircle::draw(){
    ofPushMatrix();
    ofTranslate(xOffset, yOffset);
    drawName();
    ofTranslate(0, circleOffset);
    ofTranslate(width/2, height/2);


    //Draw outer circle
    ofNoFill();

    ofSetColor(currentColor);
    ofDrawCircle(0,0, circleOuterRadius);


    //Draw control triangle
    ofPushMatrix();
    ofFill();
    ofSetColor(255);
    int target_rot = this->targetValue*rotationRange;
    ofRotateDeg(target_rot);
    ofTranslate(0, circleOuterRadius+3);
    ofVec3f t1(0,0);
    ofVec3f t2(5,10);
    ofVec3f t3(-5,10);
    ofDrawTriangle(t1, t2,t3);
    ofPopMatrix();

    //Fill up to current value
    int current_rot = this->currentValue*rotationRange;
    ofPushMatrix();
    for(int i =0; i<360; i++ ){
        ofColor c = ofColor::fromHsb(float(i)/360. *255,200,200);
        ofSetColor(c);
        ofRotateDeg(1);
        ofDrawLine(0,circleInteriorRadius +2,0,circleOuterRadius-2);
    }
    ofPopMatrix();

    //Draw current value line
    ofPushMatrix();
    ofRotateDeg(360*this->currentValue);
    ofSetColor(255.);
    ofDrawLine(0,circleInteriorRadius+2,1,0,circleOuterRadius-2,1);
    ofPopMatrix();

    //Draw interior circle
    ofSetColor(currentColor);
    ofNoFill();
    ofDrawCircle(0,0,0, circleInteriorRadius);

    //Draw weight circle
    ofFill();
    ofDrawCircle(0,0,0, (circleInteriorRadius-10)*currentWeight);

    ofPopMatrix();
}
