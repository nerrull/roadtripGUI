#include "ofApp.h"
#include "ofxJsonSettings.h"


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


//--------------------------------------------------------------
void ofApp::setup(){
    ofGetWindowPtr()->setVerticalSync(true);
    ofSetVerticalSync(true);
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    ofSetCircleResolution(100);
    glPointSize(2.0);

    Settings::get().load("settings.json");
    string db_path = Settings::getString("db_path");
    DEV_MODE = Settings::getBool("dev_mode");

    if (DEV_MODE) ofSetFrameRate(60);

    font.load("futura.ttf", 12);
    initAudio();
    initNames();
    //    ofSetLogLevel(OF_LOG_ERROR);
    ofSetLogLevel(OF_LOG_NOTICE);

    numKnobs =23;
    search_timer =ofGetElapsedTimeMillis();

    databaseLoader.loadHDF5Data(db_path);
    fKNN.init(&databaseLoader);
    if (Settings::getInt("point_mode") ==0){
        pointCloudRender.setRotation(false);
        pointCloudRender.initPoints(databaseLoader.dimension_reduction_2D, databaseLoader.colors);
    }

    else if (Settings::getInt("point_mode") ==1){
        pointCloudRender.setRotation(true);
        pointCloudRender.initPoints(databaseLoader.dimension_reduction, databaseLoader.colors);
    }

    //Initialize feature gui elements
    int num_features =21;    
    featureGuiElements.resize(num_features+2);
    featureGuiElements[0] = make_unique<FillCircle>();
    featureGuiElements[1] = make_unique<SearchRadiusElement>();
    for (int i =2; i<featureGuiElements.size(); i++){
        featureGuiElements[i]= make_unique<CircleFeatureGuiElement>();
    }
    //Init custom gui elements (hue)
    featureGuiElements[11] = make_unique<ColorCircle>();
    dynamic_cast<ColorCircle*>(featureGuiElements[11].get())->setColorLimits(databaseLoader.color_min_max.first, databaseLoader.color_min_max.second);

    speedSetting =0;
    currentPlayingVideo ="";

    for(int i =0; i < num_features;i++){
        coms.sendLightControl(i+3, 0);
    }
    coms.sendLightControl(3,4095);

    ofBackground(0);
    setLayout();
}

void ofApp::initNames(){
    featureNamesEn.push_back("Loudness");
    featureNamesEn.push_back("Spectral\nCentroid");
    featureNamesEn.push_back("Percussiveness");
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
    featureNamesFr.push_back("Largeur\nde bande");
    featureNamesFr.push_back("Vitesse");
    featureNamesFr.push_back("Instabilité");
    featureNamesFr.push_back("Régime Moteur");
    featureNamesFr.push_back("T° extérieur");
    featureNamesFr.push_back("Inclinaison");
    featureNamesFr.push_back("Teinte");
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

    imageManager.setLayout(div*windowWidth, 0, rem*windowWidth, 3*windowHeight/4);
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
            featureGuiElements[i]->setTextOnTop(false);
            featureGuiElements[i]->setCircleOffset(firstRowOffset);
        }
        else{
            y +=secondRowOffset;
            featureGuiElements[i]->setTextOnTop(false);
            featureGuiElements[i]->setCircleOffset(firstRowOffset);
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
    fc.update();

    //Every 100 ms
    if ((currentTime - search_timer)>100)
    {
        fKNN.getKNN(fc.getTargetValues(), fc.getWeights());
        search_timer =currentTime;
        videoIndexes = fKNN.getSearchResultIndexes();
        pointCloudRender.setActiveNodes(videoIndexes);

        vector<string> videoNames = databaseLoader.getVideoNamesFromIndexes(videoIndexes);
        if (fc.state == FeatureControl::HUMAN_ACTIVE){
            if (currentPlayingVideo != videoNames[0]){
                coms.publishVideoNow( videoNames[0], true);
                currentPlayingVideo = videoNames[0];
                lastVideos.clear();
                lastVideos.push_back(videoNames[0]);
            }
        }

        else if (!vectorsAreEqual(videoNames, lastVideos) && videoNames.size()>0){
            coms.publishVideos(videoNames, true);
            lastVideos = videoNames;
        }
    }

    //Update active features

    fc.update();

    if (fc.shouldSlowdown(speedSetting) ){
        speedSetting -=1;
        speedChanged = true;
    }

    //Send messages to player if speed changed
    if (speedChanged){
        coms.publishSpeed(speedSetting);
        speedChanged = false;
        int speed = SPEEDS[speedSetting];
        if (speed == -1) speed = playingFileDuration;
        featureGuiElements[0]->setValue(float(1000./60./speed));
    }

    //Send message to lights if features change
    for (int i = 0; i <fc.numFeatures(); i++)
    {
        if (fc.weightChanged(i)){
            coms.sendLightControl(i+3, int(fc.getWeights()[i]*4096));
        }
    }

    //Update gui elements
    featureGuiElements[0]->update(); //Update speed timer
    featureGuiElements[1]->update(); //Update search radius

    for (int i = 0; i <fc.numFeatures(); i++){
        //Do some interpolation on the value
        float diff = fc.getValueDiff(i);
        float inc = sgn(diff)*diff*diff;
        fc.incrementLastFeatureValue(i,inc);

        featureGuiElements[i+2]->setWeight(fc.getWeight(i));
        featureGuiElements[i+2]->setValue(fc.getLastValue(i));
        featureGuiElements[i+2]->setTarget(fc.getTargetValue(i));
        featureGuiElements[i+2]->setActive(fc.getActive(i));
    }
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
    pointCloudRender.draw();
    imageManager.draw();

    drawControls(windowWidth, windowHeight);
    drawColors();
}

//Draw control gui elements
void ofApp::drawControls(int windowWidth, int windowHeight){

    //Draw feature controls
    for (int i=0; i< featureGuiElements.size(); i++){
        featureGuiElements[i]->draw();
    }
}

void ofApp::drawColors(){

    ofColor c = databaseLoader.colors[databaseLoader.getVideoIndexFromName(currentPlayingVideo)];

    int windowWidth = ofGetWidth();
    ofPushMatrix();
    ofTranslate(5*windowWidth/6, 20);
    int boxWidth = windowWidth/6;


    ofSetColor(c);
    ofDrawRectangle(0,0, boxWidth/2, boxWidth/2);
    ofSetColor(255);
    font.drawString("Palette",0,20);

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

    if (key =='r'){
        playRandomVideo();
    }
}


void ofApp::playRandomVideo(){

    updatePlayingVideo(databaseLoader.getRandomVideo());
    coms.publishVideoNow( currentPlayingVideo, true);


}

void ofApp::updatePlayingVideo(string video){
    currentPlayingVideo = video;
    int videoIndex = databaseLoader.getVideoIndexFromName(currentPlayingVideo);

    imageManager.loadImages(currentPlayingVideo);
    fc.updateFeatureValues( databaseLoader.getFeaturesFromName(currentPlayingVideo));
    fKNN.setPlayingIndex(videoIndex);
    pointCloudRender.setPlayingNode(videoIndex);
}

void ofApp::incrementSpeed(int step){
    speedSetting=  CLAMP(speedSetting-step, 0, 18);
    speedChanged =true;
}

void ofApp::incrementSearchRadius(int step){
    int nv=  CLAMP(this->fKNN.numVideos+step, 1, 50);
    this->fKNN.setNumVideos(nv);
    this->featureGuiElements[1]->setValue(nv);
}

void ofApp::setSpeed(int value){
    speedSetting=  CLAMP(value, 0, 8);
    speedChanged =true;
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
               featureGuiElements[0]->setValue(float(1000./60./playingFileDuration));
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
                    fc.registerInputActivity();

                    if (i ==0){
                        incrementSpeed(diff);
                    }
                    if (i ==1){
                        incrementSearchRadius(diff*100);
                    }

                    if (i> 1){
                        fc.incrementFeatureTarget( i-2,  diff);
                    }
                }
            }
            ofLogError(ofToString(ofGetElapsedTimef(),3)) << "FEATURE_DIFFS Message received" ;
        }
    }

}

void ofApp::handleKnobInput(ofxOscMessage m){
    //    ofLogDebug(ofToString(ofGetElapsedTimef(),3)) << " Step Message received : " ;
    int i = m.getArgAsInt(0);
    fc.registerInputActivity();

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
    fc.incrementFeatureTarget(i, step);
}

void ofApp::handleButtonInput(int index){
    //    ofLogDebug(ofToString(ofGetElapsedTimef(),3)) << " Step Message received : " ;
    fc.registerInputActivity();

    if (index ==1){
        setSpeed(0);
        return;
    }

    if (index ==2){
        //Todo: Set number of neighbours to something else
        return;
    }

    index =index-3;
    fc.toggleFeatureTarget(index);
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
    waveform.receiveBuffer(buffer);
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
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
