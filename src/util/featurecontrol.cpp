#include "featurecontrol.h"
#include "ofxJsonSettings.h"

FeatureControl::FeatureControl()
{
    Settings::get().load("settings.json");
    idleTimeout =  Settings::getInt("idle_timeout"); //seconds
    featureDecayRate = Settings::getFloat("feature_decay_sec")/60.;
    idleActivityInterval = Settings::getFloat("idle_activity_interval"); //seconds
    idleActivityDuration= Settings::getFloat("idle_activity_duration"); //seconds

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

    //Make loudness the only active feature
    featureWeights[0]= 1.;
}

void FeatureControl::updateState(){
    switch (state){
    case HUMAN_ACTIVE:
        if (idleTimeCounter> idleTimeout){
            toIdle();
            break;
        }
    case IDLE:
        if (input_activity_flag){
            toHumanActive();
            break;
        }
        if (idleTimeCounter > idleActivityInterval){
            toIdleActive();
            break;
        }
    case IDLE_ACTIVE:
        if (input_activity_flag){
            toHumanActive();
            break;
        }
        else if ((currentTime - idleActivatedTime) >idleActivityDuration){
            toIdle();
        }
        break;
    }
}

void FeatureControl::toIdle(){
    lastActivity = ofGetElapsedTimef();
    slowdownTime = ofGetElapsedTimef();
    idleFeatureIndex = idleFeatureIndexes[rand()%numIdleFeatures];
    state= IDLE;

}
void FeatureControl::toIdleActive(){
    state = IDLE_ACTIVE;
    idleActivatedTime = ofGetElapsedTimef();
}

void FeatureControl::toHumanActive(){
    state = HUMAN_ACTIVE;

}

void FeatureControl::update(){
    if (input_activity_flag){
        lastActivity = ofGetElapsedTimef();
    }

    currentTime = ofGetElapsedTimef();
    idleTimeCounter = currentTime - lastActivity;

    updateState();
    input_activity_flag = false;
    lastFeatureWeights =featureWeights;


    switch (state){
    case HUMAN_ACTIVE:
        //Todo
        break;

    case IDLE:
        for (int i = 0; i <featureWeights.size(); i++)
        {
            if (i!= idleFeatureIndex){
                inactiveCounter[i] = idleTimeout;
            }
            featureWeights[idleFeatureIndex] = 1.;
        }
        break;

    case IDLE_ACTIVE:
        //Todo make behaviour types and manage them
        break;
    }

    for (int i = 0; i <featureWeights.size(); i++)
    {
        if (featureWeights[i] >0.){
            inactiveCounter[i]+=1;
        }

        //After 5 seconds of inactivity decrement the weight by 0.1 every 0.5 seconds
        if (inactiveCounter[i]>idleTimeout && featureWeights[i] >0){
            featureWeights[i] = CLAMP(featureWeights[i] - featureDecayRate, 0., 1.);
        }

        else if (inactiveCounter[i]>idleTimeout && featureWeights[i] ==0.){
            featureActive[i]=false;
            continue;
        }
    }
}

void FeatureControl::draw(){

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
    targetFeatureValues[index] =CLAMP(targetFeatureValues[index]+step, 0., 1.);
    updateActiveFeature(index);
}

void FeatureControl::toggleFeatureTarget(int index){
    float v = targetFeatureValues[index];
    int target_value = 0.;
    if (v<.5) target_value = 1.;
    targetFeatureValues[index] =CLAMP(target_value, 0., 1.);
    updateActiveFeature(index);
}

void FeatureControl::updateActiveFeature(int index){
    featureActive[index]=true;
    featureWeights[index]=1.0;
    inactiveCounter[index] = 0;

    //Timeout the other features
    for(int i=0; i<this->inactiveCounter.size(); i++){
        if (i ==index )continue;
        inactiveCounter[i] = idleTimeout;
    }
}

void FeatureControl::registerInputActivity(){
    input_activity_flag = true;
}
