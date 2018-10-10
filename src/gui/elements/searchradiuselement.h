
#include "ofMain.h"
#include "circlefeatureguielement.h"

class SearchRadiusElement: public CircleFeatureGuiElement
{
public:
    SearchRadiusElement():CircleFeatureGuiElement(){
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



    void setValue(int v){
        this->num_neighbours =v;
        this->currentValue = float(v)/max_num_neighbours;
        updateFillLine();
    }


    ofColor currentColor;
    float currentRadius= 0.;
    float minRadius = 8.;
    int num_neighbours = 1;
    int max_num_neighbours = 32;

};

