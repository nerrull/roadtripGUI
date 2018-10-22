#include "featuresearch.h"

bool FeatureSearch::compareVideoItems(const VideoItem& first, const VideoItem& second){
    float d1 = 0.;
    float d2 = 0.;
    for (int featureIndex = 0; featureIndex< targetFeatures.size(); featureIndex++){
        if (featureWeights[featureIndex]<0.1){
            continue;
        }
        d1 += abs(targetFeatures[featureIndex] - (*dbl->getFeatures())[first.videoIndex][featureIndex] )*featureWeights[featureIndex];
        d2 += abs(targetFeatures[featureIndex] - (*dbl->getFeatures())[second.videoIndex][featureIndex])*featureWeights[featureIndex];
    }
    return d1 <d2;
}



float FeatureSearch::getDistance(const VideoItem video){
    float d1 =0;
    for (int featureIndex = 0; featureIndex< targetFeatures.size(); featureIndex++){
        d1 +=  pow(targetFeatures[featureIndex] -(*dbl->getFeatures())[video.videoIndex][featureIndex], 2.)*featureWeights[featureIndex];
    }
    return sqrt(d1);
}

FeatureSearch::FeatureSearch(DatabaseLoader *db)
{
    dbl = db;
    threshold_distance =0.05;
    for(int i =0; i< db->feature_values.size(); i++){
        videos.push_back(VideoItem(db->video_names[i], i));
    }
}

void FeatureSearch::getKNN(vector<float> search_point, vector<float> search_weights, vector<float> &search_distances)
{
    float t = ofGetElapsedTimef();
    searchDistances.clear();
    search_distances.clear();
    targetFeatures = vector<float>(search_point);
    featureWeights = vector<float>(search_weights);
    std::sort (videos.begin(), videos.end(), [this](VideoItem l, VideoItem r) {return compareVideoItems(l, r); });

    for (int i =0; i< numSearchPoints;i++){
//        searchDistances.  push_back(0.5);
        searchDistances.push_back( getDistance(videos[i]));
        search_distances.push_back( searchDistances[i]);
    }
//    ofLogError( ofToString(ofGetElapsedTimef() -t)) <<"Time for KNN search with FeatureSearch" ;
}

vector<int> FeatureSearch::getSearchResultsDistance(int minVideos, bool shuffle, int & numWithinRange){
    nearest_videos.clear();
    int n_v =0;
    for (std::size_t i = 0; i < numSearchPoints; i++)
    {
        float v =searchDistances[i];
        if (v > threshold_distance){
            break;
        }
        nearest_videos.push_back(videos[i].videoIndex);
        n_v++;

    }
    numWithinRange = int (nearest_videos.size());
    if (shuffle){
        std::shuffle(std::begin(nearest_videos), std::end(nearest_videos), rng);
    }

    if (nearest_videos.size() <minVideos){
        for (;n_v < minVideos; n_v++){
            nearest_videos.push_back(videos[n_v].videoIndex);
        }
    }

    return nearest_videos;

}

