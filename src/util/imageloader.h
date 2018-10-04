#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include "ofMain.h"

class ImageLoader
{
public:
    ImageLoader();
    map<string, ofPixels> images;
    vector<ofPixels> imagePixels;
    vector<string> imageNames;

    ofDirectory dir;

    void setPath(string path);
    void loadAllImages();
    ofPixels* getRandomImage();
    ofPixels* getImage(string imageName);

    int getImageIndexFromName(string name);


};

#endif // IMAGELOADER_H
