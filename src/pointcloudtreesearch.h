#ifndef POINTCLOUDVISUALIZER_H
#define POINTCLOUDVISUALIZER_H

#include "ofMain.h"
#include "databaseloader.h"
#include "ofxKDTree.h"

class PointCloudTreeSearch
{
public:
    PointCloudTreeSearch(DatabaseLoader*);

    void update();
    void draw();
    void updatePoints();
    void updateSearchSpace(vector<float> desiredValues, vector<float> activeIndexes);

    void initPoints();
    void setViewPort(int x, int y, int w, int h);
//    void getNearestPoints();
    void getKNN(vector<float> search_point, vector<float> search_weights);

    vector<int> getSearchResultIndexes();
    void setPlayingIndex(int);
    void drawLines();
    void drawCurvyBoy();
    int curveIterCount = 0;

    int num_points;
    int drawCount =0;
    int playingIndex =0;
    int targetNumberOfPoints = 50;
    int minVideos = 5;
    float threshold_distance = 0.01;

    deque<ofVec3f> playedPoints;
    ofPolyline line;
    ofPolyline twineLine;

    ofMesh playedLine;
    DatabaseLoader* db;
    std::vector<ofVec3f> visualization_points;
    std::vector<vector<double>> search_space;


    std::vector<ofVec3f> point_steps;
    vector<ofColor> colors;

    ofxKDTree kdTree;
    vector<size_t> search_indexes;
    vector<double> search_dists;
    vector<int> draw_indexes;


    ofEasyCam cam;
    ofRectangle viewMain;

    ofVboMesh mesh;
    ofMesh nextMesh;
    int point_step;
    int MAX_STEPS =30;
    float search_radius;
    float max_search_radius;

};

#endif // POINTCLOUDVISUALIZER_H
