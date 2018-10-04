#include "circlefeatureguielement.h"

CircleFeatureGuiElement::CircleFeatureGuiElement(){
    setRange(270);
    setSize(100,100);
    setPosition(0,0);

    this->currentValue = 0.;
    this->targetValue = 0.;
    this->currentWeight = 0.;
    this->font.load("futura.ttf", 12);
    this->name = "No name";
    this->circleOffset = 0;
    this->weightCircleColor = ofColor(255);
    this->dualColor = false;


//    exteriorPath.setFilled(false);
//    interiorPath.setFilled(false);
//    exteriorPath.setStrokeWidth(1);
//    interiorPath.setStrokeWidth(1);
//    exteriorPath.setStrokeColor(255);
//    interiorPath.setStrokeColor(255);
}


CircleFeatureGuiElement::CircleFeatureGuiElement(int w, int h, int x, int y)
{
    setPosition(x,y);
    setSize(w,h);
    setRange(270);

    this->currentValue = 0.;
    this->targetValue = 0.;
    this->currentWeight = 0.;
}

void CircleFeatureGuiElement::setRange(int range){
    range = CLAMP(range, 0, 360);
    this->rotationRange =range-5;
    int diff = 360 - range;
    this->rotationMin = diff/2;
    this->rotationMax = 360-diff/2;

}

void CircleFeatureGuiElement::setPosition(int x, int y){
    this->xOffset = x;
    this->yOffset = y;
}

void CircleFeatureGuiElement::setSize(int w, int h){
    this->width = w;
    this->height = h;
    this->circleOuterRadius = min(width, height)/2-5;
    this->circleInteriorRadius = circleOuterRadius*0.6;
    exteriorPath.clear();
    interiorPath.clear();
    exteriorPath.arc(ofVec3f(0,0,0), this->circleOuterRadius, this->circleOuterRadius, this->rotationMin, this->rotationMax, 100);
    interiorPath.arc(ofVec3f(0,0,0), this->circleInteriorRadius, this->circleInteriorRadius, this->rotationMin, this->rotationMax, 100);
    exteriorPath.rotateDeg(90, ofVec3f(0,0,1));
    interiorPath.rotateDeg(90, ofVec3f(0,0,1));
}

void CircleFeatureGuiElement::setTextOnTop(bool top){
    this->textOnTop = top;
}
void CircleFeatureGuiElement::setCircleOffset(int o){
    circleOffset = o;
}

void CircleFeatureGuiElement::setName(string n){
    this->name = n;

    float fontWidth = font.stringWidth(name);
    if (fontWidth > this->width){
        ofLogError()<<this->name << " is too wide"<<endl;
    }
}

void CircleFeatureGuiElement::drawName(){
    float fontHeight = font.stringHeight(name);
    float fontWidth = font.stringWidth(name);
    int offset = 0;
    if (fontWidth < this->width){
        offset = (this->width - fontWidth)/2;
    }
    ofSetColor(255);
    ofPushMatrix();
    int y= 0;
    if (!this->textOnTop)
    {
        y= height+25;
    }
    ofTranslate(offset,y );
    font.drawString(this->name,0,0);
    ofPopMatrix();

}

void CircleFeatureGuiElement::draw(){
    ofPushStyle();
    ofPushMatrix();
    ofSetLineWidth(2);

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

    //Draw weight circle
    ofFill();
    float radius = (circleInteriorRadius-10)*currentWeight;
    if (dualColor){
        ofPath left;
        left.setColor(weightCircleColor);
        left.arc(0,0,0, radius, radius, 90, 270);
        left.draw();

        ofPath right;
        right.setColor(secondWeightCircleColor);
        right.arc(0,0,0, radius, radius, 270, 90);
        right.draw();
    }
    else{
        ofSetColor(weightCircleColor);
        ofDrawCircle(0,0,0,radius);
    }


    ofPopMatrix();
    ofPopStyle();
}

void CircleFeatureGuiElement::setWeightColor(ofColor c){
    weightCircleColor = c;
}

void CircleFeatureGuiElement::setWeightColors(ofColor c1, ofColor c2){
    weightCircleColor = c1;
    secondWeightCircleColor = c2;
    dualColor = true;
}
