#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#endif // COMMUNICATION_H
#include "ofMain.h"
#include "ofxOsc.h"

#define PLAYING_RECEIVE_PORT 44445
#define PYTHON_CONTROL_RECEIVE_PORT 44444
#define CONTROL_RECEIVE_PORT 9002
#define SEND_PORT 33333

class CommunicationManager{
private:
    ofxOscSender local_sender;
    ofxOscSender controller_sender;

public:
    ofxOscReceiver receiver_python_controller;
    ofxOscReceiver receiver_controller;
    ofxOscReceiver receiver_playing;
    int outgoing_player_message_counter;
    int outgoing_controller_message_counter;


    CommunicationManager(){
        std::cout << "listening for osc messages on port " << CONTROL_RECEIVE_PORT << " and "<<  PLAYING_RECEIVE_PORT<<"\n";
        receiver_python_controller.setup( PYTHON_CONTROL_RECEIVE_PORT );

        receiver_controller.setup( CONTROL_RECEIVE_PORT );
        receiver_playing.setup( PLAYING_RECEIVE_PORT );

        std::cout << "Sending on " << SEND_PORT << "\n";
        local_sender.setup("localhost",SEND_PORT);
        controller_sender.setup("10.6.54.101", 9001);
        outgoing_player_message_counter =0;
        outgoing_controller_message_counter =0;

    }

    void logMessageCount(){
        ofLogNotice()<<"Sent " <<outgoing_controller_message_counter<< " OSC messages to controller in the last second"<<endl;
        ofLogNotice()<<"Sent " <<outgoing_player_message_counter<< " OSC messages to player in the last second"<<endl;
        outgoing_controller_message_counter =0;
        outgoing_player_message_counter =0;
    }

    void publishVideos(vector<string> v1, bool log){
        outgoing_player_message_counter++;
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
        outgoing_player_message_counter++;
        ofxOscMessage m;
        if (log) ofLogVerbose(ofToString(ofGetElapsedTimef(),3))<<"Sending "<< v1<<"to play now"<<endl ;
        m.setAddress("/PLAY_NOW");
        m.addStringArg(v1);
        local_sender.sendMessage(m);
    }


    void sendLightControl(int knob_index,int value){
        outgoing_controller_message_counter++;
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
        outgoing_player_message_counter++;
        ofxOscMessage m;
        ofLogVerbose(ofToString(ofGetElapsedTimef(),3))<<"Sending speed value" ;

        m.setAddress("/SET_SPEED");
        m.addInt32Arg(speed);
        local_sender.sendMessage(m);
    }
};
