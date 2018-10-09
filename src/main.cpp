#include "ofMain.h"
#include "ofApp.h"
#include "ofxJsonSettings.h"

//========================================================================
int main( ){

    Settings::get().load("settings.json");
    ofGLFWWindowSettings settings;
//    settings.setGLVersion(,0);
    settings.setSize(Settings::getInt("monitor_width"), Settings::getInt("monitor_height"));
    settings.monitor =Settings::getInt("monitor_index");
    settings.windowMode = Settings::getBool("windowed") ? OF_WINDOW :OF_FULLSCREEN;
    auto window = ofCreateWindow(settings);

    //Ultrawide aspect ratio 21:9
//    ofSetupOpenGL(2560/2,1080/2, OF_WINDOW);
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
}
