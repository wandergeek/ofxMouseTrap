#include "ofxMouseTrap.h"

//--------------------------------------------------------------
ofxMouseTrap::ofxMouseTrap() {
    mouseData = *new ofxMouseTrapData;
    bisPlaying = false;
    bisRecording = false;
}

//--------------------------------------------------------------
ofxMouseTrap::~ofxMouseTrap() {
//    delete &mouseData; //this is  not right
}
//--------------------------------------------------------------
void ofxMouseTrap::update(){
    currentTime = ofGetElapsedTimeMillis();
    if(bisPlaying) {
        vector<MouseEvent> playPath = mouseData.paths[curPlayPathIndex];
        if(curPlayEventIndex + 1 > playPath.size()) {
            curPlayEventIndex++;
            playPath = mouseData.paths[curPlayPathIndex];
            curPlayEventIndex = 0;
        }
        
        
        curPlayEvent = playPath[curPlayEventIndex];
        MouseEvent nextEvent = playPath[curPlayEventIndex+1];
        int nextDiff = abs(nextEvent.time - currentTime);
        int curDiff = abs(curPlayEvent.time - currentTime);
        if(nextDiff <= curDiff) {
            curPlayEvent = nextEvent;
            ofLog(ofLogLevel::OF_LOG_NOTICE, "Moving mouse to " + ofToString(curPlayEvent.x) + ", " + ofToString(curPlayEvent.y));
            curPlayEventIndex++;
        }
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::stop(){
    bisPlaying = false;
}

//--------------------------------------------------------------
void ofxMouseTrap::pause(){
    bisPlaying = false;
}

//--------------------------------------------------------------
bool ofxMouseTrap::isRecording() {
    return bisRecording;
}
//--------------------------------------------------------------
bool ofxMouseTrap::isPlaying() {
    return bisPlaying;
}

//--------------------------------------------------------------
void ofxMouseTrap::play(){
    if(mouseData.paths.size() == 0) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "You need to load an XML file before playing it");
        return;
    }
    
    ofResetElapsedTimeCounter();
    bisPlaying = true;
    bisRecording = false;
    curPlayEventIndex = 0;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStart() {
    bisRecording = true;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStop() {
    bisRecording = false;
}
//--------------------------------------------------------------
void ofxMouseTrap::toggleRecordState() {
    bisRecording = !bisRecording;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordMouseEvent(int x, int y, int button) {
    if(bisRecording) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Logging mouse: " + ofToString(x) + "," + ofToString(y));
        
        MouseEvent e;
        e.x = x;
        e.y = y;
        e.time = currentTime;
        
        Path *curPath = &mouseData.paths[mouseData.paths.size()-1];
        curPath->push_back(e);
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::save() {
    string filename = ofGetTimestampString() + ".xml";
    save(filename);
}

//--------------------------------------------------------------
void ofxMouseTrap::save(string filename) {
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Saving " + filename);
    
    ofxXmlSettings xml;
    
    for(int i=0; i<mouseData.paths.size(); i++) { //for each path
        xml.addTag("path");
        xml.pushTag("path", i);
        for(int j=0; j<mouseData.paths[i].size(); j++) { //for each mouseevent in path
            MouseEvent e = mouseData.paths[i][j];
            xml.addTag("item");
            xml.addAttribute("item", "x", e.x, j);
            xml.addAttribute("item", "y", e.y, j);
            xml.addAttribute("item", "timeMS" ,int(e.time), j);
        }
        xml.popTag();
    }
    
    xml.save(filename);
}

//--------------------------------------------------------------
bool ofxMouseTrap::load(string filename){
    //check if XML
    ofFile fileToRead(ofToDataPath(filename));
    if(!fileToRead.exists()) {
        ofLog(ofLogLevel::OF_LOG_FATAL_ERROR, filename + " does not exist");
        return false;
    }
    
    ofxXmlSettings xml;
    xml.load(filename);
    int numPaths = xml.getNumTags("path");
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Got " + ofToString(numPaths) + " paths");
    
    for(int i=0; i<numPaths; i++) {
        xml.pushTag("path", i);
        int numItems = xml.getNumTags("item");
        vector <MouseEvent> mv;
        for(int j=0; j<numItems; j++) {
            MouseEvent m;
            m.x = xml.getAttribute("item", "x", -1, j);
            m.y = xml.getAttribute("item", "y", -1, j);
            m.time = xml.getAttribute("item", "timeMS", -1, j);
            mv.push_back(m);
        }
        xml.popTag();
        mouseData.paths.push_back(mv);
    }
    
    mouseData.startTime = xml.getAttribute("fileInfo", "startTime", 0);
    
    return true;
}


//--------------------------------------------------------------
void ofxMouseTrap::beginPath() {
    if(bisRecording) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Starting path " + ofToString(mouseData.paths.size()+1));
        Path newPath;
        mouseData.paths.push_back(newPath);
        bisRecordingPath = true;
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::endPath() {
    if(bisRecording) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Ending path " + ofToString(mouseData.paths.size() ));
        bisRecordingPath = false;
    }
}


//--------------------------------------------------------------
int ofxMouseTrap::getNumPaths(){
    mouseData.paths.size();
}

//--------------------------------------------------------------
int ofxMouseTrap::getNumItems(int pathID){
    return mouseData.paths[pathID].size();

}


//--------------------------------------------------------------
MouseEvent ofxMouseTrap::getCurrentMouseEvent() {
    if(!bisPlaying) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "You've requested the current mouseevent, but the player is not currently running. Run play() first.");
    }
    
    return curPlayEvent;
}

//--------------------------------------------------------------
ofPolyline ofxMouseTrap::getPathPolyline() {
    ofPolyline line;
    if(!bisPlaying) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "Call play() before requesting a polyline");
    } else {
        vector<MouseEvent> curPath = mouseData.paths[curPlayPathIndex];
        for(int i=0; i<curPath.size(); i++) {
            line.addVertex(curPath[i].x, curPath[i].y);
        }
    }
    return line;
}
