#include "imagemanager.h"

ImageManager::ImageManager()
{
    loaded = false;
    currentFileName = "NONE";

    imgLoader1.setPath(IMAGE_DATA_BASE_PATH);
    imgLoader2.setPath(SEGNET_DATA_BASE_PATH);
    imgLoader3.setPath(SEGNET_DATA_BASE_PATH);



}

//Todo: maybe make this work with the threadedimageloader
void ImageManager::loadImages(string fileName){
    currentFileName = fileName;

    string frameName= fileName + ".png";
    string segnetName = fileName + "_segnet.png";
    string uncertaintyName = fileName + "_heatmap.png";

    firstFrameImage.setFromPixels(*imgLoader1.getImage(frameName));
    segnetImage.setFromPixels(*imgLoader2.getImage(segnetName));
    heatmapImage.setFromPixels(*imgLoader3.getImage(uncertaintyName));
    loaded = true;

}

void ImageManager::draw(int cornerX, int cornerY, int width, int height){
   ofPushMatrix();
   ofSetColor(255);
   ofTranslate(cornerX,cornerY);
   ofDrawBitmapString(currentFileName, 15, 15);

   if (!loaded){
       ofPopMatrix();
       return;
   }

   if (firstFrameImage.isUsingTexture()){
       firstFrameImage.draw(0, 0, width/2, height/2);
   }
   if (segnetImage.isUsingTexture()){
        segnetImage.draw(0,height/2, width/2, height/2);
   }
   if (heatmapImage.isUsingTexture()){
       heatmapImage.draw(width/2,height/2, width/2, height/2);
   }
   ofPopMatrix();
}

void ImageManager::update(){
}


