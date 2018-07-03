#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
//    ofSetupOpenGL(1920,1020,OF_FULLSCREEN);			// <-------- setup the GL context
    ofGLFWWindowSettings settings;

    settings.setSize(1920,1080);

    settings.monitor =0;
    settings.windowMode = OF_FULLSCREEN;
    ofCreateWindow(settings);
    //Ultrawide aspect ratio 21:9
//    ofSetupOpenGL(2560/2,1080/2, OF_WINDOW);
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());

}
