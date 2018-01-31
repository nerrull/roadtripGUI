#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    //ofSetupOpenGL(1920,1020,OF_WINDOW);			// <-------- setup the GL context

    //Ultrawide aspect ratio 21:9
    ofSetupOpenGL(2560/2,1080/2, OF_WINDOW);
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());

}
