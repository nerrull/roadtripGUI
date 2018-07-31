#include "ofApp.h"
#include "ofxJsonSettings.h"


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


//--------------------------------------------------------------
void ofApp::setup(){
    ofGetWindowPtr()->setVerticalSync(true);
    ofSetVerticalSync(true);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    ofSetCircleResolution(100);
    glPointSize(2.0);

    Settings::get().load("settings.json");
    string db_path = Settings::getString("db_path");
    DEV_MODE = Settings::getBool("dev_mode");
    featureTimeout = Settings::getInt("feature_timeout");
    featureDecayRate = Settings::getFloat("feature_decay_sec")/60.;
    lastActiveFeatureIndex =0;


    if (DEV_MODE) ofSetFrameRate(60);

    font.load("futura.ttf", 12);
    initAudio();
    initNames();
    //    ofSetLogLevel(OF_LOG_ERROR);
    ofSetLogLevel(OF_LOG_NOTICE);


    imageManager = new ImageManager();
    numKnobs =23;
    search_timer =ofGetElapsedTimeMillis();

    databaseLoader.loadHDF5Data(db_path);
//    worldMap.setVideoPoints(databaseLoader.getCoordinates());
    fKNN.init(&databaseLoader);
//    pointCloudRender.initPoints(fKNN.num_points, databaseLoader.colors);
    pointCloudRender.initPoints(databaseLoader.dimension_reduction, databaseLoader.colors);


    int num_features =21;

    //Initialize weight to zero
    featureValues.resize(num_features);
    lastFeatureValues.resize(num_features);
    lastFeatureWeights.resize(num_features);
    featureWeights = vector<float>(num_features, 0.);
    inactiveCounter.resize(num_features,0);
    targetFeatureValues.resize(num_features, 0.);
    featureActive.resize(num_features, false);

    featureGuiElements.resize(num_features+2);
    featureGuiElements[0] = make_unique<FillCircle>();
    featureGuiElements[1] = make_unique<SearchRadiusElement>();

    for (int i =2; i<featureGuiElements.size(); i++){
        featureGuiElements[i]= make_unique<CircleFeatureGuiElement>();
    }
    //Init custom gui elements (hue)
    featureGuiElements[11] = make_unique<ColorCircle>();
    speedSetting =0;
    activityTimer =0;

    //Make loudness the only active feature
    featureWeights[0]= 1.;
    currentPlayingVideo ="";

    windowResized(ofGetWidth(),ofGetHeight());
    ofBackground(0);

    for(int i =0; i < num_features;i++){
        coms.sendLightControl(i+3, 0);
    }
    coms.sendLightControl(3,4095);

    setLayout();
}

void ofApp::initNames(){


    featureNamesEn.push_back("Loudness");
    featureNamesEn.push_back("Spectral\nCentroid");
    featureNamesEn.push_back("Percussiveness");
//    featureNamesEn.push_back("Pitched");
    featureNamesEn.push_back("Bandwidth");
    featureNamesEn.push_back("Vehicle speed");
    featureNamesEn.push_back("Instability");
    featureNamesEn.push_back("Engine RPM");
    featureNamesEn.push_back("Intake T°");
    featureNamesEn.push_back("Roll");
    featureNamesEn.push_back("Hue");
    featureNamesEn.push_back("Lightness");
    featureNamesEn.push_back("Uncertainty");
    featureNamesEn.push_back("Building");
    featureNamesEn.push_back("Pavement");
    featureNamesEn.push_back("Road");
    featureNamesEn.push_back("Sky");
    featureNamesEn.push_back("Vegetation");
    featureNamesEn.push_back("Vehicle");
    featureNamesEn.push_back("Signage");
    featureNamesEn.push_back("Fence/Pole");
    featureNamesEn.push_back("Bike/Pedestrian");


    featureNamesFr.push_back("Volume");
    featureNamesFr.push_back("Centroïde\nSpectral");
    featureNamesFr.push_back("Percussif");
//    featureNamesFr.push_back("Hauteur?");
    featureNamesFr.push_back("Largeur\nde bande");
    featureNamesFr.push_back("Vitesse");
    featureNamesFr.push_back("Instabilité");
    featureNamesFr.push_back("RPM Moteur");
    featureNamesFr.push_back("T° extérieur");
    featureNamesFr.push_back("Inclinaison");
    featureNamesFr.push_back("Teinte?");
    featureNamesFr.push_back("Luminosité");
    featureNamesFr.push_back("Incertitude");
    featureNamesFr.push_back("Bâtiment");
    featureNamesFr.push_back("Trottoir");
    featureNamesFr.push_back("Route");
    featureNamesFr.push_back("Ciel");
    featureNamesFr.push_back("Végétation");
    featureNamesFr.push_back("Véhicule");
    featureNamesFr.push_back("Signalisation");
    featureNamesFr.push_back("Clôture/Pôteau");
    featureNamesFr.push_back("Piéton/Vélo");


    languageIsEnglish = Settings::getBool("english");
    if (languageIsEnglish) {
        durationName= "Duration";
        neighbourName= "Neigbours";

        featureNames = featureNamesEn;
    }
    else {
        durationName= "Durée";
        neighbourName = "Voisins";

        featureNames = featureNamesFr;

    }
}

void ofApp::initAudio(){
    audioToggle = true;
    soundStream.printDeviceList();
    std::vector<ofSoundDevice> devices = soundStream.getDeviceList();
    ofSoundStreamSettings s;
    int device_id = Settings::getInt("audio_device_id");
    soundStream.setDeviceID(device_id);
//    s.numBuffers=4;
//    s.sampleRate=44100;
//    s.bufferSize=IN_AUDIO_BUFFER_LENGTH;
//    soundStream.setup(s);
    soundStream.setup(this, 0, 2, 44100, IN_AUDIO_BUFFER_LENGTH, 4);
}

void ofApp::setLayout(){
    int windowWidth =ofGetWidth();
    int windowHeight=ofGetHeight();

    //Set widget layouts
    float div = 14./23.;

    float rem =1.-div;

    imageManager->setLayout(div*windowWidth, 0, rem*windowWidth, 3*windowHeight/4);
//    worldMap.setLayout(0, windowHeight/4, div*windowWidth, 2* windowHeight/4);
    pointCloudRender.setLayout(0, windowHeight/4, div*windowWidth, 2* windowHeight/4);
    waveform.setLayout(0,0, div*windowWidth, windowHeight/4 );

    //Place knobs
    int controlSectionHeight = windowHeight/4;
    int knobWidth = windowWidth/numKnobs;
    int offset = knobWidth;

    float max_row_1_height  =0;
    float max_row_2_height = 0;
    for (int i = 0; i < featureNames.size(); i++){
        if (i%2 ==0){
            max_row_1_height = max(font.stringHeight(featureNames[i]), max_row_1_height);
        }
        else{
            max_row_2_height = max(font.stringHeight(featureNames[i]), max_row_2_height);
        }
    }

    int textHeight = max_row_1_height + max_row_2_height +20;
    int knobSectionHeight = controlSectionHeight - textHeight;
    int elementHeight = knobSectionHeight/2;
    int elementWidth = knobWidth*1.1;
    int firstRowOffset =max_row_1_height;
    int secondRowOffset =max_row_1_height+elementHeight ;

    //Initialize special knobs
    featureGuiElements[0]->setName(durationName);
    featureGuiElements[1]->setName(neighbourName);

    int x, y;
    x = 0;
    for (int i=0; i<featureGuiElements.size(); i++){
        y = windowHeight - controlSectionHeight;
        if (i%2 ==0){
            featureGuiElements[i]->setTextOnTop(true);
            featureGuiElements[i]->setCircleOffset(firstRowOffset);
        }
        else{
            y +=secondRowOffset;
            featureGuiElements[i]->setTextOnTop(false);
        }

        featureGuiElements[i]->setPosition(x,y);
        featureGuiElements[i]->setSize(elementWidth, elementHeight);

        if (i>=2){
            featureGuiElements[i]->setName(featureNames[i-2]);

        }
        x+=knobWidth;
    }

    ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Offset "<<rectangleOffset;
    ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Remaining space "<< spaceRemainder;
}

//--------------------------------------------------------------
void ofApp::update(){
    updateOSC();
    if (DEV_MODE){
        handlePythonMessages();
    }

    //Log incoming and outgoing messages
    uint64_t currentTime = ofGetElapsedTimeMillis();
    if ((currentTime - search_timer)>1000)
    {
        ofLogNotice()<<"Received " <<incomingControllerMessageCounter<< " OSC messages from controller in the last second"<<endl;
        ofLogNotice()<<"Received " <<incomingPlayerMessageCounter<< " OSC messages from player in the last second"<<endl;
        coms.logMessageCount();
        incomingControllerMessageCounter =0;
        incomingPlayerMessageCounter =0;
    }

    pointCloudRender.update();
    pointCloudRender.meshFromConnections(fKNN.getConnectionsForFeature(lastActiveFeatureIndex));

    if ((currentTime - search_timer)>100)
    {
//        pointCloudRender.updatePointPositions(fKNN.getPointFeatureDistances(targetFeatureValues, featureWeights), featureWeights);
        fKNN.getKNN(targetFeatureValues, featureWeights);


        search_timer =currentTime;
        videoIndexes = fKNN.getSearchResultIndexes();
        pointCloudRender.setActiveNodes(videoIndexes);
        //DEBUG MODE
//        fKNN.setPlayingIndex(videoIndexes[0]);
//        pointCloudRender.playingIndex = videoIndexes[0];

        std::ostringstream oss;
        if (!videoIndexes.empty())
        {
            // Convert all but the last element to avoid a trailing ","
            std::copy(videoIndexes.begin(), videoIndexes.end()-1,
                      std::ostream_iterator<int>(oss, ","));

            // Now add the last element with no delimiter
            oss << videoIndexes.back();
        }

        //std::cout << oss.str() << std::endl;
        vector<string> videoNames = databaseLoader.getVideoNamesFromIndexes(fKNN.getSearchResultIndexes());
        if (input_activity){
            if (currentPlayingVideo != videoNames[0]){
                coms.publishVideoNow( videoNames[0], true);
                currentPlayingVideo = videoNames[0];
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
        if (inactiveCounter[i]>featureTimeout && featureWeights[i] >0){
            //            if ((inactiveCounter[i] %30) ==0){
            featureWeights[i] = CLAMP(featureWeights[i] - featureDecayRate, 0., 1.);
            desireChanged=true;
//            featureActive[i]=false;
            //            }
        }

        else if (inactiveCounter[i]>featureTimeout && featureWeights[i] ==0.){
            featureActive[i]=false;
            continue;
        }

    }

    //If no activity go to low volume low playback speed
    float totalWeight = std::accumulate(featureWeights.begin(), featureWeights.end(), 0.);

    if (!input_activity && totalWeight <1.){
        //Decrement amplitude if it's not already at 0
        if (targetFeatureValues[0] != 0.){
            targetFeatureValues[0] = CLAMP(targetFeatureValues[0] - 0.01, 0., 1.);
        }
        totalWeight = std::accumulate(featureWeights.begin()+1, featureWeights.end(), 0.);
        featureWeights[0] = 1-totalWeight;


        //Decrement speed while inactive

//        if (speedSetting >0 && inactiveCounter[0] > 300 &&inactiveCounter[0] %60 ==0){
//            setSpeed(speedSetting -1);
//        }
    }

    //Do coms stuff
    if (speedChanged){
        coms.publishSpeed(speedSetting);
        speedChanged = false;
        int speed = SPEEDS[speedSetting];
        if (speed == -1) speed = playingFileDuration;

        featureGuiElements[0]->setValue(1000./60./speed);

    }

    for (int i = 0; i <featureWeights.size(); i++)
    {
        if (lastFeatureWeights[i]!=featureWeights[i]){
            coms.sendLightControl(i+3, int(featureWeights[i]*4096));
        }
    }

    //Update gui elements
    featureGuiElements[0]->update(); //Update speed timer
    featureGuiElements[1]->update(); //Update search radius


    for (int i = 0; i <featureWeights.size(); i++){

        //Do some interpolation on the value
        float diff = featureValues[i] - lastFeatureValues[i];
        float inc = sgn(diff)*diff*diff;
        lastFeatureValues[i]+=inc;

        featureGuiElements[i+2]->setWeight(featureWeights[i]);
        featureGuiElements[i+2]->setValue(lastFeatureValues[i]);
        featureGuiElements[i+2]->setTarget(targetFeatureValues[i]);
        featureGuiElements[i+2]->setActive(featureActive[i]);

    }


    //Upekep for next loop
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

    waveform.draw();

    //colourInspector.draw();
    //pointCloudTree->draw();
//    worldMap.draw();
    pointCloudRender.draw();
    imageManager->draw();

    //drawColors();

    drawControls(windowWidth, windowHeight);


}

//Draw control gui elements
void ofApp::drawControls(int windowWidth, int windowHeight){

    //Draw feature controls
    for (int i=0; i< featureGuiElements.size(); i++){
        featureGuiElements[i]->draw();
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

void ofApp::toggleLanguage(){
    languageIsEnglish = !languageIsEnglish;
    if (languageIsEnglish){
        featureNames = featureNamesEn;
    }
    else featureNames = featureNamesFr;
    setLayout();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key =='l'){
        toggleLanguage();
    }
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
    setLayout();

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

void ofApp::updatePlayingVideo(string video){
    currentPlayingVideo = video;
    imageManager->loadImages(currentPlayingVideo);
    lastFeatureValues = featureValues;
    featureValues = databaseLoader.getFeaturesFromName(currentPlayingVideo);
    for (int i =0; i <featureValues.size(); i++){
        if (!featureActive[i]){
            targetFeatureValues[i] = featureValues[i];
        }
    }
    fKNN.setPlayingIndex(databaseLoader.getVideoIndexFromName(currentPlayingVideo));
    pointCloudRender.setPlayingNode(databaseLoader.getVideoIndexFromName(currentPlayingVideo));

}

void ofApp::incrementSpeed(int step){
    speedSetting=  CLAMP(speedSetting+step, 0, 8);
    speedChanged =true;
}

void ofApp::incrementSearchRadius(int step){
    float step_norm = float(step)/20.;
    float search_radius=  CLAMP(this->fKNN.threshold_distance+step, 0., 1.);
    this->fKNN.updateSearchRadius(search_radius);
        this->featureGuiElements[1]->setValue(search_radius);
}

void ofApp::setSpeed(int value){
    speedSetting=  CLAMP(value, 0, 8);
    speedChanged =true;
}

void ofApp::incrementFeatureTarget(int index, float step){

    targetFeatureValues[index] =CLAMP(targetFeatureValues[index]+step, 0., 1.);

    updateActiveFeature(index);

}

void ofApp::toggleFeatureTarget(int index){
    float v = targetFeatureValues[index];
    int target_value = 0.;
    if (v<.5) target_value = 1.;
    targetFeatureValues[index] =CLAMP(target_value, 0., 1.);

    updateActiveFeature(index);
}

void ofApp::updateActiveFeature(int index){
    lastActiveFeatureIndex =index;
    featureActive[index]=true;
    featureWeights[index]=1.0;
    inactiveCounter[index] = 0;
    desireChanged=true;
    activityTimer = 0;
    input_activity = true;
    for(int i=0; i<this->inactiveCounter.size(); i++){
        if (i ==index )continue;
        inactiveCounter[i] = featureTimeout;
    }
}


void ofApp::updateOSC() {
    // hide old messages
    while( coms.receiver_playing.hasWaitingMessages() )
    {
        incomingPlayerMessageCounter++;
        ofxOscMessage m;
        coms.receiver_playing.getNextMessage( m );
        if ( m.getAddress().compare( string("/PLAYING_VIDEO") ) == 0 )
        {
            updatePlayingVideo(m.getArgAsString(0));
            playingFileDuration = m.getArgAsInt64(1);

            if (this->speedSetting ==0){
               featureGuiElements[0]->setValue(1000./60./playingFileDuration);
            }
            featureGuiElements[0]->reset();

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
        incomingControllerMessageCounter++;
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

void ofApp::handlePythonMessages(){
    // check for waiting messages
    while( coms.receiver_python_controller.hasWaitingMessages() )
    {
        // get the next message
        ofxOscMessage m;
        coms.receiver_python_controller.getNextMessage(m );

        // check for mouse button message
        if ( m.getAddress().compare( string("/FEATURE_DIFFS") ) == 0 )
        {
            // the single argument is a string
            for (int i =0; i<23;i++){
                float diff =  m.getArgAsFloat(i);

                if (abs(diff) >0){
                    if (i ==0){
                        incrementSpeed(diff);
                    }
                    if (i ==1){
                        incrementSearchRadius(diff*100);
                    }

                    if (i> 1){
                        targetFeatureValues[i-2] =CLAMP(targetFeatureValues[i-2] +diff, 0., 1.);
                        inactiveCounter[i-2] = 0;
                        featureWeights[i-2]=1.0;

                        desireChanged=true;
                        activityTimer = 0;

                        input_activity = true;
                    }

                }
            }

            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "FEATURE_DIFFS Message received : " ;
        }

        if ( m.getAddress().compare( string("/SET_SPEED") ) == 0 )
        {

            speedSetting=  m.getArgAsInt(0);
            speedChanged =true;
        }

        else if ( m.getAddress().compare( string("/FEATURE_VALUES") ) == 0 )
        {
            // the single argument is a string

            for (int i =0; i<24;i++){
                featureValues[i] = m.getArgAsFloat(i);
            }
            string fileName = m.getArgAsString(24);
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Message received : " ;
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;


        }

        else if ( m.getAddress().compare( string("/FEATURE_NAMES") ) == 0 )
        {
            // the single argument is a string
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

    if (i ==2){
        int step =m.getArgAsInt(1);
        if (step ==0) step = -1;
        incrementSearchRadius(step);
        return;
    }


    i =i-3;

    float step = (float(m.getArgAsInt(1)) -0.5)*2/50.;
    incrementFeatureTarget(i, step);
}



void ofApp::handleButtonInput(int index){
    //    ofLogDebug(ofToString(ofGetElapsedTimef(),3)) << " Step Message received : " ;

    if (index ==1){
        setSpeed(0);
        return;
    }

    if (index ==2){
//        setSpeed(0);
//        return;
        //Set number of neighbours to something else
    }

    index =index-3;
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

