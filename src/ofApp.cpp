#include "ofApp.h"5


//--------------------------------------------------------------
void ofApp::setup(){
    ofGetWindowPtr()->setVerticalSync(true);
    ofSetVerticalSync(true);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    glPointSize(2.0);
    font.load("verdana.ttf", 12);
    initAudio();
    initNames();
    DEV_MODE = false;
//    ofSetLogLevel(OF_LOG_ERROR);
    ofSetLogLevel(OF_LOG_NOTICE);


    if (!DEV_MODE) imageManager = new ImageManager();
    num_knobs =23;
    search_timer =ofGetElapsedTimeMillis();

    string db_path = "/home/nuc/Documents/dataStore/database.h5";
    databaseLoader.loadHDF5Data(db_path);

    pointCloudTree = new PointCloudTreeSearch(&databaseLoader);
    pointCloudTree->initPoints();

    int num_features = databaseLoader.getFeatures()[0][0].size();
    //Initialize weight to zero
    featureValues.resize(num_features);
    lastFeatureValues.resize(num_features);
    lastFeatureWeights.resize(num_features);
    featureWeights = vector<float>(num_features, 0.);
    inactiveCounter.resize(num_features);
    desiredFeatureValues.resize(num_features);

    speedSetting =0;
    activityTimer =0;

    //Make loudness the only active feature
    featureWeights[0]= 1.;
    current_playing_video ="";

    windowResized(ofGetWidth(),ofGetHeight());
    ofBackground(0);

    for(int i =0; i < num_features;i++){
        coms.sendLightControl(i+2, 0);
    }
    coms.sendLightControl(2,4095);


}

void ofApp::initNames(){
    feature_names_en.push_back("Loudness");
    feature_names_en.push_back("Spectral Centroid");
    feature_names_en.push_back("Percussiveness");
    feature_names_en.push_back("Pitched");
    feature_names_en.push_back("Harmonicity");
    feature_names_en.push_back("Vehicle speed");
    feature_names_en.push_back("Instability");
    feature_names_en.push_back("Engine temperature");
    feature_names_en.push_back("Engine RPM");
    feature_names_en.push_back("Intake temperature");
    feature_names_en.push_back("Roll");
    feature_names_en.push_back("Hue");
    feature_names_en.push_back("Lightness");
    feature_names_en.push_back("Uncertainty");
    feature_names_en.push_back("Building");
    feature_names_en.push_back("Pavement");
    feature_names_en.push_back("Road");
    feature_names_en.push_back("Sky");
    feature_names_en.push_back("Vegetation");
    feature_names_en.push_back("Vehicle");
    feature_names_en.push_back("Signage");
    feature_names_en.push_back("Fence Pole");
}

void ofApp::initAudio(){
    audioToggle = true;
    soundStream.printDeviceList();
    soundStream.setDeviceID(7); //Is computer-specific
    soundStream.setup(this, 0, 2, 44100, IN_AUDIO_BUFFER_LENGTH, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
    updateOSC();
    if (!DEV_MODE) imageManager->update();
    pointCloudTree->update();

    uint64_t currentTime = ofGetElapsedTimeMillis();
    if ((currentTime - search_timer)>100)
    {
        pointCloudTree->updateSearchSpace(desiredFeatureValues, featureWeights);
        pointCloudTree->getKNN(desiredFeatureValues, featureWeights);
        search_timer =currentTime;
        video_indexes = pointCloudTree->getSearchResultIndexes();

        std::ostringstream oss;
         if (!video_indexes.empty())
         {
           // Convert all but the last element to avoid a trailing ","
           std::copy(video_indexes.begin(), video_indexes.end()-1,
               std::ostream_iterator<int>(oss, ","));

           // Now add the last element with no delimiter
           oss << video_indexes.back();
         }

         //std::cout << oss.str() << std::endl;
         vector<string> videoNames = databaseLoader.getVideoNamesFromIndexes(pointCloudTree->getSearchResultIndexes());
        if (input_activity){
            if (current_playing_video != videoNames[0]){
                coms.publishVideoNow( videoNames[0], true);
                current_playing_video = videoNames[0];
                lastVideos.clear();
                lastVideos.push_back(videoNames[0]);
            }
            input_activity= false;
        }

        else if (!vectorsAreEqual(videoNames, lastVideos) && videoNames.size()>0){
            coms.publishVideos(videoNames, true);
            lastVideos = videoNames;
        }
    }


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
                desireChanged=true;
//            }
        }

        else if (inactiveCounter[i]>300 == featureWeights[i] ==0.){
            continue;
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

        //Decrement speed while inactive
        if (speedSetting >0 && inactiveCounter[0] > 300 &&inactiveCounter[0] %60 ==0){
            setSpeed(speedSetting -1);
        }
        //pointCloudTree->updateSearchSpace(desiredFeatureValues, indexWeights);
    }

    //Do coms stuff
    if (speedChanged){
        coms.publishSpeed(speedSetting);
        speedChanged = false;
    }

    for (int i = 0; i <featureWeights.size(); i++)
    {
        if (lastFeatureWeights[i]!=featureWeights[i]){
            coms.sendLightControl(i+2, int(featureWeights[i]*4096));
        }
    }

    //Reset the input activity flag
    lastFeatureValues = featureValues;
    lastFeatureWeights = featureWeights;



}

//--------------------------------------------------------------
void ofApp::draw(){


    ofBackground(0);
    ofSetColor(255);
    if (DEV_MODE){
        std::stringstream strm;
        strm << "fps: " << ofGetFrameRate();
        ofDrawBitmapString(strm.str(),20, 20);
    }


    int windowWidth =ofGetWidth();
    int windowHeight=ofGetHeight();

    waveform.draw(0, 0, windowWidth/4, windowHeight/3 );
    waveform.drawSpectrum(0, windowHeight/3 , windowWidth/4, windowHeight/3 -10 );

    colourInspector.draw(0,windowHeight/3, windowWidth/4, windowHeight/3 -10);
    pointCloudTree->draw();

    if (!DEV_MODE) imageManager->draw(3*windowWidth/4, 0, windowWidth/4, 2*windowHeight/3 -10);

    //drawColors();

    drawControls(windowWidth, windowHeight);


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
        ofSetColor(80);
//        ofTranslate(offset, windowHeight-20,1);
        ofTranslate(offset, windowHeight-20,1);

        ofRotateZDeg(-90.);
        string name = "BPM";
        float fontWidth = font.stringHeight(name);
        float fontHeight = font.stringWidth(name);
        font.drawString(name,0,rectangleWidth/2 +fontWidth/2);
        ofPopMatrix();

        offset+=rectangleWidth;

    }

    //Draw feature controls
    for(int i = 0; i<featureValues.size(); i++){


        ofSetColor(80);
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
        ofRotateZDeg(-90.);
        string name = feature_names_en[i];

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

    rectangleWidth = ofGetWidth()/num_knobs-20;
    int emptySpace = ofGetWidth() -num_knobs*rectangleWidth;
    rectangleTop = ofGetHeight()*2/3 ;
    rectangleOffset = emptySpace/num_knobs+1;
    spaceRemainder = emptySpace - (num_knobs+1)*rectangleOffset;
    minHeight = 7;
    rectangleMaxHeight = ofGetHeight() -rectangleTop;

    pointCloudTree->setViewPort(ofGetWidth()/4, 0, ofGetWidth()/2, 2*  ofGetHeight()/3);

    ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Offset "<<rectangleOffset;
    ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Remaining space "<< spaceRemainder;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

void ofApp::updatePlayingVideo(string video){
    current_playing_video = video;
    if (!DEV_MODE) imageManager->loadImages(current_playing_video);
    lastFeatureValues = featureValues;
    featureValues = databaseLoader.getFeaturesFromName(current_playing_video);
    pointCloudTree->setPlayingIndex(databaseLoader.getVideoIndexFromName(current_playing_video));
}

void ofApp::incrementSpeed(int step){
    speedSetting=  CLAMP(speedSetting+step, 0, 8);
    speedChanged =true;
}

void ofApp::setSpeed(int value){
    speedSetting=  CLAMP(value, 0, 8);
    speedChanged =true;
}


void ofApp::incrementFeatureTarget(int index, float step){
    desiredFeatureValues[index] =CLAMP(desiredFeatureValues[index]+step, 0., 1.);
    featureWeights[index]=1.0;
    inactiveCounter[index] = 0;
    desireChanged=true;
    activityTimer = 0;
    input_activity = true;
}

void ofApp::toggleFeatureTarget(int index){
    float v = desiredFeatureValues[index];
    int target_value = 0.;
    if (v<.5) target_value = 1.;
    desiredFeatureValues[index] =CLAMP(target_value, 0., 1.);
    featureWeights[index]=1.0;
    inactiveCounter[index] = 0;
    desireChanged=true;
    activityTimer = 0;
    input_activity = true;
}

void ofApp::updateOSC() {
    // hide old messages
    while( coms.receiver_playing.hasWaitingMessages() )
    {
        ofxOscMessage m;
        coms.receiver_playing.getNextMessage( m );
        if ( m.getAddress().compare( string("/PLAYING_VIDEO") ) == 0 )
        {
            updatePlayingVideo(m.getArgAsString(0));
        }
        else
        {
            ofLogVerbose()<<  m.getAddress();
        }

    }
    //Uncomment this when using controller simulator
    //handlePythonMessages();

    while( coms.receiver_controller.hasWaitingMessages() )
    {
        // get the next message
        ofxOscMessage m;
        coms.receiver_controller.getNextMessage( m );

        if ( m.getAddress().compare( string("/ENCODER/STEP") ) == 0 )
        {
            handleKnobInput(m);
        }

        if ( m.getAddress().compare( string("/ENCODER/SWT") ) == 0 )
        {
            int idx = m.getArgAsInt(0);
            int swt = m.getArgAsInt(1);
            ofLogVerbose(ofToString(ofGetElapsedTimef(),3)) << " SWT Message received : Index"<< idx << "value"<<swt ;
            if (swt >0) handleButtonInput(idx);
        }

        else
        {
            ofLogVerbose()<<  m.getAddress();
        }
    }
}

void ofApp::handleKnobInput(ofxOscMessage m){
//    ofLogDebug(ofToString(ofGetElapsedTimef(),3)) << " Step Message received : " ;
    int i = m.getArgAsInt(0);

    if (i ==1){
        int step =m.getArgAsInt(1);
        if (step ==0) step = -1;
        incrementSpeed(step);
        return;
    }

     i =i-2;

    float step = (float(m.getArgAsInt(1)) -0.5)*2/100.;
    incrementFeatureTarget(i, step);
}



void ofApp::handleButtonInput(int index){
//    ofLogDebug(ofToString(ofGetElapsedTimef(),3)) << " Step Message received : " ;

    if (index ==1){
        setSpeed(0);
        return;
    }

     index =index-2;
     toggleFeatureTarget(index);
}


bool ofApp::vectorsAreEqual(vector<string>v1, vector<string> v2){
    if (v1.size()!= v2.size()){
        return false;
    }
    else if (!std::equal(v1.begin(), v1.begin() + v1.size(), v2.begin())){
       return false;
    }
    return true;
}


void ofApp::audioIn(ofSoundBuffer & buffer){
    if (audioToggle){
        waveform.receiveBuffer(buffer);
    }
}

