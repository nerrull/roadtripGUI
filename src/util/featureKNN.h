#ifndef FEATUREKNN_H
#define  FEATUREKNN_H

#include "ofMain.h"
#include "util/databaseloader.h"
#include "ofxKDTree.h"



struct NodeConnection{
public:
    NodeConnection(int p1,int p2,int d){
        firstPointIndex = p1;
        secondPointIndex = p2;
        depth = d;
    }

    int firstPointIndex;
    int secondPointIndex;
    int depth;
};

struct GraphNode{
public:
    GraphNode(){}

    GraphNode(int i, vector<int> cpi){
       pointIndex = i;
       connectedPointIndices=cpi;
    }

    int pointIndex;
    vector<int> connectedPointIndices;
};

class ConnectionGraph{
public:
    ConnectionGraph(){}

    void addNode(int i, vector<int> n ){
        nodes.push_back(GraphNode(i, n));
    }
    vector<NodeConnection> getPointPairsStartingFromIndex( int index, int maxpairs, int maxdepth);

private:
    int max_depth;
    vector<GraphNode> nodes;
    void recursiveGetPairs(int, vector<bool> &,vector<int> &, vector<NodeConnection> &,int ,int );
};



class FeatureKNN
{
public:
    FeatureKNN();
    void init(DatabaseLoader*);
    void update();
    void updateSearchRadius(float v);
    void updateSearchSpace(vector<float> desiredValues, vector<float> activeIndexes);
    vector<vector<float>> getPointFeatureDistances(const vector<float> search_point, const vector<float> index_weights);
    ConnectionGraph getLocalConnections(int featureIndex);

    void getKNN(vector<float> search_point, vector<float> search_weights);
    vector<int> getSearchResultIndexes();

    void setPlayingIndex(int);
    void setMinVideos(int);
    void setNumVideos(int);

    int curveIterCount = 0;

    int num_points;
    int playingIndex =0;
    int targetNumberOfPoints = 50;
    int minVideos = 1;
    int numVideos = -1;

    float threshold_distance = 0.05;


    vector<int> active_indexes;
    vector<ConnectionGraph> featureConnectionGraphs;
    vector<NodeConnection> getConnectionsForFeature(int featureIndex);
    void initPoints();


private:
    void createFeatureGroups();
    DatabaseLoader* db;
    std::vector<vector<double>> search_space;

    ofxKDTree kdTree;
    vector<size_t> search_indexes;
    vector<double> search_dists;


};

#endif //  FEATUREKNN_H
