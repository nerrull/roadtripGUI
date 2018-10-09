
#include "ofMain.h"
#include "circlefeatureguielement.h"

class SearchRadiusElement: public CircleFeatureGuiElement
{
public:
    SearchRadiusElement(){
        setRange(360);
    }

    void drawText(){
        CircleFeatureGuiElement::drawText();
        ofPushMatrix();
        ofPushStyle();
        translateToCenter();
        //Draw inner text
        string number = std::to_string(this->num_neighbours);
        float w = font.stringWidth(number);
        float h = font.stringHeight(number);

        font.drawString(number,-w/2,h/2);


        ofPopStyle();
        ofPopMatrix();
    }

    void draw(){
       ofPushMatrix();
       ofPushStyle();

       ofSetLineWidth(2);
       ofTranslate(xOffset, yOffset);
       ofTranslate(0, circleOffset);
       ofTranslate(width/2, height/2);
       ofSetColor(255);
       ofNoFill();
       ofDrawCircle(0,0, currentRadius);


       ofPopStyle();
       ofPopMatrix();



    }

    void setValue(float v){
        this->incrementValue =v * this->circleOuterRadius;
    }

    void setValue(int v){
        this->num_neighbours =v;
    }

    void update(){
        this->currentRadius = CLAMP(this->currentRadius +this->incrementValue,minRadius, this->circleOuterRadius);
    }

    ofColor currentColor;
    float currentRadius= 0.;
    float incrementValue = 1.;
    float minRadius = 8.;
    int num_neighbours = 1;

};

