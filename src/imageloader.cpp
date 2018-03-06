#include "imageloader.h"

ImageLoader::ImageLoader()
{

}
void ImageLoader::setPath(string path){
    dir= ofDirectory(path);
    dir.allowExt("png");
    dir.listDir();
    loadAllImages();
}

void ImageLoader::loadAllImages(){
    vector<ofFile> files =  dir.getFiles();
    for (int i =0 ; i<files.size(); i++){

        ofPixels p;
        ofLoadImage(p,dir.getPath(i));
        imagePixels.push_back(p);
        string fullname = dir.getName(i);
//        size_t lastindex = fullname.find_last_of(".");
//        string rawname = fullname.substr(0, lastindex);
        imageNames.push_back(fullname);

    }
}

ofPixels* ImageLoader::getRandomImage(){
    int size = imagePixels.size();
    int imageIndex = ofRandom(0,size);
    return &imagePixels[imageIndex];
}

ofPixels* ImageLoader::getImage(string imageName){
    return &imagePixels[getImageIndexFromName(imageName)];
}


int ImageLoader::getImageIndexFromName(string name){
    auto it = std::find(imageNames.begin(), imageNames.end(), name);
    if (it == imageNames.end())
    {
        ofLogError() <<"Received faulty image name: "<< name<<endl;
        return 0;
    } else
    {
      auto index = std::distance(imageNames.begin(), it);
      return index;
    }
}
