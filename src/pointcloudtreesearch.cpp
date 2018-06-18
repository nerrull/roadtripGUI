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
    cam.setDistance(100);
}

void PointCloudTreeSearch::setPlayingIndex(int i){
    playingIndex= i;

    ofVec3f p = points[i];


    playedPoints.push_back(p);

}

void PointCloudTreeSearch::initPoints(){

    num_points= db->feature_values.size();
    mesh.setMode(OF_PRIMITIVE_POINTS);

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
        points.push_back(point);
        mesh.addVertex(point);
        mesh.addColor(c);
        point_steps.push_back(ofVec3f(0.,0.,0.));
        search_points.push_back(point);


    }
    point_step = MAX_STEPS +1;
}

void PointCloudTreeSearch::getNearestPoints()
{
//    // Random walk.
//    uint64_t time = ofGetElapsedTimeMillis();

      ofVec3f centerPoint = ofVec3f(0.,0.,0.);
      vector<double> search_point;
      if (kdTree.getSamples().size()>0){
        for (int i =0; i < kdTree.getSamples()[0].size(); i++){
            search_point.push_back(0.);
        }
      }

      kdTree.getKNN(search_point, targetNumberOfPoints, search_indexes, search_dists);
}

void PointCloudTreeSearch::update(){
    updatePoints();

    if (playedPoints.size() >10){
        playedPoints.pop_front();
    }

    if (playedPoints.size() >1){
        line.clear();
        playedLine.clear();
        ofVec3f p = playedPoints[0];
        line.addVertex(p);

        for (int pointIndex=1; pointIndex < playedPoints.size(); pointIndex ++){
            p = playedPoints[pointIndex];
            line.curveTo(p);

        }

    }
}

void PointCloudTreeSearch::draw()
{
    ofVec3f centerPoint = ofVec3f(0.,0.,0.);

//    cam.lookAt(centerPoint);
//    cam.setTarget(centerPoint);
//    cam.setDistance(1000);

    cam.begin(viewMain);
    ofPushMatrix();
    //ofRotateY(0.5*drawCount);

    ofNoFill();
    ofSetColor(255);
    drawCount++;

    // Draw all of the points.
    mesh.draw();

    ofFill();
    ofSetColor(255, 255, 0, 80);

    for (std::size_t i = 0; i < search_indexes.size(); ++i)
    {
        //float normalizedDistance = ofMap(searchResults[i].second, search_radius * search_radius, 0, 0, 1, true);
        //ofSetColor(255, 255, 0, normalizedDistance * 127);
        ofDrawSphere(search_points[search_indexes[i]], .5);
    }


    ofSetColor(255, 0, 0);
    ofDrawIcoSphere(search_points[playingIndex], 1.);
    ofSetColor(255);

    //line.draw();
    //playedLine.draw();



//    ofSpherePrimitive s ;
//    s.setPosition(centerPoint);
//    s.setRadius(search_radius);
//    s.drawWireframe();

    //drawLines();
    //drawCurvyBoy();
    ofPopMatrix();
    cam.end();

}

void PointCloudTreeSearch::drawLines(){
    for (std::size_t i = 0; i < num_points; i++)
    {
        ofSetColor(db->colors[i].r, db->colors[i].g, db->colors[i].b, 4);
        for (std::size_t j = i; j < num_points; j++)
        {
            ofDrawLine(points[i], points[j]);
        }
    }
}



void PointCloudTreeSearch::drawCurvyBoy(){
    twineLine.clear();
    for (std::size_t i = 0; i < num_points; i++)
    {
        twineLine.curveTo(points[i]);
    }
    ofSetColor(200,255,200,100);
    twineLine.draw();
}

void PointCloudTreeSearch::updatePoints(){

    mesh.clear();
    for (std::size_t i = 0; i < num_points; ++i)
    {
        mesh.addVertex(search_points[i]);
        mesh.addColor(db->colors[i]);
    }
}



void PointCloudTreeSearch::updateActiveCoordinates(float *desiredValues, bool* activeIndexes){

    ofVec3f centerPoint = ofVec3f(0.,0.,0.);

    int num_features = db->feature_values[0].size();
    float num_active =1.;
    for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
        if (activeIndexes[featureIndex]){
            num_active +=1.0;
        }
    }

    mesh.clear();
    float epsilon = 0.01;
    for (std::size_t i = 0; i < num_points; ++i){
        float radius = 1.;
        float phase = 0.;


        for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
            if (!activeIndexes[featureIndex]) continue;
            float dist = abs(desiredValues[featureIndex] -db->feature_values[i][featureIndex]);
            radius += (dist +epsilon);
            phase += dist-0.5;
        }
        radius = radius*100/num_active;

//        float angleY = db->feature_values[i][18]*TWO_PI;
//        float angleZ= db->feature_values[i][19]*TWO_PI;
        float angleY = phase*TWO_PI*2;
        float angleZ= phase*TWO_PI*2;

        float x = radius*sin(angleY)*cos(angleZ);
        float y = radius*sin(angleY)*sin(angleZ);
        float z = radius*cos(angleY);

        ofVec3f point(x,y,z);
        point= point +centerPoint;
        point_steps[i]= (point - points[i])/((float)MAX_STEPS);
    }
    point_step = 0;

}



void PointCloudTreeSearch::updateCoordinates(){
    int num_features = db->feature_values[0].size();
    mesh.clear();
    for (std::size_t i = 0; i < num_points; ++i)
    {
        float radius = (1-db->feature_values[i][ofRandom(0,num_features)]);
        float angleY = db->feature_values[i][ofRandom(0,num_features)]*TWO_PI;
        float angleZ= db->feature_values[i][ofRandom(0,num_features)]*TWO_PI;

        float x = radius*sin(angleY)*cos(angleZ);
        float y = radius*sin(angleY)*sin(angleZ);
        float z = radius*cos(angleY);

        ofVec3f point(x,y,z);
        point_steps[i]= (point - points[i])/((float)MAX_STEPS);
    }
    point_step = 0;


}



void PointCloudTreeSearch::updateSearchSpace(float * desiredValues, bool* activeIndexes){

    ofVec3f centerPoint = ofVec3f(0.,0.,0.);

    int num_features = db->feature_values[0].size();
    int num_active =1;
    for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
        if (activeIndexes[featureIndex]){
            num_active +=1;
        }
    }

    float epsilon = 0.01;
    search_points.clear();
    kdTree.clear();
    for (std::size_t i = 0; i < num_points; ++i){
        ofVec3f d(epsilon,epsilon,epsilon);
        vector<double> kd_sample;

        float radius = 1.;
        int dim_idx =0;
        float angleY, angleZ;
        for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
            if (!activeIndexes[featureIndex]) continue;
            float dist = abs(desiredValues[featureIndex] -db->feature_values[i][featureIndex]);
            radius += (1. -dist )*50/num_active;

            if (dim_idx %2 ==0 ) angleY += TWO_PI*dist/10;

            else angleZ += TWO_PI *dist/30;

            dim_idx++;
            kd_sample.push_back(dist);
        }

        float x = radius*sin(angleY)*cos(angleZ);
        float y = radius*sin(angleY)*sin(angleZ);
        float z = radius*cos(angleY);


        search_points.push_back(ofVec3f(x,y,z));
        kdTree.addPoint(kd_sample);
    }
   kdTree.constructKDTree();
}

vector<int> PointCloudTreeSearch::getSearchResultIndexes(){
    vector<int> indexes;

    for (std::size_t i = 0; i < search_indexes.size(); ++i)
    {
        indexes.push_back(search_indexes[i]);
    }
    return indexes;
}

