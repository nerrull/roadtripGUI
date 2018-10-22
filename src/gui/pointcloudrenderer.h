#ifndef POINTCLOUDRENDERER_H
#define POINTCLOUDRENDERER_H

#include "ofMain.h"
#include "gui/physicalline.h"
#include "gui/curveline.h"

class PointCloudRenderer
{
public:
    PointCloudRenderer();

    void draw();
    void update();
    void updatePoints();
    void initPoints(vector<ofVec3f> points, vector<ofColor>c);
    void setRotation(bool);


    void setLayout(int x, int y, int w, int h);
    void updatePointPositions(vector<vector< float >> featureDistances, vector<float> weights);
    void drawLines();
    void setActiveNodes(vector<int>);
    void setPlayingNode(int playingIndex);
    void updateLine(int);



    bool rotationOn;
    int drawCount =0;
    int point_step;
    int MAX_STEPS =30;
    int playingIndex;
    int num_points;
    float scaleFactor;

    //All points
    std::vector<ofVec3f> visualization_points;
    std::vector<ofVec3f> point_steps;
    vector<ofColor> colors;
    vector<int> activePointIndexes;

    //Connections
    ofMesh connectionMesh;


    //Active points
    deque<ofVec3f> playedPoints;


    //Drawing
    CurveLine c_line;
    ofEasyCam cam;
    ofRectangle viewMain;

    ofVboMesh mesh;
    ofVboMesh playingPointMesh;
    ofVboMesh activePointMesh;
    vector<float> point_sizes;

    ofShader billboardShader;
    ofImage texture;
    int minPointSize;
    int maxPointSize;

//    NodeParticleManager nodeParticles;
};

#endif // POINTCLOUDRENDERER_H
