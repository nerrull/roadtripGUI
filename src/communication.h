#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#endif // COMMUNICATION_H
#include "ofMain.h"
#include "ofxOsc.h"
#include "ofApp.h"

#define PLAYING_RECEIVE_PORT 44445
#define PYTHON_CONTROL_RECEIVE_PORT 44444
#define CONTROL_RECEIVE_PORT 9002
#define SEND_PORT 33333

class CommunicationManager{


public:
    CommunicationManager(){
        std::cout << "listening for osc messages on port " << CONTROL_RECEIVE_PORT << " and "<<  PLAYING_RECEIVE_PORT<<"\n";
        receiver_python_controller.setup( PYTHON_CONTROL_RECEIVE_PORT );

        receiver_controller.setup( CONTROL_RECEIVE_PORT );
        receiver_playing.setup( PLAYING_RECEIVE_PORT );

        std::cout << "Sending on " << SEND_PORT << "\n";
        local_sender.setup("localhost",SEND_PORT);
        controller_sender.setup("10.6.54.101", 9001);
    }




    void publishVideos(vector<string> v1, bool log){
        ofxOscMessage m;
        if (log) ofLogVerbose(ofToString(ofGetElapsedTimef(),3))<<"Sending batch of length: "<<v1.size() ;

        m.setAddress("/VIDEO_NAMES");
        m.addInt32Arg(v1.size());
        for (std::size_t i = 0; i < v1.size(); i++){
            m.addStringArg(v1[i]);

            if (log) ofLogVerbose(ofToString(ofGetElapsedTimef(),3))<<v1[i];
        }
        local_sender.sendMessage(m);
    }

    void publishVideoNow(string v1, bool log){
        ofxOscMessage m;
        if (log) ofLogVerbose(ofToString(ofGetElapsedTimef(),3))<<"Sending "<< v1<<"to play now"<<endl ;
        m.setAddress("/PLAY_NOW");
        m.addStringArg(v1);
        local_sender.sendMessage(m);
    }


    void sendLightControl(int knob_index,int value){
        ofxOscMessage m;
        value = CLAMP(value, 0, 4095);
        m.setAddress("/RGB");
        m.addInt32Arg(knob_index);
        m.addInt32Arg(value);
        m.addInt32Arg(value);
        m.addInt32Arg(value);
        //SUPER IMPORTANT TO SET BUNDLE TO FALSE FOR ARDUINO
        ofLogVerbose(ofToString(ofGetElapsedTimef(),3)) <<"Sending brightness" <<value<< "to knob" << knob_index <<endl;
        controller_sender.sendMessage(m, false);
    }

    void publishSpeed(int speed){
        ofxOscMessage m;
        ofLogVerbose(ofToString(ofGetElapsedTimef(),3))<<"Sending speed value" ;

        m.setAddress("/SET_SPEED");
        m.addInt32Arg(speed);
        local_sender.sendMessage(m);
    }





    ofxOscReceiver receiver_controller;
    ofxOscReceiver receiver_playing;
private:
    ofxOscReceiver receiver_python_controller;
    ofxOscSender local_sender;
    ofxOscSender controller_sender;

    //These are only sent by the osc controller simulater
//    void handlePythonMessages(){
//        // check for waiting messages
//        while( receiver_python_controller.hasWaitingMessages() )
//        {
//            // get the next message
//            ofxOscMessage m;
//            receiver_python_controller.getNextMessage(m );

//            // check for mouse button message
//            if ( m.getAddress().compare( string("/FEATURE_DIFFS") ) == 0 )
//            {
//                // the single argument is a string
//                for (int i =0; i<23;i++){
//                    float diff =  m.getArgAsFloat(i);
//                    if (abs(diff) >0){
//                        desiredFeatureValues[i] =CLAMP(desiredFeatureValues[i] +diff, 0., 1.);
//                        inactiveCounter[i] = 0;
//                        featureWeights[i]=1.0;

//                        desireChanged=true;
//                        activityTimer = 0;

//                        input_activity = true;
//                    }
//                }

//                ofLogError(ofToString(ofGetElapsedTimef(),3)) << "FEATURE_DIFFS Message received : " ;


//            }

//            if ( m.getAddress().compare( string("/SET_SPEED") ) == 0 )
//            {

//                speedSetting=  m.getArgAsInt(0);
//                speedChanged =true;
//              }

//            else if ( m.getAddress().compare( string("/FEATURE_VALUES") ) == 0 )
//            {
//                // the single argument is a string

//                for (int i =0; i<24;i++){
//                    featureValues[i] = m.getArgAsFloat(i);
//                }
//                string fileName = m.getArgAsString(24);
//                ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Message received : " ;
//                ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Values : " << featureValues[0]<<featureValues[23] ;


//            }

//            else if ( m.getAddress().compare( string("/FEATURE_NAMES") ) == 0 )
//            {
//                // the single argument is a string
//                featureNames.clear();
//                for (int i =0; i<24;i++){
//                    featureNames.push_back(m.getArgAsString(i));
//                }

//                ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Feature names : " ;
//                ofLogError(ofToString(ofGetElapsedTimef(),3)) << "Ex : " << featureNames[0]<<" to " <<featureNames[23] ;
//            }
//            else
//            {
//                ofLogError()<<  m.getAddress();
//            }
//        }

//    }


};
