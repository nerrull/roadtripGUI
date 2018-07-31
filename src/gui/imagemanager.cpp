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

    imgLoader1.setPath(IMAGE_DATA_BASE_PATH);
    imgLoader2.setPath(SEGNET_DATA_BASE_PATH);
    imgLoader3.setPath(SEGNET_DATA_BASE_PATH);



}

void ImageManager::setLayout(int x, int y, int w, int h){
    this->xOffset = x;
    this->yOffset = y;
    this->width =w;
    this->height =h;
    segnetColourHeight = h/12.;
    segnetColors.setLayout(0, y+h - segnetColourHeight, w, segnetColourHeight);

}

//Todo: maybe make this work with the threadedimageloader
void ImageManager::loadImages(string fileName){
    if (DEV_MODE) return;

    currentFileName = fileName;
    string frameName= fileName + ".png";
    string segnetName = fileName + "_segnet.png";
    string uncertaintyName = fileName + "_uncertainty.png";

    firstFrameImage.setFromPixels(*imgLoader1.getImage(frameName));
    segnetImage.setFromPixels(*imgLoader2.getImage(segnetName));
    heatmapImage.setFromPixels(*imgLoader3.getImage(uncertaintyName));
    loaded = true;

}

void ImageManager::draw(){
   if (DEV_MODE) return;

   ofPushMatrix();
   ofSetColor(255);
   ofTranslate(xOffset,yOffset);
   //ofDrawBitmapString(currentFileName, 15, 15);

   if (!loaded){
       ofPopMatrix();
       return;
   }

   int imageHeight = height - segnetColourHeight;
   if (firstFrameImage.isUsingTexture()){
       firstFrameImage.draw(0, 0, width/2, imageHeight/3);
   }

   if (heatmapImage.isUsingTexture()){
       heatmapImage.draw(width/2,0, width/2, imageHeight/3);
   }

   if (segnetImage.isUsingTexture()){
        segnetImage.draw(0,imageHeight/3, width, 2*imageHeight/3);
   }

   segnetColors.draw();
   ofPopMatrix();
}

void ImageManager::update(){
}


