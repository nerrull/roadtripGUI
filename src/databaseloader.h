#ifndef DATABASELOADER_H
#define DATABASELOADER_H

#include "ofMain.h"
#include "ofxHDF5.h"

#define STRING_LENGTH 40

class DatabaseLoader
{
public:
    DatabaseLoader();
    void loadHDF5Data(string);
    const vector<vector<float>>* getFeatures();

    vector<string>  getFeatureNames();
    vector<string>  getVideoNames();
    vector<string>  getVideoNamesFromIndexes(vector<int> index);


    vector<float> getFeaturesFromName(string name);
    vector<float> getFeaturesFromindex(int index);
    int  getVideoIndexFromName(string name);

    vector< string> feature_names;
    vector< string> video_names;
    vector<ofColor> colors;
    vector< vector<float> > feature_values;

private:
    ofxHDF5DataSetPtr feature_values_ptr;
    ofxHDF5DataSetPtr feature_names_ptr;
    ofxHDF5DataSetPtr video_names_ptr;
    ofxHDF5DataSetPtr colors_ptr;

    int num_videos;
    int num_features;
    ofxHDF5File hdf5File;



};

#endif // DATABASELOADER_H
