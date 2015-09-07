#include "ofxMouseTrap.h"


//--------------------------------------------------------------
void ofxMouseTrap::recordStart() {
    
}


//--------------------------------------------------------------
void ofxMouseTrap::recordStop() {
    //
}

//--------------------------------------------------------------
bool ofxMouseTrap::load(string filename){
    ofFile fileToRead(ofToDataPath(filename));
    if(!fileToRead.exists()) {
        ofLog(ofLogLevel::OF_LOG_FATAL_ERROR, filename + " does not exist");
        return false;
    }
    
    ofxXmlSettings xml;
    
    xml.load(filename);
    numPaths = xml.getNumTags("path");
    xml.pushTag("path", 0);
//    int numItems = xml.getNumTags("item");
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Got " + ofToString(numPaths) + " paths");
    
    for(int i=0; i<numPaths; i++) {
        xml.pushTag("path", i);
        int numItems = xml.getNumTags("item");
        vector <MouseEvent> mv;
        for(int j=0; j<numItems; j++) {
            MouseEvent m;
            m.x = xml.getAttribute("item", "x", -1, j);
            m.y = xml.getAttribute("item", "y", -1, j);
            m.time = xml.getAttribute("item", "time", -1, j);
            mv.push_back(m);
        }
        paths.push_back(mv);
    }
    
    return true;
}

//--------------------------------------------------------------
int ofxMouseTrap::getNumPaths(){
    return numPaths;
}

//--------------------------------------------------------------
int ofxMouseTrap::getNumItems(int pathID){
    //
}

//--------------------------------------------------------------
void ofxMouseTrap::play(){
    //
}

//--------------------------------------------------------------
void ofxMouseTrap::stop(){
    //
}

//--------------------------------------------------------------
void ofxMouseTrap::pause(){
    //
}

//--------------------------------------------------------------
void ofxMouseTrap::update(){
    //
}


