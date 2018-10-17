#include "featurecontrol.h"
#include "ofxJsonSettings.h"

template <typename E>
constexpr typename std::underlying_type<E>::type to_ut(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

FeatureControl::FeatureControl(DatabaseLoader *dbl, CommunicationManager  *coms,vector<unique_ptr<CircleFeatureGuiElement>> *guiElements, PointCloudRenderer* pcr)
{
    this->dbl = dbl;
    this->coms = coms;
    this->fge = guiElements;
    this->pcr = pcr;
    this->fKNN = new FeatureKNN(dbl);
    Settings::get().load("settings.json");
    idleTimeout =  Settings::getInt("idle_timeout"); //seconds
    idleMinVideos=  Settings::getInt("idle_videos"); //seconds

    featureDecayRate = Settings::getFloat("feature_decay_sec")/60.;
    idleActivityInterval = Settings::getFloat("idle_activity_min_interval"); //seconds
    idleActivityTransitionDuration= Settings::getFloat("idle_activity_transition_duration"); //seconds

    idleActivityNumUpdates= Settings::getInt("idle_activity_num_updates"); //seconds

    state = IDLE;
    int num_features =21;

    //Initialize weight to zero
    featureValues.resize(num_features);
    lastFeatureValues.resize(num_features);
    featureWeights = vector<float>(num_features, 0.);
    inactiveCounter.resize(num_features,0);
    targetFeatureValues.resize(num_features, 0.);
    featureActive.resize(num_features, false);
    lastFeatureWeights.resize(num_features, 0.);
    videoCycleIndex = 0;
    blink =false;

    //Make loudness the only active feature
    featureWeights[0]= 1.;
    playingVideo = pair<string, int>("Empty", -1);
    videoMaxIndex = 1;
    activityType = ActivityTypes::NONE;
    activeFeatureIndex=0;
    idle_active_video_count =0;
    input_activity_flag = 0;
    blinkTimer =0;
    setSpeed(0);
    toIdle();

}

void FeatureControl::update(){
    if (input_activity_flag){
        lastActivityTime = ofGetElapsedTimef();
    }

    currentTime = ofGetElapsedTimef();
    idleTimeCounter = currentTime - lastActivityTime;

    if ((currentTime -videoStartTime) > videoCycleTimer){
        cycleVideo();
    }

    if ((currentTime - blinkTimer) > 0.02 && blink){
        blinkOff();
    }

    updateState();

    switch (state){
    case HUMAN_ACTIVE:
        //Todo
        updateFeatureWeights(false);
        break;

    case IDLE:
        updateFeatureWeights(true);
        if (weightsChanged()){
            getNewVideos(false);
            updateLights();
        }
        break;

    case IDLE_ACTIVE:
        updateFeatureWeights(true);
        float idleTime = currentTime -idleActivatedTime;
        if (idle_active_state == IDLE_ACTIVE_TRANSITION){
            lastActiveCycle = currentTime;
//            (*fge)[0]->setValue(idleActivityTransitionDuration/idleActivityNumUpdates);

            if ((currentTime - idleActivatedTime) > idleActivityTimings[videoCycleIndex]){
                idle_active_video_count++;
                cycleVideo();
            }

            if (videoCycleIndex >= (idleActivityNumUpdates-1)){
                idle_active_state = IDLE_ACTIVE_STABLE;
                setSpeed(0);
            }

        }
        else if ( idle_active_state == IDLE_ACTIVE_STABLE){
            float vidlen = dbl->getVideoLength(playingVideo.second);
            (*fge)[0]->setValue(vidlen-1);
            if ((currentTime - lastActiveCycle)>= vidlen -1.1){
                toIdle();
            }
        }
        break;
    }
    input_activity_flag = false;
    lastFeatureWeights =featureWeights;
}


void FeatureControl::updateState(){
    switch (state){
    case HUMAN_ACTIVE:
        if (idleTimeCounter> idleTimeout){
            toIdle();
            break;
        }
        break;
    case IDLE:
        if (input_activity_flag){
            toHumanActive();
            break;
        }
        if (idleTimeCounter > idleActivityInterval && playedIdleVideos >= idleMinVideos){
            toIdleActive();
            break;
        }
        break;
    case IDLE_ACTIVE:
        if (input_activity_flag){
            activityType = ActivityTypes::NONE;
//            getNewVideos();
            toHumanActive();
            break;
        }
        break;
    }
}

void FeatureControl::toIdle(){
    idleMinVideos=  Settings::getInt("idle_videos"); //number of videos to play in idle mode
    playedIdleVideos =0;
    lastActivityTime = ofGetElapsedTimef();
    slowdownTime = ofGetElapsedTimef();
    setIdleFeature(idleFeatureIndexes[rand()%numIdleFeatures]);
    state= IDLE;
}

void FeatureControl::toIdleActive(){
    state = IDLE_ACTIVE;
    idle_active_state = IDLE_ACTIVE_TRANSITION;
    idle_active_video_count = 0;
    setSpeed(9);
    activityType = ActivityTypes(rand()% (to_ut(ActivityTypes::last)) +1);
    idleFeatureIndex =idleActiveFeatureIndexes[rand()%numIdleActiveFeatures];
    targetFeatureValues[idleFeatureIndex] = 0.;
    updateActiveFeature(idleFeatureIndex, 0., false);

    idleActivityValues.clear();
    idleActivityTimings.clear();
    int half_point = idleActivityNumUpdates/2;

    for (int step =0; step < idleActivityNumUpdates; step++){
        idleActivityTimings.push_back(step*idleActivityTransitionDuration/idleActivityNumUpdates);
        switch (activityType){
        case ActivityTypes::ASCENDING:
            idleActivityValues.push_back(step*1./(idleActivityNumUpdates-1));
            break;
//        case ActivityTypes::DESCENDING:
//            idleActivityValues.push_back(1. -float(step) / (idleActivityNumUpdates-1));
//            break;
        case ActivityTypes::UP_DOWN:
            //0 to 1
            if (step <half_point){
                idleActivityValues.push_back(step*1./(half_point-1));
            }
            //1 to 0
            else{
                idleActivityValues.push_back(1 - (step-half_point) *1./(half_point-1));
            }
            break;
//        case ActivityTypes::DOWN_UP:
//            //1 to 0
//            if (step <half_point){
//                idleActivityValues.push_back(1. -float(step)/(half_point-1));
//            }
//            //0 to 1
//            else{
//                idleActivityValues.push_back(step*1./(half_point));
//            }
//            break;
        }
    }
    lastActivityTime = ofGetElapsedTimef();
    idleActivatedTime = ofGetElapsedTimef();

    //Create a set of videos along the feature value trajectory
    videoIndexes.clear();
    videoCycleIndex =0;

    vector<float> tempSearchvalues = targetFeatureValues;
    vector<float> tempFeatureWeights;
    tempFeatureWeights.resize(tempSearchvalues.size(), 0);
    tempFeatureWeights[idleFeatureIndex ]=1.;

    for (auto value: idleActivityValues){
        tempSearchvalues[idleFeatureIndex] = value;
        fKNN->getKNN(tempSearchvalues, tempFeatureWeights);
        videoIndexes.push_back( fKNN->getSearchResultsDistance(1,true, numVideosInRange)[0]);
    }

    videos =  dbl->getVideoPairsFromIndexes(videoIndexes);
    videoMaxIndex = videos.size();
    (*fge)[1]->setValue(videoMaxIndex);
}

void FeatureControl::toHumanActive(){
    state = HUMAN_ACTIVE;
    lastActivityTime = ofGetElapsedTimef();
}

void FeatureControl::setIdleFeature(int index){
    idleFeatureIndex = index;
    targetFeatureValues[idleFeatureIndex] = 0.;
    updateActiveFeature(idleFeatureIndex, 1.);
}

void FeatureControl::getNewVideos(bool play){

    fKNN->getKNN(targetFeatureValues, featureWeights);
    if (state ==IDLE || state ==IDLE_ACTIVE){
        videoIndexes = fKNN->getSearchResultsDistance(3,true, numVideosInRange);
        videoMaxIndex = videoIndexes.size();
        (*fge)[1]->setValue(videoMaxIndex);
    }

    else if (state ==HUMAN_ACTIVE){
//        videoMaxIndex = min(videoMaxIndex, 32);
//        videoIndexes = fKNN->getSearchResultsDistance(1,true, numVideosInRange);
//        videoMaxIndex = videoIndexes.size();
        (*fge)[1]->setValue(videoMaxIndex);
        videoIndexes = fKNN->getSearchResultsDistance(32, true, numVideosInRange);
    }

    videos = dbl->getVideoPairsFromIndexes(videoIndexes);

    if (playingVideo.second != videos[0].second && play){
        videoCycleIndex =0;
        playingVideo =videos[0];
        playVideo();
    }
}

void FeatureControl::playVideo(){
    //Todo: Let main loop know
    videoLength = dbl->getVideoLength(playingVideo.second);

    if (speedSetting == 0){
        videoCycleTimer = videoLength-1.;
    }

    (*fge)[0]->reset();
    (*fge)[0]->setValue(videoCycleTimer);
    (*fge)[COLOR_ELEMENT_INDEX]->setWeightColor(dbl->colors[playingVideo.second]);
    videoStartTime = ofGetElapsedTimef();
    coms->publishVideoNow( playingVideo.first, true);
    updateFeatureValues(dbl->getFeaturesFromindex(playingVideo.second));
    pcr->updateLine(playingVideo.second);
    pcr->updateLine(videos[(videoCycleIndex+1)%videoMaxIndex].second);

}

void FeatureControl::cycleVideo(){
    videoCycleIndex = (videoCycleIndex+1)%videoMaxIndex;
    playingVideo = videos[videoCycleIndex];
    playedIdleVideos ++;
    if (state ==IDLE_ACTIVE){
        targetFeatureValues[idleFeatureIndex]= idleActivityValues[videoCycleIndex];
    }
    if (shouldSlowdown()){
        incrementSpeed(-1);
        idleMinVideos+=1;
    }
    playVideo();
}

void FeatureControl::playRandomVideo(){
    playingVideo= this->dbl->getRandomVideo();
    playVideo();
}

void FeatureControl::updateFeatureWeights(bool ignoreIdle){
    for (int i = 0; i <featureWeights.size(); i++){
        if (ignoreIdle){
            if (i ==idleFeatureIndex){
                continue;
            }
        }

        if (featureWeights[i] >0.){
            inactiveCounter[i]+=1;
        }

        //After [idleTimeout] seconds of inactivity decrement the weight by 0.1 every 0.5 seconds
        if (inactiveCounter[i]>idleTimeout*secondsToFrames && featureWeights[i] >0){
            featureWeights[i] = CLAMP(featureWeights[i] - featureDecayRate, 0., 1.);
        }

        else if (inactiveCounter[i]>idleTimeout*secondsToFrames && featureWeights[i] ==0.){
            featureActive[i]=false;
        }
    }

}

void FeatureControl::draw(){
    ostringstream oss;
    oss.str("");
    oss << "State" <<endl;
    oss << stateStrings[state]<<endl;
    oss << activityTypeStrings[to_ut(activityType)]<<endl;
    ofDrawBitmapString(oss.str(), 0,0);

    oss.str("");
    oss << "" <<endl;
    oss << "Idle timer"<<endl;
    oss << "Last Act."<<endl;
    oss << "Act. index" <<endl;
    oss << "Idle Index" <<endl;
    oss << "Video name"<<endl;
    oss << "Video index" <<endl;
    oss << "Video time" <<endl;
    oss << "Video length" <<endl;
    oss << "Cycle index"<<endl;
    oss << "Cycle time" <<endl;
    oss << "Idle played" <<endl;
    oss << "Idle min vid" <<endl;
    oss << "Videos found" <<endl;


    ofDrawBitmapString(oss.str(), 100,0);

    oss.str("");
    oss << "Tnfo" <<endl;
    oss << idleTimeCounter <<endl;
    oss << lastActivityTime<<endl;
    oss << activeFeatureIndex <<endl;
    oss << idleFeatureIndex <<endl;
    oss << playingVideo.first <<endl;
    oss << playingVideo.second <<endl;
    oss << currentTime -videoStartTime <<endl;
    oss << videoLength <<endl;
    oss << videoCycleIndex <<endl;
    oss << videoCycleTimer <<endl;

    oss << playedIdleVideos <<endl;
    oss << idleMinVideos <<endl;
    oss<< numVideosInRange<<endl;

    ofDrawBitmapString(oss.str(), 200,0);

    oss.str("");
    oss << "Target" <<endl;
    for (auto featureTarget : targetFeatureValues){
        oss << setprecision(2) << featureTarget <<endl;
    }
    ofDrawBitmapString(oss.str(), 350,0);

    oss.str("");
    oss << "Value" <<endl;
    for (auto featureV: featureValues){
        oss << setprecision(2) << featureV <<endl;
    }
    ofDrawBitmapString(oss.str(), 400,0);

    oss.str("");
    oss << "Weight" <<endl;
    for (auto featureW: featureWeights){
        oss << setprecision(2) <<featureW <<endl;
    }
    ofDrawBitmapString(oss.str(), 450,0);

    oss.str("");
    oss << "Active" <<endl;
    for (auto featureA: featureActive){
        oss << featureA <<endl;
    }
    ofDrawBitmapString(oss.str(), 500,0);
}

void FeatureControl::updateFeatureValues(vector<float> fv){
    lastFeatureValues = featureValues;
    featureValues = fv;
    for (int i =0; i <featureValues.size(); i++){
        if (!featureActive[i]){
            targetFeatureValues[i] = featureValues[i];
        }
    }
}

void FeatureControl::incrementFeatureTarget(int index, float step){
    if (index ==COLOR_FEATURE_INDEX) {
        float t;
        targetFeatureValues[index] = modf(targetFeatureValues[index]+step, &t );
        if (targetFeatureValues[index] <0.){
            targetFeatureValues[index] = 1+targetFeatureValues[index];
        }
    }
    if (index ==TILT_FEATURE_INDEX) {
        targetFeatureValues[index] = CLAMP(targetFeatureValues[index]+step, -0.5, 0.5);
    }
    else{
        targetFeatureValues[index] =CLAMP(targetFeatureValues[index]+step, 0., 1.);

    }
    updateActiveFeature(index, 1);
}

void FeatureControl::toggleFeatureTarget(int index){
    float v = targetFeatureValues[index];

    if (index ==COLOR_FEATURE_INDEX){
        float t;
        float target_value = modf(v +0.5, &t);
        targetFeatureValues[index] =target_value;
    }
    else{
        int target_value = 0.;
        if (v<.5) target_value = 1.;
        targetFeatureValues[index] =CLAMP(target_value, 0., 1.);
    }

    updateActiveFeature(index, 1);
}

void FeatureControl::updateActiveFeature(int index, int timeoutOffset, bool trigger){
    featureActive[index]=true;
    featureWeights[index]=1.0;
    inactiveCounter[index] = 0;

    //Set light to max
    coms->sendLightControl(index+3, 4096);

    //Timeout the other features
    for(int i=0; i<this->inactiveCounter.size(); i++){
        if (i ==index )continue;
        //Set other active feautres to timeout limit - offset
        else if (featureActive[index]){
            int thresh = idleTimeout*secondsToFrames - timeoutOffset*secondsToFrames;
            if (inactiveCounter[i] < thresh){
                inactiveCounter[i] = thresh;
            }
        }
    }
    if (trigger){
        getNewVideos();
    }
}


void FeatureControl::registerInputActivity(){
    input_activity_flag = true;
}

bool FeatureControl::shouldSlowdown(){
    if (state ==IDLE){
        if (speedSetting >0){
            return true;
        }
    }
    return false;
}

int FeatureControl::incrementSearchRadius(int step){
    int t =  CLAMP(videoMaxIndex+step, 1, 32);
    if (t > videos.size()){
        return videoMaxIndex;
    }
    videoMaxIndex = t;
    (*fge)[1]->setValue(videoMaxIndex);
    return videoMaxIndex;
}

void FeatureControl::incrementSpeed(int step){
    setSpeed( CLAMP(speedSetting+step, 0, 14));
}

void FeatureControl::setSpeed(int value){
    speedSetting = value;
    videoCycleTimer = SPEEDS[speedSetting];
    videoLength = dbl->getVideoLength(playingVideo.second);
    if (speedSetting == 0){
        videoCycleTimer = videoLength-1.;
    }

    (*fge)[0]->setValue(videoCycleTimer);
}


void FeatureControl::updateLights(){
    for (int index = 0 ; index < featureValues.size(); index ++){
        int lightValue = 0;
        lightValue += 4096*0.25 * featureValues[index];
        lightValue += 4096*0.75 * featureActive[index];
        coms->sendLightControl(index +3, lightValue);
    }
}


void FeatureControl::blinkOn(){
    coms->sendLightControl(0, 4096);
    blink = true;
    blinkTimer= currentTime;

}

void FeatureControl::blinkOff(){
    coms->sendLightControl(0, 0);
    blink = false;
}
