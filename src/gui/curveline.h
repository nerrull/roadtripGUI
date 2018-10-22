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
        int tempRes = resolution;
        if (tp!= targetPoint){
            float dist = tp.distance(targetPoint);
            if (dist<resolution){
                tempRes =CLAMP(dist, 3, resolution);
            }

            line.curveTo(tp,tempRes);

            if (tp == lastPoint){
                ofVec3f ttp = targetPoint +(tp -targetPoint)/2 +ofVec3f(ofRandom(50), ofRandom(50));
                float dist = ttp.distance(targetPoint);
                if (dist<50){
                    tempRes =CLAMP(2*dist, 3, 100);
                }
                line.curveTo(ttp,tempRes);
            }

            lastPoint=targetPoint;
            targetPoint = tp;
            if (line.getVertices().size()>tempRes){
                drawOffset = tempRes;
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
            if (line.size()>1){
                float step =line.size()/ (float(resolution)/3);
                step = CLAMP(step, 1, 100);
                step = CLAMP(step, 0, line.size()-1);

//                step = CLAMP(resolution/30,0,line.size()-1) ;
                int numsteps = (int) floor(step);

                if (line.size() > 32*resolution){
                    numsteps +=line.size() - 32*resolution;
                }
                if (step <1.){
                    stepcounter+=step;
                    if (stepcounter>1.){
                        numsteps =1.;
                        stepcounter -=1;
                    }
                }

    //            std::vector<ofDefaultVertexType> verts= line.getVertices();
    //            std::vector<ofDefaultVertexType> t(verts.begin()+numsteps, verts.end());
    //            line.clear();
    //            line.addVertices(t);
                line.shorten(numsteps);

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
    ofVec3f lastPoint;
    float stepcounter;
    int drawOffset =0;
    int resolution = 100;

};

#endif // CURVELINE_H
