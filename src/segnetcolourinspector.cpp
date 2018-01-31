#include "segnetcolourinspector.h"

SegnetColourInspector::SegnetColourInspector()
{
    colorPointer =new ofColor[12];
    colorNamePointer = new string[12];

    ofColor skyColor = ofColor(128,128,128);
    colorPointer[0] = skyColor;
    colorNamePointer[0]="sky";
    ofColor buildingColor = ofColor(128, 0, 0 );
    colorPointer[1] = buildingColor;
    colorNamePointer[1]="building";
    ofColor poleColor = ofColor(192,192,128);
    colorPointer[2] = poleColor;
    colorNamePointer[2]="pole";
    ofColor roadMarkingColor = ofColor(255,69,0);
    colorPointer[3] = roadMarkingColor;
    colorNamePointer[3]="roadMarking";
    ofColor roadColor = ofColor(128,64,128);
    colorPointer[4] = roadColor;
    colorNamePointer[4]="road";
    ofColor pavementColor = ofColor(60,40,222);
    colorPointer[5] = pavementColor;
    colorNamePointer[5]="pavement";

    ofColor treeColor = ofColor(128,128,0);
    colorPointer[6] = treeColor;
    colorNamePointer[6]="tree";
    ofColor signSymbolColor = ofColor(192,128,128);
    colorPointer[7] = signSymbolColor;
    colorNamePointer[7]="signSymbol";
    ofColor fenceColor = ofColor(64,64,128);
    colorPointer[8] = fenceColor;
    colorNamePointer[8]="fence";
    ofColor vehicleColor = ofColor(64,0,128);
    colorPointer[9] = vehicleColor;
    colorNamePointer[9]="vehicle";
    ofColor pedestrianColor = ofColor(64,64,0);
    colorPointer[10] = pedestrianColor;
    colorNamePointer[10]="pedestrian";
    ofColor bikeColor = ofColor(0,128,192);
    colorPointer[11] = bikeColor;
    colorNamePointer[11]="bike";



}

void SegnetColourInspector::draw(int cornerX,int cornerY, int width,int height){
    ofPushMatrix();
    ofTranslate(cornerX,cornerY);
    ofFill();

    int rectWidth = width/3;
    int rectHeight = height/4;

    int hOffset, wOffset;
    int colorIndex =0;
    for(int row = 0; row<4; row++){
        hOffset = row*rectHeight;
        for(int col = 0; col<3; col++){
            wOffset = col*rectWidth;

            ofSetColor(colorPointer[colorIndex]);
            ofDrawRectangle(wOffset, hOffset, rectWidth, rectHeight);
            ofSetColor(255);
            ofDrawBitmapString(colorNamePointer[colorIndex], wOffset+5, hOffset+15);
            colorIndex++;
        }

    }
    ofPopMatrix();
}
