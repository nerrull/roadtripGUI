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
        currentVelocity *= 0.9;
        currentPosition += currentVelocity;
        currentVelocity += attract(targetPoint, currentPosition, 10.);

        addPoint(currentPosition);
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
        if (dist >=50.){
            force =1.;
        }

        else if (dist <50. && dist >2.){
            force = 0.5;
        }
        else {
            currentVelocity = currentVelocity.normalize();
            force = 1.;

        }
        return direction * force;
    }


};
#endif // PHYSICALLINE_H
