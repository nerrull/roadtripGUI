#include "segnetcolourinspector.h"


SegnetColourInspector::SegnetColourInspector()
{
    //Single colours
    ofColor buildingColor = ofColor(128, 0, 0 );
    ofColor pavementColor = ofColor(60,40,222);
    ofColor roadColor = ofColor(128,64,128);
    ofColor skyColor = ofColor(128,128,128);
    ofColor treeColor = ofColor(128,128,0);
    ofColor vehicleColor = ofColor(64,0,128);

    colors.push_back(buildingColor);
    colors.push_back(pavementColor);
    colors.push_back(roadColor);
    colors.push_back(skyColor);
    colors.push_back(treeColor);
    colors.push_back(vehicleColor);

    //Colour pairs
    vector<ofColor> cp;
    ofColor roadMarkingColor = ofColor(255,69,0);
    ofColor signSymbolColor = ofColor(192,128,128);
    cp.push_back(roadMarkingColor);
    cp.push_back(signSymbolColor);
    color_pairs.push_back(cp);

    cp.clear();
    ofColor fenceColor = ofColor(64,64,128);
    ofColor poleColor = ofColor(192,192,128);
    cp.push_back(fenceColor);
    cp.push_back(poleColor);
    color_pairs.push_back(cp);

    cp.clear();
    ofColor pedestrianColor = ofColor(64,64,0);
    ofColor bikeColor = ofColor(0,128,192);
    cp.push_back(pedestrianColor);
    cp.push_back(bikeColor);
    color_pairs.push_back(cp);

}

void SegnetColourInspector::setLayout(int cornerX,int cornerY, int w,int h){
    xOffset = cornerX;
    yOffset = cornerY;
    width = w;
    height = h;
}

void SegnetColourInspector::draw(){
    ofPushMatrix();
    ofTranslate(xOffset,yOffset);
    ofFill();

    int rectWidth = width/9;
    int rectHeight = height/2;

    int hOffset, wOffset;
    wOffset=0;
    hOffset=0;
    int pad = 3;
    for(int col = 0; col<6; col++){
         ofFill();
         ofSetColor(colors[col]);
         ofDrawRectangle(wOffset+pad, hOffset+pad+1, rectWidth-2*pad, rectHeight);
//         ofSetColor(255);
//         ofNoFill();
//         ofSetLineWidth(2);
//         ofDrawRectangle(wOffset, hOffset, rectWidth, rectHeight);
         wOffset +=rectWidth;
    }

    for(int col = 0; col<3; col++){
        ofFill();
        ofSetColor(color_pairs[col][0]);
        ofDrawRectangle(wOffset+pad, hOffset+pad+1, rectWidth/2 -pad, rectHeight);
        ofSetColor(color_pairs[col][1]);
        ofDrawRectangle(wOffset +rectWidth/2, hOffset+pad, rectWidth/2-pad, rectHeight);
//        ofSetColor(255);
//        ofNoFill();
//        ofSetLineWidth(2);
//        ofDrawRectangle(wOffset, hOffset, rectWidth, rectHeight);
        wOffset +=rectWidth;
    }

    ofPopMatrix();
}
