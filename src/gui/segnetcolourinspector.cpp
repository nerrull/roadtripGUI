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
    colorMap["building"] = buildingColor;
    colorMap["pavement"] = pavementColor;
    colorMap["road"] = roadColor;
    colorMap["sky"] = skyColor;
    colorMap["tree"] = treeColor;
    colorMap["vehicle"] = vehicleColor;


    //Colour pairs
    pair<ofColor, ofColor> cp;
    ofColor roadMarkingColor = ofColor(255,69,0);
    ofColor signSymbolColor = ofColor(192,128,128);
    colorPairMap["roadSign"]=pair<ofColor, ofColor>(roadMarkingColor, signSymbolColor);

    ofColor fenceColor = ofColor(64,64,128);
    ofColor poleColor = ofColor(192,192,128);
    colorPairMap["fencePole"]=pair<ofColor, ofColor>(fenceColor, poleColor);

    ofColor pedestrianColor = ofColor(64,64,0);
    ofColor bikeColor = ofColor(0,128,192);
    colorPairMap["pedestrianBike"]=pair<ofColor, ofColor>(pedestrianColor,bikeColor);

}

void SegnetColourInspector::setLayout(int cornerX,int cornerY, int w,int h){
    xOffset = cornerX;
    yOffset = cornerY;
    width = w;
    height = h;
}

void SegnetColourInspector::draw(){
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(xOffset,yOffset);
    ofFill();

    float rectWidth = width/9.;
    int rectHeight = height;

    float hOffset, wOffset;
    wOffset=0;
    hOffset=0;
    int pad = 3;
    for(int col = 0; col<6; col++){
         ofFill();
         ofSetColor(colorMap[segnetSingleFeatures[col]]);
         ofSetLineWidth(0);

         ofDrawRectangle(wOffset+1, hOffset+1, rectWidth-2, rectHeight-2);
         ofSetColor(255);
         ofNoFill();
         ofSetLineWidth(2);
         ofDrawRectangle(wOffset, hOffset, rectWidth, rectHeight);
         wOffset +=rectWidth;
    }

    for(int col = 0; col<3; col++){
        ofFill();
        ofSetLineWidth(0);
        ofSetColor(colorPairMap[segnetPairedFeatures[col]].first);
        ofDrawRectangle(wOffset+1, hOffset+1, rectWidth/2, rectHeight-2);
        ofSetColor(colorPairMap[segnetPairedFeatures[col]].second);
        ofDrawRectangle(wOffset +rectWidth/2, hOffset+1, rectWidth/2, rectHeight-2);
        ofSetColor(255);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawRectangle(wOffset, hOffset, rectWidth, rectHeight);
        wOffset +=rectWidth;
    }
    ofPopStyle();
    ofPopMatrix();
}
