#ifndef CURVELINE_H
#define CURVELINE_H
#include "ofMain.h"
#include "ofMath.h"

class CurveLine{
public:
    CurveLine(){
        line.clear();
        targetPoint = ofVec3f(0,0,0);
        drawOffset =0;
        resolution = 100;
    }

    ~CurveLine(){
    }

    void update(ofVec3f tp){

        if (tp!= targetPoint){
            line.curveTo(tp,resolution);
            targetPoint = tp;
            drawOffset = 100;
        }
//        for (int i = 0; i<2; i++){
//            pointAges.push_front(ofGetFrameNum());
//        }


        if(!line.getVertices().empty()){

            if (drawOffset !=0){
                drawLine.clear();
                drawOffset = MAX(0,drawOffset -resolution/10 );
                auto first = line.getVertices().begin();
                auto last = line.getVertices().begin() +line.getVertices().size() - drawOffset;
                vector<ofDefaultVertexType> newVec(first, last);
                drawLine.addVertices(newVec);
            }

            float step =line.size()/(resolution/2.);
            int numsteps = (int) floor(step);
            if (step <1.){
                stepcounter+=step;
                if (stepcounter>1.){
                    numsteps =1.;
                    stepcounter -=1;
                }
            }

            for (int i =0;i<numsteps;i++){
                line.getVertices().erase(line.getVertices().begin());
            }
        }

    }

    void draw(){
        if (drawOffset >0){
            drawLine.draw();
        }
        else{
            line.draw();
        }
    }


private:
    ofPolyline drawLine;
    ofPolyline line;

    deque<ofVec3f> pointAges;
    ofVec3f targetPoint;
    float stepcounter;
    int drawOffset =0;
    int resolution = 100;

    void addPoint(ofVec3f p){
        pointAges.push_front(p);
    }

    void dropPoint(){
        pointAges.pop_back();
    }



};

#endif // CURVELINE_H
