#ifndef PHYSICALLINE_H
#define PHYSICALLINE_H
#include "ofMain.h"


class PhysicalLine{


public:
    PhysicalLine(){
        currentVelocity = ofVec3f(0.,0.,0.);
    }


    ~PhysicalLine(){

    }
    void update(ofVec3f tp){
        if (tp != targetPoint){
            lastTarget = targetPoint;
            targetPoint = tp;
            ofVec3f distance = targetPoint - currentPosition;
            ofVec2f perp =ofVec2f(distance.x, distance.y).rotate(90.).getNormalized();
            currentVelocity = ofVec3f(perp.x, perp.y, 0.) *distance/10.;
        }

        //friction
        currentVelocity *= 0.9;
        currentPosition += currentVelocity;

        if (targetPoint.distance(currentPosition)<2.){
//            if (currentPosition !=targetPoint){
                currentPosition = targetPoint;
                addPoint(currentPosition);
//            }
        }

        else{
            currentVelocity += attract(targetPoint, currentPosition, 10.);
            addPoint(currentPosition);

        }
        if (pointQueue.size()>120){
            dropPoint();
        }

        line.clear();
        for (auto p:pointQueue){
            line.curveTo(p);
        }
    }

    void draw(){
        line.draw();

        ofDrawLine(lastTarget, targetPoint);
    }


private:
    ofPolyline line;
    deque<ofVec3f> pointQueue;
    ofVec3f currentPosition;
    ofVec3f currentVelocity;
    ofVec3f lastTarget;
    ofVec3f targetPoint;

    void addPoint(ofVec3f p){
        pointQueue.push_front(p);
    }

    void dropPoint(){
        pointQueue.pop_back();
    }

    ofVec3f attract(ofVec3f point, ofVec3f target, float gravity){
        float dist  = target.distance(point);
        ofVec3f direction = (point - target).getNormalized();
        float force = 1.;
        if (dist >=1.){
            force =1.;
        }

        else {
            currentVelocity = ofVec3f(0,0,0);
            force = 1.;

        }
        return direction * force;
    }


};
#endif // PHYSICALLINE_H
