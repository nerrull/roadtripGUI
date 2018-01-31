#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H
#include "ofMain.h"
#include "ofxThreadedImageLoader.h"


class ImageManager
{
public:
    ImageManager();
    string IMAGE_DATA_BASE_PATH = "/media/rice1902/OuterSpace/dataStore/IMAGE/frames_480/";
    string SEGNET_DATA_BASE_PATH = "/media/rice1902/OuterSpace/dataStore/IMAGE/segnet/";
    string currentFileName;
    ofxThreadedImageLoader imgLoader1;
    ofxThreadedImageLoader imgLoader2;
    ofxThreadedImageLoader imgLoader3;

    ofImage firstFrameImage;
    ofImage segnetImage;
    ofImage heatmapImage;
    bool loaded;


    void loadImages(string fileName);
    void draw(int,int,int,int);
    void update();
};

#endif // IMAGEMANAGER_H
