#include "featureKNN.h"
#include <nanoflann.hpp>

using namespace nanoflann;
vector<NodeConnection> ConnectionGraph::getPointPairsStartingFromIndex(int start_index, int max_connections, int maxdepth=-1){
    this->max_depth =maxdepth;
    int num_nodes = this->nodes.size();
    vector<bool> passed_indexes;
    vector<int> connection_counts;
    vector<NodeConnection> connections;
    passed_indexes.resize(num_nodes, false);
    connection_counts.resize(num_nodes, 0);

    passed_indexes[start_index] = true;
    GraphNode start_node = nodes[start_index];
    for (int i= 0; i <start_node.connectedPointIndices.size(); i++){
//    for (auto node_index :start_node.connectedPointIndices){
        int node_index= start_node.connectedPointIndices[i];
        connections.push_back(NodeConnection(start_index, node_index, 0));
        connection_counts[start_index]+=1;
        connection_counts[node_index]+=1;
    }
    for (auto nodeIndex :start_node.connectedPointIndices){
        recursiveGetPairs(nodeIndex, passed_indexes, connection_counts, connections, max_connections,0 );
    }
    return connections;
}


void ConnectionGraph::recursiveGetPairs(int current_index, vector<bool>& passed_indexes,vector<int> & connection_counts,
                                   vector<NodeConnection> & connections, int max_connections, int depth ){
    depth +=1;
    if (max_depth !=-1 &&depth >max_depth){
        return;
    }

    if (connection_counts[current_index] >=max_connections){
        return;
    }

    passed_indexes[current_index] = true;
    GraphNode current_node = nodes[current_index];

    vector<int> nextIndexes;

    for (auto node_index :current_node.connectedPointIndices){
        // If the other node doesn't already max connections (to avoid having a ton of connections)
        // and hasn't been passed over (to avoid double connections)
        if (connection_counts[current_index] >= max_connections){
            break;
        }

        if (connection_counts[node_index] >= max_connections || passed_indexes[node_index] ) {
            continue;
        }
            connections.push_back(NodeConnection(current_index,node_index, depth));
            connection_counts[current_index]+=1;
            connection_counts[node_index]+=1;
            passed_indexes[node_index] = true;
            nextIndexes.push_back(node_index);
    }
    //Recursive call after updating all the connections (otherwise it would be depth-first)
    for (auto node_index :nextIndexes){
        recursiveGetPairs(node_index, passed_indexes, connection_counts,connections, max_connections,depth );
    }
}


//vector<graph_node> RandomDistributionGenerator::get_weighted_nearest_neighbours(,int num_neighbours){
//    ofx::KDTree<ofVec3f> hash(points);
//    ofx::KDTree<ofVec3f>::SearchResults results;
//    vector<graph_node> neighbour_nodes;

//    for (size_t idx = 0; idx< points.size(); idx++){
//        graph_node n;
//        n.point_index = idx;
//        ofVec3f point = points[idx];
//        //get the 4 nearest neighbours
//        hash.findNClosestPoints(point, num_neighbours+1, results);
//        // disregard first point because it's the search point
//        vector<float> distances;
//        distances.resize(num_neighbours);
//        float total_distance = 0.;

//        for (size_t i = 1; i<results.size(); i++){
//           float d=results[i].second;
//           total_distance +=d;
//           distances[i-1] =d;
//           n.near_indexes.push_back(results[i].first);
//        }
//        float sum= 0;
//        for (auto d : distances){
//           sum +=(total_distance - d)/total_distance;
//           n.near_weights.push_back(sum);
//        }
//        for (size_t i = 0; i< n.near_weights.size(); i++){
//           n.near_weights[i] =n.near_weights[i]/ sum;
//        }
//        neighbour_nodes.push_back(n);
//    }
//    return neighbour_nodes;
//}



FeatureKNN::FeatureKNN()
{

}

void FeatureKNN::setPlayingIndex(int i){
    playingIndex= i;

}

void FeatureKNN::updateSearchRadius(float v){
    threshold_distance = v;
}

void FeatureKNN::init(DatabaseLoader *database){
    db = database;
    createFeatureGroups();
    initPoints();
}

vector<float> getFeatureColumn(vector<vector<float>> featureValues, int columnIndex){
    int  num_points= featureValues.size();
    vector<float> result;
    result.reserve( num_points );
    for( int i = 0; i < num_points; i++ ) {
        result.push_back(featureValues[i][columnIndex]);
    }
    return result;
}

void FeatureKNN::createFeatureGroups(){
    // KNN on each feature set (overkill but whatever)
    int numFeatures =  db->getNumFeatures();
    vector<vector<float>> featureValues =  *db->getFeatures();
    int npoints= featureValues.size();
    ofxKDTree tempKD;
    vector<size_t> s_i;
    vector<double> s_d;
    vector<vector<int>> groups;
    featureConnectionGraphs.clear();
    for (std::size_t i = 0; i < numFeatures; ++i){
        tempKD.clear();
        vector<float> values = getFeatureColumn(featureValues, i);
        ConnectionGraph featureConnectedGraph;
        for (auto v:values){
            vector<double> vec;
            vec.push_back(v);
            tempKD.addPoint(vec);
        }
        tempKD.constructKDTree();

        int minNeighbours = 3;
        int pointIndex = 0;

        for (auto v:values){
            int numNeighbours = 0;
            vector<double> search_point;
            search_point.push_back(v);
            tempKD.getKNN(search_point,values.size(), s_i, s_d);
            vector<int> neighbours;

            for (int idx =1; idx<s_d.size(); idx++){
                //Take the square root because we want 1-D distance and I don't know hwo to change template classes in pofxKDTree
                float current_distance = sqrt(s_d[idx]);
                int current_index= s_i[idx];
                //Each node has a minimum of 3 neighbours, a max of 100 neighbours
                // and the distance to neighbours should rarely exceed 0.05
                if (current_distance > 0.05 && neighbours.size() >= 3){
                    break;
                }
                neighbours.push_back(current_index);
            }
            featureConnectedGraph.addNode(pointIndex, neighbours);
            pointIndex++;
        }

        featureConnectionGraphs.push_back(featureConnectedGraph);
    }
}

vector<NodeConnection>  FeatureKNN::getConnectionsForFeature(int featureIndex){
    float t =ofGetElapsedTimeMillis();
    vector<NodeConnection> connections = featureConnectionGraphs[featureIndex].getPointPairsStartingFromIndex(playingIndex,2 ,-1);
    t =ofGetElapsedTimeMillis() -t;
    ofLogNotice()<<"Getting connections took "<< t << "milliseconds"<<endl;
    return connections;

}

ConnectionGraph  FeatureKNN::getLocalConnections(int featureIndex){
    return this->featureConnectionGraphs[featureIndex];
}

void FeatureKNN::initPoints(){

    num_points= db->feature_values.size();

    kdTree.clear();

    for (std::size_t i = 0; i < num_points; ++i)
    {
        vector<double> double_point(db->getFeatures()[0][i].begin(), db->getFeatures()[0][i].end());
        kdTree.addPoint(double_point);
    }

    kdTree.constructKDTree();
}

void FeatureKNN::getKNN(vector<float> search_point, vector<float> search_weights)
{
    vector<double> double_point(search_point.begin(),search_point.end());
    vector<double> double_weights(search_weights.begin(),search_weights.end());

    kdTree.getWeightedKNN(double_point, targetNumberOfPoints, search_indexes, search_dists, double_weights);

}

void FeatureKNN::update(){

    ofLogNotice() <<"Point cloud tree search does nothing on update?"<<endl;
}


vector<vector<float>> FeatureKNN::getPointFeatureDistances(const vector<float> search_point, const vector<float> index_weights){

    int num_features = db->feature_values[0].size();
    float epsilon = 0.0001;

    float total_weight = std::accumulate(index_weights.begin(), index_weights.end(), 0.);
//    cout<<total_weight<<endl;
    int active_features = 0;
    for (auto w : index_weights){
        if (w > 0.) active_features ++;
    }
    vector<vector<float>> featureDistances;
    vector<float> distances;

    for (std::size_t i = 0; i < num_points; ++i){
        distances.clear();
        for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
            //Ignore unweighted values
            if (index_weights[featureIndex] <=0.) continue;
            float diff = search_point[featureIndex]-db->feature_values[i][featureIndex];
            float dist = abs(diff);
            distances.push_back(dist);

        }
        featureDistances.push_back(distances);

    }
    return featureDistances;
}




vector<int> FeatureKNN::getSearchResultIndexes(){
    active_indexes.clear();
    int n_v =0;
    for (std::size_t i = 0; i < search_indexes.size(); ++i)
    {
        float v = search_dists[i];
        active_indexes.push_back(search_indexes[i]);
        n_v++;
        if (v > threshold_distance){
            break;
        }
    }

    if (active_indexes.size() <minVideos){
        for (;n_v < minVideos; n_v++){
            active_indexes.push_back(search_indexes[n_v]);
        }
    }

    return active_indexes;
}

