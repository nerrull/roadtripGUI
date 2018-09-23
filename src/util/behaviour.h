#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H
#include "ofMain.h"

class Behaviour
{
public:

    enum BehaviourState{
        IDLE,
        IDLE_ACTIVE,
        HUMAN_ACTIVE,
    };

    BehaviourState state;

    Behaviour(){
        state = IDLE;

        //Todo: load from settings
        randomActivityInterval = 30; //seconds
        idleTimeLimit = 6; //seconds

    }


    void update(bool humanActivity){
        if (humanActivity){
            lastHumanActivity = ofGetElapsedTimef();
        }

        currentTime = ofGetElapsedTimef();
        idleTimeCounter = currentTime - lastHumanActivity;

        updateState(humanActivity);
    }

    void updateState(bool humanActivity){
        switch (state){
        case HUMAN_ACTIVE:
            if (idleTimeCounter> idleTimeLimit){
                state = IDLE;
                break;
            }
        case IDLE:
            if (humanActivity){
                state = HUMAN_ACTIVE;
                break;
            }
            if (idleTimeCounter > randomActivityInterval){
                state = IDLE_ACTIVE;
                break;
            }
        case IDLE_ACTIVE:
            if (humanActivity){
                state = HUMAN_ACTIVE;
                break;
            }
            break;
        }
    }

    float randomActivityInterval;
    float idleTimeLimit;

    float lastHumanActivity;
    float currentTime;
    float idleTimeCounter;

};

#endif // BEHAVIOUR_H
