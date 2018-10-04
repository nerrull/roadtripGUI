#ifndef FEATURECONTROL_H
#define FEATURECONTROL_H

#include "ofMain.h"
#include "util/featureKNN.h"
#include "util/databaseloader.h"

class FeatureControl
{
public:
    FeatureControl();

    string stateStrings [3]= {"IDLE", "IDLE_ACTIVE", "HUMAN_ACTIVE"};
    enum BehaviourState{
        IDLE,
        IDLE_ACTIVE,
        HUMAN_ACTIVE,
    };

    string activityTypeStrings [5]= {"NONE","ASCENDING", "DESCENDING", "UP_DOWN", "DOWN_UP"};
    enum class ActivityTypes{
        NONE,
        ASCENDING,
        DESCENDING,
        UP_DOWN,
        DOWN_UP,
        first = NONE,
        last =DOWN_UP
    };

    string activitymodifierStrings [4]= {"NONE", "ACCELERATE", "DECELERATE", "RANDOM_TIMINGS"};
    enum class ActivityModifiers{
        NONE,
        ACCELERATE,
        DECELERATE,
        RANDOM_TIMINGS,
        first = NONE,
        last = RANDOM_TIMINGS
    };

/*    IdleFeatures{
        AMPLITUDE = 0,
        SPEED =4,
        STABILITY = 5,
        RPM = 6,
    };
    IdleActiveFeatures{
        AMPLITUDE = 0,
        Spectral Centroid =1,
        Vehicle speed = 4,
        RPM =6,
        LIGHTNESS = 11,
        UNCERTAINTY = 12,
        VEGETATION = 17
    }
*/

    int secondsToFrames = 60.;

    int numIdleFeatures= 4;
    int idleFeatureIndexes[4] = {0,4,5,6};
    int numIdleActiveFeatures= 7;
    int idleActiveFeatureIndexes[7] = {0,1,4,6,10,11,16};

    BehaviourState state;
    ActivityTypes activityType;
    FeatureKNN fKNN;
    void initialize(DatabaseLoader * dbl);
    void update();
    void updateFeatureValues(vector<float> fv);
    void incrementFeatureTarget(int index, float step);
    void toggleFeatureTarget(int index);
    void updateActiveFeature(int index);
    bool shouldSlowdown(int speedSetting);
    void registerInputActivity();
    void draw();
    void getNewVideos();
    int incrementSearchRadius(int step);

    vector<float> getValues(){
        return featureValues;
    }

    vector<float> getWeights(){
        return featureWeights;
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

    vector<int> getVideoIndexes(){return videoIndexes;}


private:

    //State functions
    void updateState();
    void toIdle();
    void toIdleActive();
    void toHumanActive();
    void setIdleFeature(int index);
    void updateFeatureTimers(bool ignoreIdle);


    //Timeout parameters
    float featureTimeout;
    float featureDecayRate;
    float idleActivityInterval;
    float idleActivityDuration;
    float idleTimeout;


    int idleActivityNumUpdates;
    deque<float> idleActivityValues;
    deque<float> idleActivityTimings;

    //Timers
    float lastActivityTime;
    float idleTimeCounter;
    float idleActivatedTime;

    float slowdownTime;

    float currentTime;

    //Feature indexes
    int idleFeatureIndex;
    int activeFeatureIndex;

    bool input_activity_flag;
    bool update_video_flag;

    vector<bool> featureActive;
    vector<int> inactiveCounter;
    vector<int> videoIndexes;

    vector<float> targetFeatureValues;
    vector<float> featureValues;
    vector<float> lastFeatureValues;

    vector<float> featureWeights;
    vector<float> lastFeatureWeights;

};

#endif // FEATURECONTROL_H
