#include "pointcloudrenderer.h"

PointCloudRenderer::PointCloudRenderer()
{
    point_step = MAX_STEPS+1;
    connectionMesh.setMode(OF_PRIMITIVE_LINES);
    playingPointMesh.setMode(OF_PRIMITIVE_POINTS);
    activePointMesh.setMode(OF_PRIMITIVE_POINTS);

    mesh.setUsage(GL_DYNAMIC_DRAW);
    mesh.setMode(OF_PRIMITIVE_POINTS);

    if(ofIsGLProgrammableRenderer()){
        billboardShader.load("shadersGL3/Billboard");
    }else{
        billboardShader.load("shadersGL2/Billboard");
    }
    ofDisableArbTex();
    texture.load("halo.png");
    minPointSize =12;
}



void PointCloudRenderer::setLayout(int x, int y, int w, int h){
    viewMain.x = x+2;
    viewMain.y = y+2;
    viewMain.width = w-3;
    viewMain.height =h-3;
    cam.setPosition(ofVec3f(0.,0.,100.));
    cam.lookAt(ofVec3f(0,0,0));
    cam.setDistance(100);
}


void PointCloudRenderer::setActiveNodes(vector<int> nodeIndexes){
    activePointIndexes = nodeIndexes;
}

void PointCloudRenderer::setPlayingNode(int index){
    playingIndex = index;
}

void PointCloudRenderer::setRotation(bool on){
    rotationOn = on;
}
void PointCloudRenderer::initPoints(vector<ofVec3f> points, vector<ofColor>c){
    mesh.clear();
    point_steps.clear();
    point_sizes.clear();
    visualization_points.clear();
    mesh.setMode(OF_PRIMITIVE_POINTS);
    colors = c;

    int index=  0;
    mesh.getVertices().resize(points.size());
    mesh.getColors().resize(points.size());
    mesh.getNormals().resize(points.size());

    for (auto p :points)
    {
        ofVec3f point = p*100;

        //Stretch out horizontal dimension if we're in 2D
        if (!rotationOn) point.y *=2.;
        mesh.getVertices()[index]= point;
        mesh.getColors()[index]= colors[index];

        point_steps.push_back(ofVec3f(0.,0.,0.));
        visualization_points.push_back(point);
        point_sizes.push_back(1.);
        mesh.setNormal(index,ofVec3f(12 + point_sizes[index]));
        index++;
    }

    point_step = MAX_STEPS +1;
}



void PointCloudRenderer::update(){
//    updatePoints();
//    nodeParticles.update();
    c_line.update(visualization_points[playingIndex]);
    float t = ofGetElapsedTimef() *5;
    for (int i = 0; i<point_sizes.size();i++){
        mesh.setNormal(i,ofVec3f(minPointSize));
    }
    for (int i:activePointIndexes){
        mesh.setNormal(playingIndex,ofVec3f(minPointSize +20*sin(t)));
    }
    mesh.setNormal(playingIndex,ofVec3f(minPointSize+20 + 20*sin(t)));


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

        mesh.setNormal(i,ofVec3f(12 + point_sizes[i]));
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

    billboardShader.begin();
    ofEnablePointSprites(); // not needed for GL3/4
    texture.getTexture().bind();
    mesh.draw();
    texture.getTexture().unbind();
    ofDisablePointSprites(); // not needed for GL3/4

    billboardShader.end();

    c_line.draw();


    ofPopMatrix();
    cam.end();
    ofSetLineWidth(2);
    ofDrawRectangle(viewMain.x -1, viewMain.y-2, viewMain.width+1, viewMain.height+3);

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

