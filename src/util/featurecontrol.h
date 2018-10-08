#ifndef FEATURECONTROL_H
#define FEATURECONTROL_H

#include "ofMain.h"
#include "util/featureKNN.h"
#include "util/databaseloader.h"
#include "util/communication.h"


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

class FeatureControl
{
public:
    FeatureControl(DatabaseLoader *, CommunicationManager*);

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


    int secondsToFrames = 60.;

    int numIdleFeatures= 4;
    int idleFeatureIndexes[4] = {0,4,5,6};
    int numIdleActiveFeatures= 7;
    int idleActiveFeatureIndexes[7] = {0,1,4,6,10,11,16};

    BehaviourState state;
    ActivityTypes activityType;
    FeatureKNN* fKNN;
    DatabaseLoader* dbl;
    CommunicationManager* coms;

    void initialize(DatabaseLoader * dbl);
    void update();

    void incrementFeatureTarget(int index, float step);
    void toggleFeatureTarget(int index);
    void updateActiveFeature(int index,int timeoutOffset);
    bool shouldSlowdown();
    void registerInputActivity();
    void draw();
    void playRandomVideo();
    int incrementSearchRadius(int step);
    void incrementSpeed(int step);
    void setSpeed(int speed);

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
     pair<string, int> getPlayingVideo(){
        return playingVideo;
    }
    vector<int> getVideoIndexes(){return videoIndexes;}

private:
    //State functions
    void updateState();
    void toIdle();
    void toIdleActive();
    void toHumanActive();

    //Update feature functions
    void setIdleFeature(int index);
    void updateFeatureWeights(bool ignoreIdle);
    void updateFeatureValues(vector<float> fv);

    //Video search/playback functions
    void getNewVideos();
    void playVideo();
    void cycleVideo();

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
    float videoCycleTimer;
    float videoStartTime;

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

    vector<pair<string, int>> videos;
    pair<string, int> playingVideo;

    int videoCycleIndex;
    int videoMaxIndex;
    int speedSetting;

    int SPEEDS [15]= {-1, 4000, 3000, 2000, 1500, 1000, 500,400, 333, 250, 200, 125, 100, 63, 33};
    string SPEEDSTRINGS [15]= {"MAX", "4", "3", "2", "1.5", "1", "1/2","1/3","1/4","1/5", "1/8","1/10", "1/12", "1/16", "1/32"};



};

#endif // FEATURECONTROL_H
