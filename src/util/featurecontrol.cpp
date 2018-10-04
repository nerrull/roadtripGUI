#include "featurecontrol.h"
#include "ofxJsonSettings.h"

template <typename E>
constexpr typename std::underlying_type<E>::type to_ut(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

FeatureControl::FeatureControl()
{
    Settings::get().load("settings.json");
    idleTimeout =  Settings::getInt("idle_timeout"); //seconds
    featureDecayRate = Settings::getFloat("feature_decay_sec")/60.;
    idleActivityInterval = Settings::getFloat("idle_activity_interval"); //seconds
    idleActivityDuration= Settings::getFloat("idle_activity_duration"); //seconds
    idleActivityNumUpdates= Settings::getInt("idle_activity_num_updates"); //seconds

    state = IDLE;
    update_video_flag = false;
    int num_features =21;

    //Initialize weight to zero
    featureValues.resize(num_features);
    lastFeatureValues.resize(num_features);
    featureWeights = vector<float>(num_features, 0.);
    inactiveCounter.resize(num_features,0);
    targetFeatureValues.resize(num_features, 0.);
    featureActive.resize(num_features, false);
    lastFeatureWeights.resize(num_features, 0.);

    //Make loudness the only active feature
    featureWeights[0]= 1.;
}

void FeatureControl::initialize(DatabaseLoader *dbl){
    fKNN.init(dbl);

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
        if (idleTimeCounter > idleActivityInterval){
            toIdleActive();
            break;
        }
        break;
    case IDLE_ACTIVE:
        if (input_activity_flag){
            activityType = ActivityTypes::NONE;
            toHumanActive();
            break;
        }
        else if ((currentTime - idleActivatedTime) >idleActivityDuration){
            activityType = ActivityTypes::NONE;
            toIdle();
        }
        break;
    }
}

void FeatureControl::toIdle(){
    lastActivityTime = ofGetElapsedTimef();
    slowdownTime = ofGetElapsedTimef();
    setIdleFeature(idleFeatureIndexes[rand()%numIdleFeatures]);
    state= IDLE;
}

void FeatureControl::toIdleActive(){
    state = IDLE_ACTIVE;
    activityType = ActivityTypes(rand()% (to_ut(ActivityTypes::last)-1) +1);
    setIdleFeature(idleActiveFeatureIndexes[rand()%numIdleActiveFeatures]);

    idleActivityValues.clear();
    idleActivityTimings.clear();
    int half_point = idleActivityNumUpdates/2;

    for (int step =0; step < idleActivityNumUpdates; step++){
        idleActivityTimings.push_back(step*idleActivityDuration/idleActivityNumUpdates);
        switch (activityType){
        case ActivityTypes::ASCENDING:
            idleActivityValues.push_back(step*1./idleActivityNumUpdates);
            break;
        case ActivityTypes::DESCENDING:
            idleActivityValues.push_back(1. -float(step) / idleActivityNumUpdates );
            break;
        case ActivityTypes::UP_DOWN:
            //0 to 1
            if (step <half_point){
                idleActivityValues.push_back(step*1./half_point);
            }
            //1 to 0
            else{
                idleActivityValues.push_back(2. -float(step)/half_point);
            }
            break;
        case ActivityTypes::DOWN_UP:
            //1 to 0
            if (step <half_point){
                idleActivityValues.push_back(1. -float(step)/half_point);
            }
            //0 to 1
            else{
                idleActivityValues.push_back(step*1./half_point);
            }
            break;
        }
    }
    lastActivityTime = ofGetElapsedTimef();
    idleActivatedTime = ofGetElapsedTimef();
}

void FeatureControl::toHumanActive(){
    state = HUMAN_ACTIVE;
    lastActivityTime = ofGetElapsedTimef();
}

void FeatureControl::setIdleFeature(int index){
    idleFeatureIndex = index;
    for (int i = 0; i <featureWeights.size(); i++)
    {
        if (i!= idleFeatureIndex){
            inactiveCounter[i] = idleTimeout*secondsToFrames;
        }
    }
    featureWeights[idleFeatureIndex] = 1.;
    updateActiveFeature(idleFeatureIndex);
}

void FeatureControl::getNewVideos(){
    fKNN.getKNN(targetFeatureValues, featureWeights);
    videoIndexes = fKNN.getSearchResultIndexes();
    update_video_flag = false;
}

void FeatureControl::update(){
    if (input_activity_flag){
        lastActivityTime = ofGetElapsedTimef();
    }
    if (update_video_flag){
        getNewVideos();
    }

    currentTime = ofGetElapsedTimef();
    idleTimeCounter = currentTime - lastActivityTime;

    updateState();
    input_activity_flag = false;
    lastFeatureWeights =featureWeights;

    switch (state){
    case HUMAN_ACTIVE:
        //Todo
        updateFeatureTimers(false);
        break;

    case IDLE:
        updateFeatureTimers(true);
        break;

    case IDLE_ACTIVE:
        float idleTime = currentTime -idleActivatedTime;
        if (idleActivityTimings.size() >0){
            if (idleTime > idleActivityTimings[0]){
                idleActivityTimings.pop_front();
                targetFeatureValues[idleFeatureIndex]= idleActivityValues.front();
                idleActivityValues.pop_front();
            }
            update_video_flag = true;
        }
        updateFeatureTimers(true);

        break;
    }
}

void FeatureControl::updateFeatureTimers(bool ignoreIdle){
    for (int i = 0; i <featureWeights.size(); i++){
        if (ignoreIdle){
            if (i ==idleFeatureIndex){
                continue;
            }
        }
        if (featureWeights[i] >0.){
            inactiveCounter[i]+=1;
        }

        //After 5 seconds of inactivity decrement the weight by 0.1 every 0.5 seconds
        if (inactiveCounter[i]>idleTimeout*secondsToFrames && featureWeights[i] >0){
            featureWeights[i] = CLAMP(featureWeights[i] - featureDecayRate, 0., 1.);
        }

        else if (inactiveCounter[i]>idleTimeout*secondsToFrames && featureWeights[i] ==0.){
            featureActive[i]=false;
            continue;
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
    oss << "Last Activity"<<endl;
    oss << "Act. index" <<endl;
    oss << "Idle Index" <<endl;
    ofDrawBitmapString(oss.str(), 100,0);

    oss.str("");
    oss << "Timings" <<endl;
    oss << idleTimeCounter <<endl;
    oss << lastActivityTime<<endl;
    oss << activeFeatureIndex <<endl;
    oss << idleFeatureIndex <<endl;
    ofDrawBitmapString(oss.str(), 250,0);

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
    updateActiveFeature(index);
    targetFeatureValues[index] =CLAMP(targetFeatureValues[index]+step, 0., 1.);
}

void FeatureControl::toggleFeatureTarget(int index){
    updateActiveFeature(index);
    float v = targetFeatureValues[index];
    int target_value = 0.;
    if (v<.5) target_value = 1.;
    targetFeatureValues[index] =CLAMP(target_value, 0., 1.);
}

void FeatureControl::updateActiveFeature(int index){

    featureActive[index]=true;
    featureWeights[index]=1.0;
    inactiveCounter[index] = 0;
    update_video_flag =true;

    // If we've already activated the index we exit here
    if (activeFeatureIndex == index){
        return;
    }

    //Timeout the other features
    for(int i=0; i<this->inactiveCounter.size(); i++){
        if (i ==index )continue;
        inactiveCounter[i] = idleTimeout*secondsToFrames;
    }
}

void FeatureControl::registerInputActivity(){
    input_activity_flag = true;
}

bool FeatureControl::shouldSlowdown(int speedSetting){
    if (state ==IDLE){
        if (speedSetting >0){
            if (currentTime -slowdownTime >0.5){
                slowdownTime = currentTime;
                return true;
            }
        }
    }
    return false;
}

int FeatureControl::incrementSearchRadius(int step){
    int nv=  CLAMP(this->fKNN.numVideos+step, 1, 50);
    this->fKNN.setNumVideos(nv);
    return nv;
}
