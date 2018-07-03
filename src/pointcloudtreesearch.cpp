#include "pointcloudtreesearch.h"

PointCloudTreeSearch::PointCloudTreeSearch(DatabaseLoader *database)
{
    db = database;
    search_radius = 5.;
    max_search_radius = 20.;
    point_step = MAX_STEPS+1;
    playedLine.setMode(OF_PRIMITIVE_LINE_STRIP);
}

void PointCloudTreeSearch::setViewPort(int x, int y, int w, int h){
    viewMain.x=x;
    viewMain.y = y;
    viewMain.width = w;
    viewMain.height =h;
    cam.setPosition(ofVec3f(0.,0.,100.));
    cam.lookAt(ofVec3f(0,0,0));
    cam.setDistance(100);
}

void PointCloudTreeSearch::setPlayingIndex(int i){
    playingIndex= i;

}

void PointCloudTreeSearch::initPoints(){

    num_points= db->feature_values.size();
    mesh.setMode(OF_PRIMITIVE_POINTS);

    kdTree.clear();

    float h,s,v;
    float radius,angleY,angleZ;
    float x,y,z;
    ofColor c;
    for (std::size_t i = 0; i < num_points; ++i)
    {

        radius = ofRandom( 0,10);
        angleY = ofRandomf() *PI;
        angleZ= ofRandomf()*PI;

        x = radius*sin(angleY)*cos(angleZ);
        y = radius*sin(angleY)*sin(angleZ);
        z = radius*cos(angleY);

        c = db->colors[i];

        ofVec3f point(x,y,z);
        mesh.addVertex(point);
        mesh.addColor(c);
        point_steps.push_back(ofVec3f(0.,0.,0.));
        visualization_points.push_back(point);
        vector<double> double_point(db->getFeatures()[0][i].begin(), db->getFeatures()[0][i].end());
        kdTree.addPoint(double_point);
    }

    kdTree.constructKDTree();

    point_step = MAX_STEPS +1;
}

void PointCloudTreeSearch::getKNN(vector<float> search_point, vector<float> search_weights)
{
    vector<double> double_point(search_point.begin(),search_point.end());
    vector<double> double_weights(search_weights.begin(),search_weights.end());

    kdTree.getWeightedKNN(double_point, targetNumberOfPoints, search_indexes, search_dists, double_weights);

}

void PointCloudTreeSearch::update(){
    updatePoints();

    if (playedPoints.size() >10){
        playedPoints.pop_front();
    }


//    if (playedPoints.size() >1){
//        line.clear();
//        playedLine.clear();
//        ofVec3f p = playedPoints[0];
//        line.addVertex(p);

//        for (int pointIndex=1; pointIndex < playedPoints.size(); pointIndex ++){
//            p = playedPoints[pointIndex];
//            line.curveTo(p);
//        }

//    }
}

void PointCloudTreeSearch::draw()
{
    ofVec3f centerPoint = ofVec3f(0.,0.,0.);


    cam.begin(viewMain);
    ofPushMatrix();
    ofRotateZDeg(0.0573*drawCount);
    ofRotateXDeg(0.137*drawCount);


    ofNoFill();
    ofSetColor(255);
    drawCount++;

    // Draw all of the points.
    mesh.draw();

    ofFill();
    ofSetColor(255, 255, 0, 80);

    for (std::size_t i = 0; i < draw_indexes.size(); ++i)
    {
        ofDrawSphere(visualization_points[draw_indexes[i]], .5);
    }


    ofSetColor(255, 0, 0);
    ofDrawIcoSphere(visualization_points[playingIndex], 1.);
    ofSetColor(255);

    ofPopMatrix();
    cam.end();

}

void PointCloudTreeSearch::drawLines(){
    for (std::size_t i = 0; i < num_points; i++)
    {
        ofSetColor(db->colors[i].r, db->colors[i].g, db->colors[i].b, 4);
        for (std::size_t j = i; j < num_points; j++)
        {
            ofDrawLine(visualization_points[i], visualization_points[j]);
        }
    }
}


void PointCloudTreeSearch::drawCurvyBoy(){
    twineLine.clear();
    for (auto p: visualization_points)
    {
        twineLine.curveTo(p);
    }
    ofSetColor(200,255,200,100);
    twineLine.draw();
}

void PointCloudTreeSearch::updatePoints(){

    mesh.clear();

    for (std::size_t i = 0; i < visualization_points.size(); ++i)
    {

        if (point_step<=MAX_STEPS){
            visualization_points[i] +=point_steps[i];
        }

        mesh.addVertex(visualization_points[i]);
        mesh.addColor(db->colors[i]);
    }
    if (point_step<=MAX_STEPS){
        point_step++;
    }

}

void PointCloudTreeSearch::updateSearchSpace(const vector<float> search_point, const vector<float> index_weights){

    int num_features = db->feature_values[0].size();
    float epsilon = 0.0001;

    float total_weight = std::accumulate(index_weights.begin(), index_weights.end(), 0.);
//    cout<<total_weight<<endl;
    int active_features = 0;
    for (auto w : index_weights){
        if (w > 0.) active_features ++;
    }

    point_steps.clear();
    for (std::size_t i = 0; i < num_points; ++i){

        float radius = 1.;
        int dim_idx =0;
        float angleY= 0;
        float angleZ =0;

        for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
            //Ignore unweighted values
            if (index_weights[featureIndex] <=0.) continue;

            float diff = search_point[featureIndex]-db->feature_values[i][featureIndex];
            float dist = abs(diff);

            radius += (1. -dist )*index_weights[featureIndex]*50/total_weight;

            if (active_features <2) {
                //ofSeedRandom(featureIndex* i);
                angleY += i;
                angleY += diff*TWO_PI;
                continue;
            }

            if (dim_idx %2 ==0 ) angleY += TWO_PI*dist;
            else angleZ += TWO_PI *dist;
            dim_idx++;
        }

        float x = radius*sin(angleY)*cos(angleZ);
        float y = radius*sin(angleY)*sin(angleZ);
        float z = radius*cos(angleY);

        point_steps.push_back((ofVec3f(x,y,z) - visualization_points[i])/MAX_STEPS);
    }
    point_step =0;
}

vector<int> PointCloudTreeSearch::getSearchResultIndexes(){
    draw_indexes.clear();
    int n_v =0;
    for (std::size_t i = 0; i < search_indexes.size(); ++i)
    {
        float v = search_dists[i];
        draw_indexes.push_back(search_indexes[i]);
        n_v++;
        if (v > threshold_distance){
            break;
        }
    }

    if (draw_indexes.size() <minVideos){
        for (;n_v < minVideos; n_v++){
            draw_indexes.push_back(search_indexes[n_v]);
        }
    }

    return draw_indexes;
}

