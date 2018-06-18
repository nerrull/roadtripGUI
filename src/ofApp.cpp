#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofGetWindowPtr()->setVerticalSync(true);
    //ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    glPointSize(2.0);
    font.load("verdana.ttf", 12);
    initAudio();
    DEV_MODE = True;
    if (!DEV_MODE) imageManager = new ImageManager();

    std::cout << "listening for osc messages on port " << CONTROL_RECEIVE_PORT << " and "<<  PLAYING_RECEIVE_PORT<<"\n";
    receiver_python_controller.setup( PYTHON_CONTROL_RECEIVE_PORT );

    receiver_controller.setup( CONTROL_RECEIVE_PORT );
    receiver_playing.setup( PLAYING_RECEIVE_PORT );

    std::cout << "Sending on " << SEND_PORT << "\n";
    sender.setup("localhost",SEND_PORT);

    search_timer =ofGetElapsedTimeMillis();

    string db_path = "/media/rice1902/OuterSpace2/dataStore/database.h5";
    databaseLoader.loadHDF5Data(db_path);

    pointCloudTree = new PointCloudTreeSearch(&databaseLoader);
    pointCloudTree->initPoints();

    int num_features = databaseLoader.getFeatures()[0][0].size();
    //Initialize weight to zero
    featureValues.resize(num_features);
    lastFeatureValues.resize(num_features);
    featureWeights = vector<float>(num_features, 0.);
    inactiveCounter.resize(num_features);
    desiredFeatureValues.resize(num_features);

    speedSetting =0;
    activityTimer =0;

    //Make loudness the only active feature
    featureWeights[0]= 1.;

    windowResized(ofGetWidth(),ofGetHeight());
    ofBackground(0);

}

void ofApp::initAudio(){
    audioToggle = true;
    soundStream.printDeviceList();
    soundStream.setDeviceID(6); //Is computer-specific
    soundStream.setup(this, 0, 2, 44100, IN_AUDIO_BUFFER_LENGTH, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
    getOscMessage();
    if (!DEV_MODE) imageManager->update();
    pointCloudTree->updateSearchSpace(desiredFeatureValues, featureWeights);

    uint64_t currentTime = ofGetElapsedTimeMillis();
    if ((currentTime - search_timer)>100)
    {
        pointCloudTree->getKNN(desiredFeatureValues, featureWeights);
        search_timer =currentTime;
    }

    vector<string> videoNames=  databaseLoader.getVideoNamesFromIndexes(pointCloudTree->getSearchResultIndexes());
    if (!vectorsAreEqual(videoNames, lastVideos) && videoNames.size()>0){
        publishVideos(videoNames, false);
    }

    pointCloudTree->update();
    lastVideos = videoNames;
}

//--------------------------------------------------------------
void ofApp::draw(){


    ofBackground(0);
    ofSetColor(255);

    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofDrawBitmapString(strm.str(),20, 20);

    int windowWidth =ofGetWidth();
    int windowHeight=ofGetHeight();

    waveform.draw(0, 0, windowWidth/3, windowHeight/3 );
    waveform.drawSpectrum(0, windowHeight/3 , windowWidth/3, windowHeight/3 -10 );

    colourInspector.draw(0,windowHeight/3, windowWidth/3, windowHeight/3 -10);
    pointCloudTree->draw();

    if (!DEV_MODE) imageManager->draw(2*windowWidth/3, 0, windowWidth/3, 2*windowHeight/3 -10);

    //drawColors();

    drawControls(windowWidth, windowHeight);

    //Update active features
    for (int i = 0; i <featureWeights.size(); i++)
    {
        if (featureWeights[i] >0.){
            inactiveCounter[i]+=1;
        }

        //After 5 seconds of inactivity decrement the weight by 0.1 every 0.5 seconds
        if (inactiveCounter[i]>300 && featureWeights[i] >0){
//            if ((inactiveCounter[i] %30) ==0){
                featureWeights[i] = CLAMP(featureWeights[i] -0.1/30, 0., 1.);
                desireChanged=True;
//            }
        }
    }

    //If no activity go to low volume low playback speed
    float totalWeight = std::accumulate(featureWeights.begin(), featureWeights.end(), 0.);

    if (!input_activity && totalWeight <1.){
        //Decrement amplitude if it's not already at 0
        if (desiredFeatureValues[0] != 0.){
            desiredFeatureValues[0] = CLAMP(desiredFeatureValues[0] - 0.01, 0., 1.);
        }
        totalWeight = std::accumulate(featureWeights.begin()+1, featureWeights.end(), 0.);
        featureWeights[0] = 1-totalWeight;
        //pointCloudTree->updateSearchSpace(desiredFeatureValues, indexWeights);
    }


    if (speedChanged){
        publishSpeed();
        speedChanged = false;
    }


    //Reset the input activity flag
    input_activity= False;

}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void ofApp::drawControls(int windowWidth, int windowHeight){

    int offset =spaceRemainder/2;

    //Draw speed control
    {
        if (speedChanged){
             ofSetColor(240);
        }
        else{
            ofSetColor(125);
        }
        offset+=rectangleOffset;
        float value =ofRandom(0.,1.);
        int height = max(minHeight, speedSetting *rectangleMaxHeight/8);
        ofNoFill();
        ofSetLineWidth(1);
        ofDrawRectangle(offset,rectangleTop, rectangleWidth, ofGetHeight()/3);
        ofSetLineWidth(0);
        ofFill();
        ofDrawRectangle(offset+3,ofGetHeight()+3-height, rectangleWidth-6, height-6);

        ofPushMatrix();
        ofSetColor(255);
        ofTranslate(offset, windowHeight-20,1);
        ofRotateZ(-90.);
        string name = "BPM";
        float fontWidth = font.stringHeight(name);
        font.drawString(name,0,rectangleWidth/2 +fontWidth/2);
        ofPopMatrix();

        offset+=rectangleWidth;

    }

    //Draw feature controls
    for(int i = 0; i<featureValues.size(); i++){


        ofSetColor(125 + 120 *featureWeights[i]);


        offset+=rectangleOffset;
        float value =ofRandom(0.,1.);
        float diff = featureValues[i] - lastFeatureValues[i];
        float inc = sgn(diff)*diff*diff;
        lastFeatureValues[i]+=inc;
        int height = max(minHeight, int((lastFeatureValues[i] * rectangleMaxHeight)));
        ofNoFill();
        ofSetLineWidth(1);
        ofDrawRectangle(offset,rectangleTop, rectangleWidth, ofGetHeight()/3);
        ofSetLineWidth(0);
        ofFill();
        ofDrawRectangle(offset+3,ofGetHeight()+3-height, rectangleWidth-6, height-6);


        ofDrawCircle(offset+rectangleWidth, ofGetHeight() - desiredFeatureValues[i]*rectangleMaxHeight, 3);

        ofPushMatrix();
        ofSetColor(255);
        ofTranslate(offset, windowHeight-20,1);
        ofRotateZ(-90.);
        string name = databaseLoader.feature_names[i];
        float fontWidth = font.stringHeight(name);
        font.drawString(name,0,rectangleWidth/2 +fontWidth/2);
        ofPopMatrix();


        offset+=rectangleWidth;
    }
}

float ofApp::getColorValue(string id){
    auto it= find(featureNames.begin(), featureNames.end(), id);
    if(it < featureNames.end()) {
        auto index = std::distance(featureNames.begin(), it);
        return featureValues[index];
    }
    else return -1.;
}

void ofApp::drawColors(){

    float h,s,v, h_m,s_m,v_m;

    h = getColorValue("h");
    s = getColorValue("s");
    v = getColorValue("v");
    h_m = getColorValue("h_m");
    s_m = getColorValue("s_m");
    v_m = getColorValue("v_m");

    int windowWidth = ofGetWidth();
    ofPushMatrix();
    ofTranslate(5*windowWidth/6, 20);
    int boxWidth = windowWidth/6;

    ofColor c1, c2,c3,c4,c5;
    c1 = ofColor(0);
    c2 = ofColor(0);
    c3 = ofColor(0);
    c4 = ofColor(0);
    c5 = ofColor(0);

    if (h!=-1. && s!=-1. && v!=-1.){
        c1 = c1.fromHsb(h*255, v*255, v*255);
    }
    if (h_m!=-1. && s_m!=-1. && v_m!=-1.){
        c2 = c2.fromHsb(h_m*255, s_m*255, v_m*255);
    }
    if (h_m!=-1.){
        c3 =c3.fromHsb(h_m*255, 255, 255);
    }
    if (s_m!=-1.){
        c4 = ofColor(s_m*255);
    }
    if (v_m!=-1.){
        c5 = ofColor(v_m*255);
    }

    ofSetColor(c1);
    ofDrawRectangle(0,0, boxWidth/2, boxWidth/2);
    ofSetColor(255);
    font.drawString("Palette",0,20);

    ofSetColor(c2);
    ofDrawRectangle(boxWidth/2,0, boxWidth/2, boxWidth/2);
    ofSetColor(255);
    font.drawString("Moyenne",boxWidth/2,20);


    ofSetColor(c3);
    ofDrawRectangle(0,boxWidth/2, boxWidth/3, boxWidth/3);
    ofSetColor(255);
    font.drawString("H_m",0,boxWidth/2+20);


    ofSetColor(c4);
    ofDrawRectangle(boxWidth/3,boxWidth/2, boxWidth/3, boxWidth/3);
    ofSetColor(255);
    font.drawString("S_m",boxWidth/3,boxWidth/2+20);

    ofSetColor(c5);
    ofDrawRectangle(2*boxWidth/3,boxWidth/2, boxWidth/3, boxWidth/3);
    ofSetColor(255);
    font.drawString("V_m",2*boxWidth/3,boxWidth/2+20);

    ofPopMatrix();
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

    pointCloudTree->setViewPort(ofGetWidth()/3, 0, ofGetWidth()/3, 2*  ofGetHeight()/3);

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

    while( receiver_playing.hasWaitingMessages() )
    {
        ofxOscMessage m;
        receiver_playing.getNextMessage( &m );
        if ( m.getAddress().compare( string("/PLAYING_VIDEO") ) == 0 )
        {
            string fileName = m.getArgAsString(0);
            if (!DEV_MODE) imageManager->loadImages(fileName);
            lastFeatureValues = featureValues;
            featureValues = databaseLoader.getFeaturesFromName(fileName);
            pointCloudTree->setPlayingIndex(databaseLoader.getVideoIndexFromName(fileName));


        }
        else
        {
            ofLogError()<<  m.getAddress();

        }

    }
    // check for waiting messages
    while( receiver_python_controller.hasWaitingMessages() )
    {
        // get the next message
        ofxOscMessage m;
        receiver_python_controller.getNextMessage( &m );

        // check for mouse button message
        if ( m.getAddress().compare( string("/FEATURE_DIFFS") ) == 0 )
        {
            // the single argument is a string
            //strcpy( next_video, m.getArgAsString( 0 ) );
            for (int i =0; i<24;i++){
                float diff =  m.getArgAsFloat(i);
                if (abs(diff) >0){
                    desiredFeatureValues[i] =CLAMP(desiredFeatureValues[i] +diff, 0., 1.);
                    inactiveCounter[i] = 0;
                    featureWeights[i]=1.0;

                    desireChanged=true;
                    activityTimer = 0;

                    input_activity = True;
                }
            }

            //featureValues = new float[24];
            //std::memcpy(featureValues, values.getData(), 24*sizeof(float));
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "FEATURE_DIFFS Message received : " ;

            //ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Message received : " ;
            //ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;
        }

        if ( m.getAddress().compare( string("/SET_SPEED") ) == 0 )
        {

            speedSetting=  m.getArgAsInt(0);
            speedChanged =true;

            //featureValues = new float[24];
            //std::memcpy(featureValues, values.getData(), 24*sizeof(float));
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "FEATURE_DIFFS Message received : " ;

            //ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Message received : " ;
            //ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;
        }



        else if ( m.getAddress().compare( string("/FEATURE_VALUES") ) == 0 )
        {
            // the single argument is a string
            //strcpy( next_video, m.getArgAsString( 0 ) );
            for (int i =0; i<24;i++){
                featureValues[i] = m.getArgAsFloat(i);
            }
            string fileName = m.getArgAsString(24);

            //imageManager.loadImages(fileName);
            //featureValues = new float[24];
            //std::memcpy(featureValues, values.getData(), 24*sizeof(float));
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Message received : " ;
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;


        }

        else if ( m.getAddress().compare( string("/FEATURE_NAMES") ) == 0 )
        {
            // the single argument is a string
            //strcpy( next_video, m.getArgAsString( 0 ) );
            featureNames.clear();
            for (int i =0; i<24;i++){
                featureNames.push_back(m.getArgAsString(i));
            }

            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Feature names : " ;
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Ex : " << featureNames[0]<<" to " <<featureNames[23] ;
        }
        else
        {
            ofLogError()<<  m.getAddress();

        }
    }

    while( receiver_controller.hasWaitingMessages() )
    {
        // get the next message
        ofxOscMessage m;
        receiver_controller.getNextMessage( &m );

        if ( m.getAddress().compare( string("/ENCODER/STEP") ) == 0 )
        {
            handle_knob_input(m);
        }

        if ( m.getAddress().compare( string("/ENCODER/SWT") ) == 0 )
        {
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << " SWT Message received : " ;
            int idx = m.getArgAsInt(0);
            int swt = m.getArgAsInt(0);


            //ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;
        }

        else
        {
            ofLogError()<<  m.getAddress();
        }
    }
}

void ofApp::handle_knob_input(ofxOscMessage m){
    ofLogError(ofToString(ofGetElapsedTimef(),3)) << " Step Message received : " ;
    int i = m.getArgAsInt(0)-1;
    float step = float(m.getArgAsInt(1) -0.5)*2/100.;

    desiredFeatureValues[i] =CLAMP(desiredFeatureValues[i]+step, 0., 1.);
    featureWeights[i]=1.0;
    inactiveCounter[i] = 0;
    desireChanged=true;
    activityTimer = 0;
    input_activity = True;

    //ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;
}

bool ofApp::vectorsAreEqual(vector<string>v1, vector<string> v2){
    if (v1.size()!= v2.size()){
        return False;
    }
    else if (!std::equal(v1.begin(), v1.begin() + v1.size(), v2.begin())){
       return False;
    }
    return True;
}

void ofApp::publishVideos(vector<string> v1, bool log){
    ofxOscMessage m;
    if (log) ofLogError(ofToString(ofGetElapsedTimef(),3))<<"Sending batch of length: "<<v1.size() ;

    m.setAddress("/VIDEO_NAMES");
    m.addInt32Arg(v1.size());
    for (std::size_t i = 0; i < v1.size(); i++){
        m.addStringArg(v1[i]);

        if (log) ofLogError(ofToString(ofGetElapsedTimef(),3))<<v1[i];
    }
    sender.sendMessage(m);
}

void ofApp::publishSpeed(){
    ofxOscMessage m;
    ofLogError(ofToString(ofGetElapsedTimef(),3))<<"Sending speed value" ;

    m.setAddress("/SET_SPEED");
    m.addInt32Arg(speedSetting);
    sender.sendMessage(m);
}

void ofApp::audioIn(ofSoundBuffer & buffer){
    if (audioToggle){
        waveform.receiveBuffer(buffer);
    }
}

