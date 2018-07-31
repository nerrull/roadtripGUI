#include "worldmapvisual.h"

WorldMapVisual::WorldMapVisual()
{

    string  mapfile = "namer-cil.txt";
    vector<ofVec3f> coastline_lakes;
    vector<ofIndexType> coastline_lakes_indices;

    MapFileParser::parseMapFile( coastline_lakes,coastline_lakes_indices, mapfile, 1,4);

    vector<ofVec3f> borders;
    vector<ofIndexType> borders_indices;

    string  boundaryfile = "namer-pby.txt";
    MapFileParser::parseMapFile( borders,borders_indices, boundaryfile, 1,1);

    string  borderfile = "namer-bdy.txt";
    MapFileParser::parseMapFile( borders,borders_indices, borderfile, 4,1);

    vector<ofVec3f> rivers;
    vector<ofIndexType> river_indices;
    string  riverfile = "namer-riv.txt";
    MapFileParser::parseMapFile( rivers,river_indices, riverfile, 4,15);

    geomesh.setMode(OF_PRIMITIVE_LINES);
    bordermesh.setMode(OF_PRIMITIVE_LINES);
    rivermesh.setMode(OF_PRIMITIVE_LINES);
    videoPointMesh.setMode(OF_PRIMITIVE_POINTS);

    for(auto p: coastline_lakes){
        geomesh.addVertex(p);
        geomesh.addColor(ofFloatColor(0.,0.5,0.5, 0.9));
    }
    geomesh.clearIndices();
    geomesh.addIndices(coastline_lakes_indices);


    for(auto p: borders){
        bordermesh.addVertex(p);
        bordermesh.addColor(ofFloatColor(1.,1.,1., 0.3));
    }
    bordermesh.clearIndices();
    bordermesh.addIndices(borders_indices);


    for(auto p: rivers){
        rivermesh.addVertex(p);
        rivermesh.addColor(ofFloatColor(0.,0.,1., 0.5));
    }
    rivermesh.clearIndices();
    rivermesh.addIndices(river_indices);


}

void WorldMapVisual::setLayout(int x,int y, int w, int h){
    viewMain.x=x;
    viewMain.y = y;
    viewMain.width = w;
    viewMain.height =h;
    cam.setPosition(ofVec3f(0.,264.,0.));
//    cam.lookAt(ofVec3f(0,0,0));
//    cam.setDistance(300);

}


void WorldMapVisual::draw(){
    ofSetColor(255,70);
    cam.begin(viewMain);
    geomesh.draw();
    bordermesh.draw();
//    rivermesh.draw();
    glPointSize(3);
    videoPointMesh.draw();
    cam.end();

}

void WorldMapVisual::setVideoPoints(vector<ofVec2f> coordinates ){
    videoPoints.clear();
    for (auto c: coordinates){
        ofVec3f p = MapFileParser::coordToXYZ(c.x, c.y);
        videoPoints.push_back(p);
        videoPointMesh.addVertex(p);
        videoPointMesh.addColor(ofFloatColor::fromHsb(0, 0.8, 0.8, 1.));
    }
}
