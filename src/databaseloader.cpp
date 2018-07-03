#include "databaseloader.h"

DatabaseLoader::DatabaseLoader()
{

}

void DatabaseLoader::loadHDF5Data(string database_path){
//    ofSetLogLevel(OF_LOG_VERBOSE);
    hdf5File.open(database_path, true);
    cout << "File '" << database_path << "' has " << hdf5File.getNumGroups() << " groups and " << hdf5File.getNumDataSets() << " datasets" << endl;
    for (int i = 0; i < hdf5File.getNumGroups(); ++i) {
        cout << "  Group " << i << ": " << hdf5File.getGroupName(i) << endl;
    }
    for (int i = 0; i < hdf5File.getNumDataSets(); ++i) {
        cout << "  DataSet " << i << ": " << hdf5File.getDataSetName(i) << endl;
    }

    feature_values_ptr= hdf5File.loadDataSet("feature_values");
    size_t  data_size = feature_values_ptr->getDataSize();

    num_videos = feature_values_ptr->getDimensionSize(0);
    num_features = feature_values_ptr->getDimensionSize(1);
    float *values= new float[num_videos*num_features];
    feature_values_ptr->read(values);
    for( int row =0; row<num_videos; row++){
        float *value_row = new float[num_features];

        std::copy(values +row*num_features,values +(row+1)*num_features,value_row );
        vector<float> v(value_row, value_row +num_features );
        feature_values.push_back(v);
    }


    colors_ptr= hdf5File.loadDataSet("colors");
    int num_colors = colors_ptr->getDimensionSize(0);
    int color_dim = colors_ptr->getDimensionSize(1);
    uint8_t *color_values= new uint8_t[num_colors*color_dim];
    colors_ptr->read(color_values);
    for( int row =0; row<num_colors; row++){
        uint8_t h,s,v;

        h = color_values[row*color_dim];
        s = color_values[row*color_dim+1];
        v = color_values[row*color_dim+2];
        ofColor c = c.fromHsb(h,s,v);
        colors.push_back(c);
    }

    feature_names_ptr= hdf5File.loadDataSet("feature_names");
    size_t  name_size = feature_names_ptr->getDataSize();
    int num_names = feature_names_ptr->getDimensionSize(0);

    uint8_t *characters = new uint8_t[num_names*STRING_LENGTH];
    feature_names_ptr->read(characters);
    char * name = new char[STRING_LENGTH];

    for( int row =0; row<num_names; row++){
        std::copy(characters +row*STRING_LENGTH,characters +(row+1)*STRING_LENGTH,name );
        feature_names.push_back(name);
    }

    video_names_ptr= hdf5File.loadDataSet("video_names");
    num_videos = video_names_ptr->getDimensionSize(0);

    characters = new uint8_t[num_videos*STRING_LENGTH];
    video_names_ptr->read(characters);
    for( int row =0; row<num_videos; row++){
        std::copy(characters +row*STRING_LENGTH,characters +(row+1)*STRING_LENGTH,name );
        video_names.push_back(name);
    }
    hdf5File.close();

}

const vector<vector<float>>* DatabaseLoader::getFeatures(){
    return &feature_values;
}

vector<string>  DatabaseLoader::getFeatureNames(){
    return feature_names;
}

vector<string>   DatabaseLoader::getVideoNames(){
    return video_names;
}

vector<string>   DatabaseLoader::getVideoNamesFromIndexes(vector<int> indexes){
    vector<string>  names;
    for (std::size_t i = 0; i < indexes.size(); i++)
    {
        names.push_back(video_names[indexes[i]]);
    }
    return names;
}

vector<float>  DatabaseLoader::getFeaturesFromName(string name){
    return feature_values[getVideoIndexFromName(name)];
}

vector<float>  DatabaseLoader::getFeaturesFromindex(int index){
    return feature_values[index];
}

int DatabaseLoader::getVideoIndexFromName(string name){
    auto it = std::find(video_names.begin(), video_names.end(), name);
    if (it == video_names.end())
    {
        ofLogError() <<"Received fault video name: "<< name<<endl;
        return 0;
    } else
    {
      auto index = std::distance(video_names.begin(), it);
      return index;
    }
}
