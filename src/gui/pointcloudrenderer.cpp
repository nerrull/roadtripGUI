#include "pointcloudrenderer.h"

PointCloudRenderer::PointCloudRenderer()
{
    point_step = MAX_STEPS+1;
    playedLine.setMode(OF_PRIMITIVE_LINE_STRIP);
    connectionMesh.setMode(OF_PRIMITIVE_LINES);
    playingPointMesh.setMode(OF_PRIMITIVE_POINTS);
    activePointMesh.setMode(OF_PRIMITIVE_POINTS);

}



void PointCloudRenderer::setLayout(int x, int y, int w, int h){
    viewMain.x=x;
    viewMain.y = y;
    viewMain.width = w;
    viewMain.height =h;
    cam.setPosition(ofVec3f(0.,0.,100.));
    cam.lookAt(ofVec3f(0,0,0));
    cam.setDistance(100);
}


void PointCloudRenderer::setActiveNodes(vector<int> nodeIndexes){
    draw_indexes = nodeIndexes;
    vector<ofVec3f> activePoints;
    vector<ofFloatColor> c;
    c.push_back(ofFloatColor(colors[playingIndex]));
    activePointMesh.clear();
    for (int i : nodeIndexes){
        if (i ==playingIndex) continue;
        activePoints.push_back(visualization_points[i]);
        c.push_back(colors[i]);
        activePointMesh.addVertex(visualization_points[i]);
        activePointMesh.addColor(colors[i]);
    }

    nodeParticles.updateNodesActivePoints(activePoints,c, visualization_points[playingIndex]);
}


void PointCloudRenderer::setPlayingNode(int index){
    playingIndex = index;
    playingPointMesh.clear();
    playingPointMesh.addVertex(visualization_points[playingIndex]);
    playingPointMesh.addColor(colors[playingIndex]);
}

void PointCloudRenderer::setRotation(bool on){
    rotationOn = on;
}
void PointCloudRenderer::initPoints(vector<ofVec3f> points, vector<ofColor>c){
    mesh.clear();
    point_steps.clear();
    visualization_points.clear();
    mesh.setMode(OF_PRIMITIVE_POINTS);
    colors = c;

    int index=  0;
    for (auto p :points)
    {
        ofVec3f point = p*100;

        //Stretch out horizontal dimension if we're in 2D
        if (!rotationOn) point.y *=2.;

        mesh.addVertex(point);
        mesh.addColor(colors[index]);
        point_steps.push_back(ofVec3f(0.,0.,0.));

        visualization_points.push_back(point);
        index++;
    }

    point_step = MAX_STEPS +1;
}


void PointCloudRenderer::initPoints(int nPoints, vector<ofColor>c){
    mesh.clear();
    point_steps.clear();
    visualization_points.clear();
    mesh.setMode(OF_PRIMITIVE_POINTS);

    float h,s,v;
    float radius,angleY,angleZ;
    float x,y,z;
    colors = c;
    for (std::size_t i = 0; i < nPoints; ++i)
    {
        radius = ofRandom( 0,100);
        angleY = ofRandomf() *PI;
        angleZ= ofRandomf()*PI;

        x = radius*sin(angleY)*cos(angleZ);
        y = radius*sin(angleY)*sin(angleZ);
        z = 0.;//        x = ofRandom(0.,1.);
//        y= ofRandom(0.,1.);
//        z = 0.;


        ofVec3f point(x,y,z);
        mesh.addVertex(point);
        mesh.addColor(colors[i]);
        point_steps.push_back(ofVec3f(0.,0.,0.));
        visualization_points.push_back(point);
    }

    point_step = MAX_STEPS +1;
}


void PointCloudRenderer::update(){
//    updatePoints();

    if (playedPoints.size() >10){
        playedPoints.pop_front();
    }
    nodeParticles.update();
    c_line.update(visualization_points[playingIndex]);

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

void PointCloudRenderer::updatePoints(){

    mesh.clear();
    for (std::size_t i = 0; i < visualization_points.size(); ++i)
    {

        if (point_step<=MAX_STEPS){
            visualization_points[i] +=point_steps[i];
        }

        mesh.addVertex(visualization_points[i]);
        mesh.addColor(colors[i]);
    }
    if (point_step<=MAX_STEPS){
        point_step++;
    }
}


void PointCloudRenderer::draw()
{
    ofVec3f centerPoint = ofVec3f(0.,0.,0.);

    cam.begin(viewMain);
    ofPushMatrix();
    ofRotateZDeg(90.);

    if (rotationOn){
        ofRotateXDeg(0.0137*drawCount);
        ofRotateYDeg(0.0269*drawCount);
    }

    ofNoFill();
    drawCount++;
    //Draw line
    c_line.draw();

    // Draw all of the points.
    ofSetColor(255,70);
    glPointSize(3);
    mesh.draw();

    ofSetColor(255);
    glPointSize(2);
    mesh.draw();

    glPointSize(8);
    ofSetColor(255);
    activePointMesh.draw();

    glPointSize(10);
    ofSetColor(255);
    playingPointMesh.draw();

    ofPopMatrix();
    cam.end();
    ofSetLineWidth(1);
    ofDrawRectangle(viewMain);

}


void PointCloudRenderer::meshFromConnections(vector<NodeConnection> connections){
    connectionMesh.clear();
    vector<ofIndexType> indices;
    int index =0;
    for (NodeConnection conn: connections){

        connectionMesh.addVertex(visualization_points[conn.firstPointIndex]);
        connectionMesh.addVertex(visualization_points[conn.secondPointIndex]);
        indices.push_back(index);
        index++;
        indices.push_back(index);
        index++;
        ofColor c1,c2 ;
        c1 = colors[conn.firstPointIndex];
        c2 = colors[conn.secondPointIndex];
        c1.a = int(125/(conn.depth+1))+60;
        c2.a = int(125./(conn.depth+1))+60;

        connectionMesh.addColor(c1);
        connectionMesh.addColor(c2);

    }
    connectionMesh.addIndices(indices);
}

void PointCloudRenderer::drawLines(){
    for (std::size_t i = 0; i < visualization_points.size(); i++)
    {
        ofSetColor(colors[i].r, colors[i].g, colors[i].b, 4);
        for (std::size_t j = i; j < visualization_points.size(); j++)
        {
            ofDrawLine(visualization_points[i], visualization_points[j]);
        }
    }
}



void PointCloudRenderer::drawCurvyBoy(){
    twineLine.clear();
    for (auto p: visualization_points)
    {
        twineLine.curveTo(p);
    }
    ofSetColor(200,255,200,100);
    twineLine.draw();
}

void setActivePoints(vector<int> activeIndexes){

}


void PointCloudRenderer::updatePointPositionsOld(vector<vector< float >> featureDistances , vector<float> index_weights){
    point_steps.clear();
    vector<float> features;
    float total_weight =0;
    for (auto& n : index_weights) {total_weight += n;}

    for (std::size_t i = 0; i < featureDistances.size(); ++i){
        float radius = 1.;
        int dim_idx =0;
        float angleY= 0;
        float angleZ =0;

        features= featureDistances[i];
        int numActiveFeatures = features.size();

        for (int featureIndex=0; featureIndex < features.size(); featureIndex++ ){
            //Ignore unweighted values
            float dist =features[featureIndex];
            radius += (1. -dist )*index_weights[featureIndex]*50/total_weight;
            if (numActiveFeatures <2) {
                //ofSeedRandom(featureIndex* i);
                angleY += i;
                //angleY += diff*TWO_PI;
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


void PointCloudRenderer::updatePointPositions(vector<vector< float >> featureDistances , vector<float> index_weights){
    point_steps.clear();
    vector<float> features;
    float total_weight =0;
    for (auto& n : index_weights) {total_weight += n;}

    for (std::size_t i = 0; i < featureDistances.size(); ++i){
        float height = 0.;
        int dim_idx =0;

        features= featureDistances[i];
        int numActiveFeatures = features.size();

        for (int featureIndex=0; featureIndex < features.size(); featureIndex++ ){
            //Ignore unweighted values
            float dist =features[featureIndex];
            height += (1. -dist )*index_weights[featureIndex]*50/total_weight;


            dim_idx++;
        }

        float x = visualization_points[i].x;
        float y = visualization_points[i].y;
        float z = height;

        point_steps.push_back((ofVec3f(x,y,z) - visualization_points[i])/MAX_STEPS);
    }
    point_step =0;
}

