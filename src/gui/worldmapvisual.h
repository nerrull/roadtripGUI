#ifndef WORLDMAPVISUAL_H
#define WORLDMAPVISUAL_H

#include "ofMain.h"
#include "util/mapfileparser.h"

class WorldMapVisual
{

public:
    WorldMapVisual();
    void draw();
    void setLayout(int,int,int,int);
    void setVideoPoints(vector<ofVec2f>);

private :
    ofMesh geomesh;
    ofMesh bordermesh;
    ofMesh rivermesh;
    ofEasyCam cam;
    ofRectangle viewMain;
    vector<ofVec3f> videoPoints;

    ofMesh videoPointMesh;

};

#endif // WORLDMAPVISUAL_H
