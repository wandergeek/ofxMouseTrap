#include "ofxMouseTrap.h"


//--------------------------------------------------------------
void ofxMouseTrap::recordStart() {
    bisRecording = true;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStop() {
    tagCount = 0;
    pathCount = 0;
    bisRecording = false;
}

//--------------------------------------------------------------
void ofxMouseTrap::save() {
    string filename = ofGetTimestampString() + ".xml";
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Saving " + filename);
    recordXML.save(filename);
    recordStop();
    recordXML.clear();
}

//--------------------------------------------------------------
void ofxMouseTrap::draw() {
    if(bisRecordingPath) {
        ofFill();
        ofSetColor(255, 0, 0);
        ofDrawCircle(ofGetWidth()*0.95, ofGetHeight()*0.95, 20);
    }
    
}
//--------------------------------------------------------------
void ofxMouseTrap::save(string filename) {
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Saving " + filename);
    recordXML.save(filename);
}

//--------------------------------------------------------------
bool ofxMouseTrap::load(string filename){
    //check if XML
    ofFile fileToRead(ofToDataPath(filename));
    if(!fileToRead.exists()) {
        ofLog(ofLogLevel::OF_LOG_FATAL_ERROR, filename + " does not exist");
        return false;
    }
    
    playXML.load(filename);
    numPaths = playXML.getNumTags("path");
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Got " + ofToString(numPaths) + " paths");
    
    for(int i=0; i<numPaths; i++) {
        playXML.pushTag("path", i);
        int numItems = playXML.getNumTags("item");
        vector <MouseEvent> mv;
        for(int j=0; j<numItems; j++) {
            MouseEvent m;
            m.x = playXML.getAttribute("item", "x", -1, j);
            m.y = playXML.getAttribute("item", "y", -1, j);
            m.time = playXML.getAttribute("item", "timeMS", -1, j);
            mv.push_back(m);
        }
        playXML.popTag();
        mouseData.paths.push_back(mv);
    }
    
    mouseData.startTime = playXML.getAttribute("fileInfo", "startTime", 0);
    
    return true;
}

//--------------------------------------------------------------
void ofxMouseTrap::toggleRecordState() {
    bisRecording = !bisRecording;
}
//--------------------------------------------------------------
void ofxMouseTrap::beginPath() {
    if(bisRecording) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Starting path " + ofToString(pathCount));
        recordXML.addTag("path");
        recordXML.pushTag("path", pathCount);
        bisRecordingPath = true;
    }

}

//--------------------------------------------------------------
void ofxMouseTrap::endPath() {
    if(bisRecording) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Ending path " + ofToString(pathCount));
        recordXML.popTag();
        pathCount++;
        tagCount = 0;
        bisRecordingPath = false;
    }
}
//--------------------------------------------------------------
void ofxMouseTrap::recordMouseEvent(int x, int y, int button) {
    if(bisRecording) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Logging mouse: " + ofToString(x) + "," + ofToString(y));
        recordXML.addTag("item");
        recordXML.addAttribute("item", "x", x, tagCount);
        recordXML.addAttribute("item", "y", y, tagCount);
        recordXML.addAttribute("item", "timeMS", int(ofGetElapsedTimeMillis()), tagCount);
        tagCount++;
    }
}

//--------------------------------------------------------------
int ofxMouseTrap::getNumPaths(){
    return numPaths;
}

//--------------------------------------------------------------
int ofxMouseTrap::getNumItems(int pathID){
    //check if XML is loaded first
    playXML.pushTag("path", pathID);
    return playXML.getNumTags("item");

}

//--------------------------------------------------------------
void ofxMouseTrap::play(){
    if(mouseData.paths.size() == 0) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "You need to load an XML file before playing it");
        return;
    }
    
    ofResetElapsedTimeCounter();
    bisPlaying = true;
    curEventIndex = 0;
}

//--------------------------------------------------------------
MouseEvent ofxMouseTrap::getCurrentMouseEvent() {
    if(!bisPlaying) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "You've requested the current mouseevent, but the player is not currently running. Run play() first.");
    }
    
    return curEvent;
}

//--------------------------------------------------------------
ofPolyline ofxMouseTrap::getPathPolyline() {
    ofPolyline line;
    if(!bisPlaying) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "Call play() before requesting a polyline");
    } else {
        vector<MouseEvent> curPath = mouseData.paths[curPathIndex];
        for(int i=0; i<curPath.size(); i++) {
            line.addVertex(curPath[i].x, curPath[i].y);
        }
        line.close();
    }
    return line;
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
void ofxMouseTrap::update(){
    if(bisPlaying) {
        currentTime = ofGetElapsedTimeMillis();
        vector<MouseEvent> curPath = mouseData.paths[curPathIndex];
        
        if(curEventIndex + 1 > curPath.size()) {
            curEventIndex++;
            curPath = mouseData.paths[curPathIndex];
            curEventIndex = 0;
        }
        
        
        curEvent = curPath[curEventIndex];
        MouseEvent nextEvent = curPath[curEventIndex+1];
        int nextDiff = abs(nextEvent.time - currentTime);
        int curDiff = abs(curEvent.time - currentTime);
        if(nextDiff <= curDiff) {
            curEvent = nextEvent;
            ofLog(ofLogLevel::OF_LOG_NOTICE, "Moving mouse to " + ofToString(curEvent.x) + ", " + ofToString(curEvent.y));
            curEventIndex++;
        }
    }
}


