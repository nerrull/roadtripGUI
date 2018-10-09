#include "imagemanager.h"

ImageManager::ImageManager()
{
    loaded = false;
    currentFileName = "NONE";
    Settings::get().load("settings.json");
    IMAGE_DATA_BASE_PATH = Settings::getString("frames_path");
    SEGNET_DATA_BASE_PATH = Settings::getString("segnet_path");
    DEV_MODE = Settings::getBool("dev_mode");

    if (DEV_MODE) return;

    frameLoader.setPath(IMAGE_DATA_BASE_PATH);
    segnetLoader.setPath(SEGNET_DATA_BASE_PATH);
}

void ImageManager::setLayout(int x, int y, int w, int h){
    this->xOffset = x;
    this->yOffset = y;
    this->width =w;
    this->height =h;
    segnetColourHeight = h/24.;
    segnetColors.setLayout(xOffset, y+height -segnetColourHeight , w, segnetColourHeight);

}

//Todo: maybe make this work with the threadedimageloader
void ImageManager::loadImages(string fileName){
    if (DEV_MODE) return;

    //If already laoded, continue
    if (fileName.compare( currentFileName) ==0){
         return;
    }

    currentFileName = fileName;

    string frameName= fileName + ".png";
    string segnetName = fileName + "_segnet.png";
    string uncertaintyName = fileName + "_uncertainty.png";

    firstFrameImage.setFromPixels(*frameLoader.getImage(frameName));
    segnetImage.setFromPixels(*segnetLoader.getImage(segnetName));
    heatmapImage.setFromPixels(*segnetLoader.getImage(uncertaintyName));
    loaded = true;

}

void ImageManager::draw(){
   segnetColors.draw();

   if (DEV_MODE) return;
   ofPushMatrix();
   ofPushStyle();
   ofSetColor(255);
   ofTranslate(xOffset,yOffset);
   //ofDrawBitmapString(currentFileName, 15, 15);
   ofSetLineWidth(2);
   ofNoFill();

   if (!loaded){
       ofPopStyle();
       ofPopMatrix();
       return;
   }

   int imageHeight = height ;
   if (firstFrameImage.isUsingTexture()){
       firstFrameImage.draw(0, 0, width/2, imageHeight/3);
       ofDrawRectangle(0, 0, width/2, imageHeight/3);
   }

   if (heatmapImage.isUsingTexture()){
       heatmapImage.draw(width/2,0, width/2, imageHeight/3);
       ofDrawRectangle(width/2,0, width/2, imageHeight/3);
   }

   if (segnetImage.isUsingTexture()){
       int segHeight = 2*imageHeight/3 -segnetColourHeight;
       segnetImage.draw(0,imageHeight/3, width, segHeight);
       ofDrawRectangle(0,imageHeight/3, width, segHeight);
   }

   ofPopStyle();
   ofPopMatrix();
}

void ImageManager::update(){
}

ofColor ImageManager::getSegnetColor(string segnetFeature){
    return segnetColors.colorMap[segnetFeature];
}

pair<ofColor, ofColor> ImageManager::getSegnetColorPair(string segnetFeature){
    return segnetColors.colorPairMap[segnetFeature];
}


