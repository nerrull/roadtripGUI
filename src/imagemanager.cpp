#include "imagemanager.h"

ImageManager::ImageManager()
{
    loaded = false;
    currentFileName = "NONE";


}

//Todo: maybe make this work with the threadedimageloader
void ImageManager::loadImages(string fileName){
    currentFileName = fileName;
    string framePath = IMAGE_DATA_BASE_PATH+fileName + ".png";
    string segnetPath = SEGNET_DATA_BASE_PATH+fileName + "_segnet.png";
    string uncertaintyPath = SEGNET_DATA_BASE_PATH+fileName + "_heatmap.png";

//    imgLoader1.loadFromDisk(firstFrameImage,framePath );
//    imgLoader2.loadFromDisk(segnetImage,segnetPath );
//    imgLoader3.loadFromDisk(heatmapImage,uncertaintyPath );
    firstFrameImage.load(framePath);
    segnetImage.load(segnetPath);
    heatmapImage.load(uncertaintyPath);
    loaded = true;

}

void ImageManager::draw(int cornerX, int cornerY, int width, int height){
   ofPushMatrix();
   ofSetColor(255);
   ofTranslate(cornerX,cornerY);
   ofDrawBitmapString(currentFileName, width/2 +15, 15);

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


