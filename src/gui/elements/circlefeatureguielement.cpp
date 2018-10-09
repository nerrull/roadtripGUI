#include "circlefeatureguielement.h"

CircleFeatureGuiElement::CircleFeatureGuiElement(){
    setRange(250);
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
    this->fillWidth =6;
    this->fillOffset=4;

//    exteriorLine.setGlobalColor(ofColor(255));
//    exteriorPath.setMode(ofPath::POLYLINES);
//    interiorPath.setMode(ofPath::POLYLINES);


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
    this->rotationRange =range;
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
    this->circleOuterRadius = min(width, height)/2;
    this->circleInnerRadius = circleOuterRadius-(2*fillWidth +6);
    this->fillRadius =circleInnerRadius + (circleOuterRadius-circleInnerRadius)*0.5;


    exteriorPath.clear();
    interiorPath.clear();
    exteriorPath.arc(ofVec3f(0,0,0), this->circleOuterRadius, this->circleOuterRadius, this->rotationMin, this->rotationMax, 100);
    interiorPath.arc(ofVec3f(0,0,0), this->circleInnerRadius, this->circleInnerRadius, this->rotationMin, this->rotationMax, 100);

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

void CircleFeatureGuiElement::drawText(){
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(xOffset, yOffset);
    float fontHeight = font.stringHeight(name);
    float fontWidth = font.stringWidth(name);

    int offset = (this->width - fontWidth)/2;

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

    ofPopMatrix();
    ofPopStyle();
}

void CircleFeatureGuiElement::translateToCenter(){
    ofTranslate(xOffset, yOffset);
    ofTranslate(0, circleOffset);
    ofTranslate(width/2, height/2);
}

void CircleFeatureGuiElement::drawShell(){
    ofPushMatrix();
    ofPushStyle();


    translateToCenter();

    //Draw outer circle
    ofSetColor(255);
    ofSetLineWidth(2);

    exteriorPath.draw();
//    exteriorPath2.draw();
//    exteriorLine.draw();

    //Draw control triangle
    if (active){
        ofPushMatrix();
        ofFill();
        int target_rot = this->targetValue*(rotationRange-2*fillOffset);
        ofRotateDeg(rotationMin +target_rot+fillOffset);
        ofTranslate(0, circleOuterRadius+4);
        ofVec2f t1(0,0);
        ofVec2f t2(5,10);
        ofVec2f t3(-5,10);
        ofDrawTriangle(t1, t2,t3);
        ofPopMatrix();
    }

    ofPushMatrix();
    ofRotateDeg(this->rotationMin);
    ofDrawLine(0,circleInnerRadius, 0, 0,circleOuterRadius, 0);
    ofPopMatrix();

    //Draw max boundary line
    ofPushMatrix();
    ofRotateDeg(this->rotationMax);
    ofDrawLine(0,circleInnerRadius, 0,0,circleOuterRadius,0);
    ofPopMatrix();

    //Draw interior circle
    interiorPath.draw();
//    interiorPath2.draw();

    //Draw weight circle
    ofFill();
    float radius = (circleInnerRadius-10)*currentWeight;
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

    ofPopStyle();
    ofPopMatrix();
}

void CircleFeatureGuiElement::drawFill(){
    //Fill up to current value

    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255);
    translateToCenter();

    fillLine.draw();

    ofPopStyle();
    ofPopMatrix();
}


void CircleFeatureGuiElement::updateFillLine(){
    ofPushStyle();
    int current_rot = CLAMP(this->currentValue*(rotationRange - fillOffset*2),1, rotationRange - fillOffset*2);
    fillArc.clear();
    fillArc.arc(ofVec3f(0,0,0), fillRadius, fillRadius, rotationMin+3, rotationMin+3 +current_rot, current_rot+1);
    fillArc.rotateDeg(90, ofVec3f(0,0,1));

    ofSetLineWidth(fillWidth);
    fillLine.clear();
    fillLine.setFromPolyline(fillArc);
    ofPopStyle();
}
void CircleFeatureGuiElement::setValue(float v){
    this->currentValue = v;
    updateFillLine();
}

void CircleFeatureGuiElement::draw(){
    drawShell();
    drawFill();
}

void CircleFeatureGuiElement::setWeightColor(ofColor c){
    weightCircleColor = c;
}

void CircleFeatureGuiElement::setWeightColors(ofColor c1, ofColor c2){
    weightCircleColor = c1;
    secondWeightCircleColor = c2;
    dualColor = true;
}
