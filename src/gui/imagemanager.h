#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H
#include "ofMain.h"
#include "ofxThreadedImageLoader.h"
#include "ofxJsonSettings.h"
#include "util/imageloader.h"
#include "segnetcolourinspector.h"
class ImageManager
{
public:
    ImageManager();

    string IMAGE_DATA_BASE_PATH;
    string SEGNET_DATA_BASE_PATH;
    string currentFileName;

    ImageLoader frameLoader;
    ImageLoader segnetLoader;
    ImageLoader imgLoader3;


    ofImage firstFrameImage;
    ofImage segnetImage;
    ofImage heatmapImage;

    SegnetColourInspector segnetColors;
    ofColor getSegnetColor(string segnetFeature);
    pair<ofColor, ofColor> getSegnetColorPair(string segnetFeature);



    //Loading images takes too long so we disable them in dev mode
    bool DEV_MODE;
    bool loaded;
    int xOffset, yOffset, width, height,segnetColourHeight;

    void loadImages(string fileName);
    void draw();
    void setLayout(int x, int y, int w, int h);

    void update();
};

#endif // IMAGEMANAGER_H
