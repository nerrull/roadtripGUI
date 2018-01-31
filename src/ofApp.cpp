#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetVerticalSync(true);
    //ofSetFrameRate(60);

    audioToggle = true;
    std::cout << "listening for osc messages on port " << PORT << "\n";
    receiver.setup( PORT );
    featureValues =  new float[24];
    activeIndex = -1;
    for (int i=0; i<24; i++) {
        featureValues[i] = 0.0;
    }

    windowResized(0,0);

    ofBackground(0);

    soundStream.printDeviceList();
    soundStream.setDeviceID(6); //Is computer-specific

    soundStream.setup(this, 0, 2, 88200*2, IN_AUDIO_BUFFER_LENGTH, 4);

}

//--------------------------------------------------------------
void ofApp::update(){
    getOscMessage();
    imageManager.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    int offset =spaceRemainder/2;

    ofSetColor(255);

    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofDrawBitmapString(strm.str(),20, 20);

    int width =ofGetWidth();
    int height=ofGetHeight();

    for(int i = 0; i<24; i++){
        if (activeIndex ==i){
            ofSetColor(245);
        }
        else{
            ofSetColor(125);
        }

        offset+=rectangleOffset;
        float value =ofRandom(0.,1.);
        int height = max(minHeight, int(featureValues[i] * rectangleMaxHeight));
        ofNoFill();
        ofSetLineWidth(1);
        ofDrawRectangle(offset,rectangleTop, rectangleWidth, ofGetHeight()/3);
        ofSetLineWidth(0);
        ofFill();
        ofDrawRectangle(offset+3,ofGetHeight()+3-height, rectangleWidth-6, height-6);

        offset+=rectangleWidth;

    }

    waveform.draw(0, 0, width/3, height/3 );
    waveform.draw2D(2*width/3, 0, width/3, 2*height/3 );

    colourInspector.draw(0,height/3, width/3, height/3 -10);
    imageManager.draw(width/3, 0, width/3, 2*height/3 -10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}



//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    audioToggle = !audioToggle;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

    rectangleWidth = ofGetWidth()/24-20;
    int emptySpace = ofGetWidth() -24*rectangleWidth;
    rectangleTop = ofGetHeight()*2/3;
    rectangleOffset = emptySpace/25;
    spaceRemainder = emptySpace - 25*rectangleOffset;
    minHeight = 7;
    rectangleMaxHeight = ofGetHeight()*1/3;

    ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Offset "<<rectangleOffset;
    ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Remaining space "<< spaceRemainder;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}



void ofApp::getOscMessage() {
    // hide old messages
//    for ( int i=0; i<NUM_MSG_STRINGS; i++ )
//    {
//        if ( timers[i] < ofGetElapsedTimef() )
//            msg_strings[i] = "";
//    }

    // check for waiting messages
    while( receiver.hasWaitingMessages() )
    {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage( &m );


        // check for mouse button message
        if ( m.getAddress().compare( string("/FEATURE_VALUES") ) == 0 )
        {
            // the single argument is a string
            //strcpy( next_video, m.getArgAsString( 0 ) );
            for (int i =0; i<24;i++){
                featureValues[i] = m.getArgAsFloat(i);
            }
            string fileName = m.getArgAsString(24);
            activeIndex = m.getArgAsInt(25);

            imageManager.loadImages(fileName);
            //featureValues = new float[24];
            //std::memcpy(featureValues, values.getData(), 24*sizeof(float));
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Message received : " ;
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;


        }
        else
        {
            printf( "WOops" );

        }

    }
}



void ofApp::audioIn(ofSoundBuffer & buffer){
    if (audioToggle){
        waveform.receiveBuffer(buffer);
    }
}

