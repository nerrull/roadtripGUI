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
    void update(ofVec3f targetPoint){
        //friction
        currentVelocity *= 0.98;
        currentPosition += currentVelocity;
        currentVelocity += attract(targetPoint, currentPosition, 10.);

        addPoint(currentPosition);
        if (pointQueue.size()>400){
            dropPoint();
        }

        line.clear();
        for (auto p:pointQueue){
            line.curveTo(p);
        }
    }

    void draw(){
        line.draw();
    }


private:
    ofPolyline line;
    deque<ofVec3f> pointQueue;
    ofVec3f currentPosition;
    ofVec3f currentVelocity;

    void addPoint(ofVec3f p){
        pointQueue.push_front(p);
    }

    void dropPoint(){
        pointQueue.pop_back();
    }

    ofVec3f attract(ofVec3f point, ofVec3f target, float gravity){
        float dist  = target.distance(point);
        ofVec3f direction = (point - target).getNormalized();
        float force = 10.;
        if (dist >100.){
            force =10.;
        }

        else if (dist <100. && dist >50.){
            force = gravity/dist;
        }
        else {
            currentVelocity = currentVelocity.normalize();
            force = 1.;

        }
        return direction * force;
    }


};
#endif // PHYSICALLINE_H
