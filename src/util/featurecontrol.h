#ifndef FEATURECONTROL_H
#define FEATURECONTROL_H

#include "ofMain.h"

class FeatureControl
{
public:

    enum BehaviourState{
        IDLE,
        IDLE_ACTIVE,
        HUMAN_ACTIVE,
    };

//    IdleFeatures{
//        AMPLITUDE = 0,
//        SPEED =4,
//        STABILITY = 5,
//        RPM = 6,
//    };

    int numIdleFeatures= 4;

    int idleFeatureIndexes[4] = {0,4,5,6};


    BehaviourState state;
    FeatureControl();

    void update();
    void updateFeatureValues(vector<float> fv);
    void incrementFeatureTarget(int index, float step);
    void toggleFeatureTarget(int index);
    void updateActiveFeature(int index);
    void registerInputActivity();

    void draw();

    vector<float> getValues(){
        return featureValues;
    }

    vector<float> getWeights(){
        return featureValues;
    }
    vector<float> getTargetValues(){
        return targetFeatureValues;
    }

    float getWeight(int index){
        return featureWeights[index];
    }
    float getValue(int index){
        return featureValues[index];
    }
    float getLastValue(int index){
        return lastFeatureValues[index];
    }
    float getTargetValue(int index){
        return targetFeatureValues[index];
    }

    float getActive(int index){
        return featureActive[index];
    }

    bool weightChanged(int index){
        if (lastFeatureWeights[index]!= featureWeights[index]){
            return true;
        }
        return false;
    }

    float getValueDiff(int index){
        return featureValues[index] - lastFeatureValues[index];
    }

    int numFeatures(){
        return featureValues.size();
    }

    void incrementLastFeatureValue(int index, float inc){
        lastFeatureValues[index]+=inc;
    }

    bool shouldSlowdown(int speedSetting){
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

private:

    //State functions
    void updateState();
    void toIdle();
    void toIdleActive();
    void toHumanActive();

    //Timeout parameters
    float featureTimeout;
    float featureDecayRate;

    float lastActivity;
    float idleActivityInterval;
    float idleActivityDuration;

    float idleTimeout;
    float idleTimeCounter;
    float idleActivatedTime;

    float slowdownTime;

    float currentTime;

    int idleFeatureIndex;

    bool input_activity_flag;

    vector<bool> featureActive;
    vector<int> inactiveCounter;

    vector<float> targetFeatureValues;
    vector<float> featureValues;
    vector<float> lastFeatureValues;

    vector<float> featureWeights;
    vector<float> lastFeatureWeights;






};

#endif // FEATURECONTROL_H
