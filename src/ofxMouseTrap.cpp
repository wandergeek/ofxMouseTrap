#include "ofxMouseTrap.h"

//--------------------------------------------------------------
ofxMouseTrap::ofxMouseTrap() {
    bisPlaying = false;
    bisRecording = false;
    currentTime = 0;
}

//--------------------------------------------------------------
ofxMouseTrap::~ofxMouseTrap() {
    //
}

//--------------------------------------------------------------
void ofxMouseTrap::update(){
    currentTime = ofGetElapsedTimeMillis();
    
    if(bisPlaying) {
        vector<MouseEvent> curPlayPath = mouseData.paths[pathIndex];
        
        //loop path and event indexes
        if(eventIndex + 1 >= curPlayPath.size()) {
            eventIndex = 0;
            pathIndex++;
            if(pathIndex >= mouseData.paths.size()) {
                ofResetElapsedTimeCounter();
                currentTime = 0;
                pathIndex = 0;
            }
        }
        
        curPlayPath = mouseData.paths[pathIndex];
        curPlayEvent = curPlayPath[eventIndex];
        MouseEvent nextEvent = curPlayPath[eventIndex+1];
        int nextDiff = abs(nextEvent.time - currentTime);
        int curDiff = abs(curPlayEvent.time - currentTime);
        
        if(nextDiff <= curDiff) {
            curPlayEvent = nextEvent;
            eventIndex++;
            ofLog(ofLogLevel::OF_LOG_NOTICE, "Moving mouse to " + ofToString(curPlayEvent.x) + ", " + ofToString(curPlayEvent.y));
        }
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::drawPaths(){
    vector<ofPolyline> lines = getPathPolylines();
    ofSetColor(ofColor::black);
    for(int i=0; i<lines.size(); i++) {
        lines[i].draw();
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::stop(){
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
    eventIndex = 0;
    pathIndex = 0;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStart() {
    bisRecording = true;
    bisPlaying = false;
    mouseData.paths.clear();
    ofResetElapsedTimeCounter();
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStop() {
    bisRecording = false;
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
    mouseData.paths.clear();
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
vector<ofPolyline> ofxMouseTrap::getPathPolylines() {
    vector<ofPolyline> lines;
        int numPaths = mouseData.paths.size();
        for(int i=0; i<numPaths; i++) {
            ofPolyline line;
            vector<MouseEvent> curPath = mouseData.paths[i];
            for(int i=0; i<curPath.size(); i++) {
                line.addVertex(curPath[i].x, curPath[i].y);
            }
            lines.push_back(line);
            line.clear();
        }
    return lines;
}
