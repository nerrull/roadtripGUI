#ifndef POINTCLOUDVISUALIZER_H
#define POINTCLOUDVISUALIZER_H

#include "ofMain.h"
#include "ofxSpatialHash.h"
#include "databaseloader.h"

class PointCloudTreeSearch
{
public:
    PointCloudTreeSearch(DatabaseLoader*);

    void update();
    void draw();
    void updatePoints();
    void updateCoordinates();
    void updateActiveCoordinates(float *desiredValues, bool* activeIndexes);

    void initPoints();
    void setViewPort(int x, int y, int w, int h);
    void getNearestPoints();
    vector<int> getSearchResultIndexes();
    void setPlayingIndex(int);
    void drawLines();



    int num_points;
    int drawCount =0;
    int playingIndex =0;
    int targetNumberOfPoints = 15;
    deque<int> playedPoints;
    ofPolyline line;

    DatabaseLoader* db;
    std::vector<ofVec3f> points;
    std::vector<ofVec3f> point_steps;
    vector<ofColor> colors;

    ofx::KDTree<ofVec3f> hash;
    ofx::KDTree<ofVec3f>::SearchResults searchResults;

    ofEasyCam cam;
    ofRectangle viewMain;

    ofVboMesh mesh;
    ofMesh nextMesh;
    int point_step;
    int MAX_STEPS =60;
    float search_radius;
    float max_search_radius;




};

#endif // POINTCLOUDVISUALIZER_H
