#include "pointcloudtreesearch.h"

PointCloudTreeSearch::PointCloudTreeSearch(DatabaseLoader *database):  hash(points)
{
    db = database;
    search_radius = 5.;
    max_search_radius = 20.;
    point_step = MAX_STEPS+1;

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
    playedPoints.push_back(i);
    if (playedPoints.size() >10){
        playedPoints.pop_front();
    }
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

    }
    point_step = MAX_STEPS +1;
    hash.buildIndex();
}

void PointCloudTreeSearch::getNearestPoints()
{
    // Random walk.
    uint64_t time = ofGetElapsedTimeMillis();

    searchResults.clear();

    // An estimate of the number of points we are expecting to find.

    // Estimate the volume of our seach envelope as a cube.
    // A cube already overestimates a spherical search space.
    float approxCubicSearchBoxSize = (search_radius * 2 * search_radius * 2 * search_radius * 2);

    // Calculate the volume of our total search space as a cube.
    float approxCubicSearchSpaceSize = (ofGetWidth() * ofGetHeight() * 2 * 500);

    // Determine the percentage of the total search space we expect to capture.
    float approxPercentageOfTotalPixels = approxCubicSearchBoxSize / approxCubicSearchSpaceSize;

    // Assuming an uniform distribution of points in our search space,
    // get a percentage of them.
    std::size_t approximateNumPointsToFind = points.size() * approxPercentageOfTotalPixels;
    searchResults.resize(approximateNumPointsToFind);

    hash.findPointsWithinRadius(ofVec3f(0.,0.,0.), search_radius, searchResults);

    if (searchResults.size()< targetNumberOfPoints){
        search_radius += search_radius/10.;
        search_radius = MIN(search_radius, max_search_radius);
    }

    else if (searchResults.size()> targetNumberOfPoints){
        search_radius -= search_radius/5.;
        search_radius = MAX(search_radius, 1);
    }

}

void PointCloudTreeSearch::update(){
    updatePoints();
    if (playedPoints.size() >1){
        line.clear();
        line.addVertex(points[playedPoints[0]]);
        for (int pointIndex=1; pointIndex < playedPoints.size(); pointIndex ++){
             line.curveTo(points[playedPoints[pointIndex]]);
        }
    }
}


void PointCloudTreeSearch::draw()
{

    cam.begin(viewMain);
    ofPushMatrix();
    ofRotateY(0.5*drawCount);

    ofNoFill();
    ofSetColor(255);
    drawCount++;

    // Draw all of the points.
    mesh.draw();

    ofFill();
    ofSetColor(255, 255, 0, 80);

    for (std::size_t i = 0; i < searchResults.size(); ++i)
    {
        float normalizedDistance = ofMap(searchResults[i].second, search_radius * search_radius, 0, 0, 1, true);

        ofSetColor(255, 255, 0, normalizedDistance * 127);

        ofDrawSphere(points[searchResults[i].first], 0.1);
    }


    ofSetColor(255, 0, 0);
    ofDrawIcoSphere(points[playingIndex], 0.2);
    ofSetColor(255);
    line.draw();

    ofBoxPrimitive s ;
    s.set(search_radius, search_radius,search_radius);
    s.drawWireframe();
    drawLines();
    ofPopMatrix();
    cam.end();

}

void PointCloudTreeSearch::drawLines(){
    for (std::size_t i = 0; i < num_points; i++)
    {
        ofSetColor(db->colors[i].r, db->colors[i].g, db->colors[i].b, 5);
        for (std::size_t j = i; j < num_points; j++)
        {
            ofDrawLine(points[i], points[j]);

        }
    }
}

void PointCloudTreeSearch::updatePoints(){
    if (point_step>MAX_STEPS) return;
    point_step ++;
    mesh.clear();
    for (std::size_t i = 0; i < num_points; ++i)
    {
        points[i] +=point_steps[i];
        mesh.addVertex(points[i]);
        mesh.addColor(db->colors[i]);
    }
    hash.buildIndex();

}



void PointCloudTreeSearch::updateActiveCoordinates(float *desiredValues, bool* activeIndexes){
    int num_features = db->feature_values[0].size();
    float num_active =0.;
    for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
        if (activeIndexes[featureIndex]){
            num_active +=1.0;
        }
    }
    if (num_active ==0.){
        num_active =1.;
    }
    mesh.clear();
    for (std::size_t i = 0; i < num_points; ++i){
        float radius = 0;

        for (int featureIndex=0; featureIndex < num_features; featureIndex++ ){
            if (!activeIndexes[featureIndex]) continue;
            float dist = abs(desiredValues[featureIndex] -db->feature_values[i][featureIndex]);
            radius +=dist;
        }
        radius = radius*100/num_active;

        float angleY = db->feature_values[i][18]*TWO_PI;
        float angleZ= db->feature_values[i][19]*TWO_PI;

        float x = radius*sin(angleY)*cos(angleZ);
        float y = radius*sin(angleY)*sin(angleZ);
        float z = radius*cos(angleY);

        ofVec3f point(x,y,z);
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

vector<int> PointCloudTreeSearch::getSearchResultIndexes(){
    vector<int> indexes;

    for (std::size_t i = 0; i < searchResults.size(); ++i)
    {
        indexes.push_back(searchResults[i].first);
    }
    return indexes;
}

