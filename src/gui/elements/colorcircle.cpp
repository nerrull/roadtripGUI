#include "colorcircle.h"

ColorCircle::ColorCircle():CircleFeatureGuiElement()
{
    setColorLimits(0.,360.);

}

void ColorCircle::setValue(float v){
    currentColor = ofColor::fromHsb(v*255,200,200);
    currentValue = v;
}

void ColorCircle::setSize(int w, int h){
    CircleFeatureGuiElement::setSize(w,h);
    updateFillLine();
}

void ColorCircle::updateFillLine(){
    ofPushStyle();
    ofSetLineWidth(fillWidth);
    vector<double> widths;
    vector<ofFloatColor> colors;
    ofFloatColor c(1.);

    fillArc.clear();
    fillArc.arc(ofVec3f(0,0,0), fillRadius, fillRadius, rotationMin+fillOffset, rotationMax-fillOffset-1, rotationRange);
    fillArc.rotateDeg(90, ofVec3f(0,0,1));

    for(int i = 0; i< fillArc.getVertices().size(); i++){
        float h = ofMap(i, 0,fillArc.getVertices().size(),minColor,maxColor );
        c = ofFloatColor::fromHsb(h/255.,0.8,0.8);
        colors.push_back(c);
    }

    fillLine.clear();
    fillLine.setFromPolyline(fillArc, colors);
    ofPopStyle();

}

void ColorCircle::setColorLimits(float min, float max){
    minColor = min;
    maxColor = max*255./360.;
}


void ColorCircle::drawFill(){
    //Move to draw position and draw name
    ofPushMatrix();
    ofPushStyle();
    translateToCenter();
    fillLine.draw();
    ofRotateDeg(this->rotationMin);

    ofRotateDeg(fillOffset);
    int current_rot = this->currentValue*(rotationRange -fillOffset*2);
    ofSetLineWidth(2);
    ofSetColor(255);
    ofRotateDeg(fillOffset);
    ofRotateDeg(current_rot);
    ofDrawLine(0,circleInnerRadius,0,circleOuterRadius);

    ofPopStyle();
    ofPopMatrix();
}
