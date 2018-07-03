#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H
#include "ofMain.h"
#include "ofxThreadedImageLoader.h"
#include "imageloader.h"

class ImageManager
{
public:
    ImageManager();
    string IMAGE_DATA_BASE_PATH = "/home/nuc/Documents/dataStore/IMAGE/frames_480/";
    string SEGNET_DATA_BASE_PATH = "/home/nuc/Documents/dataStore/IMAGE/segnet/";
    string currentFileName;
    ImageLoader imgLoader1;
    ImageLoader imgLoader2;
    ImageLoader imgLoader3;


    ofImage firstFrameImage;
    ofImage segnetImage;
    ofImage heatmapImage;
    bool loaded;


    void loadImages(string fileName);
    void draw(int,int,int,int);
    void update();
};

#endif // IMAGEMANAGER_H
