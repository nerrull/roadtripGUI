#ifndef CURVELINE_H
#define CURVELINE_H
#include "ofMain.h"
#include "ofMath.h"
#include "ofxFatLine.h"

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

    void addPoint(ofVec3f tp){
        if (tp!= targetPoint){
            line.curveTo(tp,resolution);
//            line.curveTo(tp +ofVec3f(0.001,0.001, 0.001),100);
            targetPoint = tp;
            if (line.getVertices().size()>resolution){
                drawOffset = resolution;
            }


        }
    }
    void update(){
        ofPushStyle();
        if(!line.getVertices().empty()){
//            if (drawOffset !=0){
//                drawLine.clear();
//                drawOffset = MAX(0,drawOffset - resolution/10 );
//                auto first = line.getVertices().begin();
//                auto last = line.getVertices().begin() +line.getVertices().size() - drawOffset;
//                vector<ofDefaultVertexType> newVec(first, last);
//                drawLine.addVertices(newVec);
//            }

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


            ofPopStyle();
            int numVerts = line.getVertices().size();
            vector<ofFloatColor> colors;

            for (int i =0; i<numVerts; i++){
                float s =  float(i)/float(numVerts) *0.5;
                colors.push_back(ofFloatColor(1.,1.,1.,s+0.1));
            }
            ofSetLineWidth(1);
            fatLine.clear();
            fatLine.setFromPolyline(line, colors);
            ofPushStyle();

        }
        ofPopStyle();
    }

    void draw(){
        ofPushStyle();
        ofSetColor(255,255,255,200);
//        line.draw();
        fatLine.draw();
//        if (drawOffset >0){
//            drawLine.draw();
//        }
//        else{
//        }
        ofPopStyle();

    }


private:
    ofPolyline drawLine;
    ofPolyline line;
    ofxFatLine fatLine;

    deque<ofVec3f> pointAges;
    ofVec3f targetPoint;
    float stepcounter;
    int drawOffset =0;
    int resolution = 100;

};

#endif // CURVELINE_H
