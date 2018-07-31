#ifndef MAPFILEPARSER_H
#define MAPFILEPARSER_H
#include "ofMain.h"

class MapFileParser{
public:
    static void parseMapFile(vector<ofVec3f>& boundary, vector<ofIndexType>& indices, string filepath, int maxRank, int subsample){
        ofBuffer b = ofBufferFromFile(filepath);
        bool skiplines = false;
        int skipCounter = 0;
        int n_points =0;
        int subsample_counter= 0;
        int added_counter = 0;
        //Initialize indexes at current value
        int index = boundary.size();

        for (auto line: b.getLines()){
            auto values = ofSplitString(line, " ");
            subsample_counter ++;


            //Skip low rank map features
            if (skiplines){
                skipCounter ++;
                if (skipCounter > n_points){
                    skiplines =false;
                }
                else continue;
            }

            if (values.size()>2){
                subsample_counter =0;
                added_counter = 0;
                ofLogNotice()<<"Info line " <<line<<endl;
                n_points = stoi(values[7]);
                int rank = stoi(values[4]);

                if (rank >maxRank){
                    skipCounter =0;
                    skiplines = true;
                    ofLogNotice()<<"Skipping line " <<line<<endl;
                }
                continue;
            }

            else{
                if (subsample_counter %subsample !=0){
                    continue;
                }
                added_counter++;
                index ++;
                float coord1 = stof(values[0]);
                float coord2 = stof(values[1]);

//              boundary.push_back(ofVec3f(coord2*5, coord1*5,0.));
                boundary.push_back(coordToXYZ(coord1, coord2));
                if (added_counter >2){
                    indices.push_back(index-1);
                    indices.push_back(index-2);
                }
            }
        }



        ofLogNotice()<<"Loaded " <<filepath<<endl;


    }

    static ofVec3f coordToXYZ(float lat, float lon, float x_distorsion = 1.){
        float cosLat = cos(lat * PI / 180.0);
        float sinLat = sin(lat * PI / 180.0);
        float cosLon = cos(lon * PI / 180.0);
        float sinLon = sin(lon * PI / 180.0);
        float rad = 500.0;
        ofVec3f xyz;
        xyz.x = rad * cosLat * cosLon*x_distorsion;
        xyz.y = rad * cosLat * sinLon;
        xyz.z = rad * sinLat;
        return xyz;
    }
    MapFileParser(){



    }
    ~MapFileParser(){

    }

};

#endif // MAPFILEPARSER_H
