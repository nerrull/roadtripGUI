#ifndef POINTCLOUDRENDERER_H
#define POINTCLOUDRENDERER_H

#include "ofMain.h"
#include "util/featureKNN.h"
#include "gui/nodeparticlemanager.h"
#include "gui/physicalline.h"
class PointCloudRenderer
{
public:
    PointCloudRenderer();

    void draw();
    void update();
    void updatePoints();
    void initPoints(int nPoints, vector<ofColor> c);
    void initPoints(vector<ofVec3f> points, vector<ofColor>c);


    void setLayout(int x, int y, int w, int h);
    void updatePointPositions(vector<vector< float >> featureDistances, vector<float> weights);
    void updatePointPositionsOld(vector<vector< float >> featureDistances , vector<float> index_weights);
    void meshFromConnections(vector<NodeConnection>);
    void drawLines();
    void drawCurvyBoy();
    void setActiveNodes(vector<int>);
    void setPlayingNode(int playingIndex);


    ofPolyline line;
    ofPolyline twineLine;
    PhysicalLine p_line;

    ofMesh playedLine;
    ofEasyCam cam;
    ofRectangle viewMain;

    ofVboMesh mesh;
    ofMesh nextMesh;

    int drawCount =0;
    int point_step;
    int MAX_STEPS =30;
    int playingIndex;
    int num_points;

    //All points
    std::vector<ofVec3f> visualization_points;
    std::vector<ofVec3f> point_steps;
    vector<ofColor> colors;

    //Connections
    ofMesh connectionMesh;

    //Active points
    vector<int> draw_indexes;
    deque<ofVec3f> playedPoints;

    ofPolyline trail;

    NodeParticleManager nodeParticles;
};

#endif // POINTCLOUDRENDERER_H
