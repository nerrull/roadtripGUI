#include "ofMain.h"
#include "ofApp.h"
#include "ofxJsonSettings.h"

//========================================================================
int main( ){
    Settings::get().load("settings.json");
    ofGLFWWindowSettings settings;
    settings.setSize(1920,1080);
    settings.monitor =Settings::getInt("monitor_index");
    settings.windowMode = Settings::getBool("windowed") ? OF_WINDOW :OF_FULLSCREEN;
    ofCreateWindow(settings);
    //Ultrawide aspect ratio 21:9
//    ofSetupOpenGL(2560/2,1080/2, OF_WINDOW);
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());

}
