#include "colorcircle.h"

ColorCircle::ColorCircle()
{
    setRange(270);
}

void ColorCircle::setValue(float v){
    currentColor = ofColor::fromHsb(v*255,200,200);
    currentValue = v;
}

void ColorCircle::draw(){
    //Move to draw position and draw name
    ofPushMatrix();
    ofTranslate(xOffset, yOffset);
    drawName();
    ofTranslate(0, circleOffset);
    ofTranslate(width/2, height/2);

    //Draw outer circle
    ofNoFill();
    ofSetColor(255);
    exteriorPath.draw();

    //Draw control triangle
    if (active){
        ofPushMatrix();
        ofFill();
        ofSetColor(255);
        int target_rot = this->targetValue*rotationRange;
        ofRotateDeg(rotationMin + target_rot);
        ofTranslate(0, circleOuterRadius+3);
        ofVec3f t1(0,0);
        ofVec3f t2(5,8);
        ofVec3f t3(-5,8);
        ofDrawTriangle(t1, t2,t3);
        ofPopMatrix();
      }

    //Fill up to current value
    int current_rot = this->currentValue*rotationRange-6;

    ofPushMatrix();
    ofRotateDeg(this->rotationMin);
    ofDrawLine(0,circleInteriorRadius, 0, 0,circleOuterRadius, 0);
    ofRotateDeg(3);

    for(int i =0; i<rotationRange; i++ ){
        ofColor c = ofColor::fromHsb(float(i)/rotationRange*255,200,200);
        ofSetColor(c);
        ofRotateDeg(1);
        ofDrawLine(0,circleInteriorRadius +3,0,circleOuterRadius-3);
    }
    ofPopMatrix();

    //Draw current value line
    ofPushMatrix();
    ofRotateDeg(this->rotationMin);
    ofRotateDeg(3);
    ofRotateDeg(this->targetValue*rotationRange);
    ofSetColor(255.);
    ofDrawLine(0,circleInteriorRadius+2,1,0,circleOuterRadius-2,1);
    ofPopMatrix();

    //Draw max boundary line
    ofPushMatrix();
    ofRotateDeg(this->rotationMax);
    ofDrawLine(0,circleInteriorRadius, 0,0,circleOuterRadius,0);
    ofPopMatrix();

    //Draw interior circle
    ofNoFill();
    ofSetColor(255);
    interiorPath.draw();

    //Draw weight circle
    ofFill();
    ofDrawCircle(0,0,0, (circleInteriorRadius-10 )*currentWeight);


    ofPopMatrix();
}
