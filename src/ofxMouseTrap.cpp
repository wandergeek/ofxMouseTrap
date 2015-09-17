#include "ofxMouseTrap.h"

//--------------------------------------------------------------
ofxMouseTrap::ofxMouseTrap() {
    
    mouseEventCurrent = NULL;
    
    timeCurrent = 0;
    timeRecordStart = 0;
    timePlayStart = 0;

    bPlaying = false;
    bRecording = false;
    bVerbose = true;
}

//--------------------------------------------------------------
ofxMouseTrap::~ofxMouseTrap() {
    //
}

//--------------------------------------------------------------
void ofxMouseTrap::setVerbose(bool value) {
    bVerbose = value;
}

//--------------------------------------------------------------
void ofxMouseTrap::update(){
    
    timeCurrent = ofGetElapsedTimeMillis();
    
    if(bPlaying == false) {
        return;
    }
    
    if(mouseData.size() == 0) {
        return;
    }
    
    uint64_t timePlay = timeCurrent - timePlayStart;
    
    mouseEventCurrent = NULL;

    for(int i=0; i<mouseData.size(); i++) {
        vector<ofxMouseTrapEvent> & path = mouseData[i];
        
        if(path.size() < 2) {
            continue;
        }
        
        for(int j=0; j<path.size()-1; j++) {
            ofxMouseTrapEvent & event0 = path[j+0];
            ofxMouseTrapEvent & event1 = path[j+1];
            
            bool bInTimeRange = true;
            bInTimeRange = bInTimeRange && (timePlay >= event0.time);
            bInTimeRange = bInTimeRange && (timePlay < event1.time);
            
            if(bInTimeRange == false) {
                continue;
            }
            
            uint64_t time0 = timePlay - event0.time;
            uint64_t time1 = event0.time - timePlay;
            
            mouseEventCurrent = &event0;
            if(time1 < time0) {
                mouseEventCurrent = &event1;
            }
        }
    }
    
    const ofxMouseTrapPath& pathLast = mouseData[mouseData.size()-1];
    const ofxMouseTrapEvent & mouseEventLast = pathLast[pathLast.size() - 1];
    if(timePlay > mouseEventLast.time) {
        playReset();
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::drawDebug(){
    vector<ofPolyline> lines = getPathPolylines();
    ofSetColor(ofColor::black);
    for(int i=0; i<lines.size(); i++) {
        lines[i].draw();
    }
    
    if(bPlaying == true) {
        const ofxMouseTrapEvent * mouseEventCurrent = getCurrentMouseEvent();
        
        if(mouseEventCurrent != NULL) {
            ofFill();
            ofSetColor(ofColor::fuchsia);
            ofDrawCircle(mouseEventCurrent->x, mouseEventCurrent->y, 20);
        }
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::playStart(){
    if(bRecording == true) {
        recordStop();
    }
    
    if(mouseData.size() == 0) {
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_ERROR, "You need to load an XML file before playing it");
        }
        return;
    }
    
    playReset();
    
    bPlaying = true;
}

//--------------------------------------------------------------
void ofxMouseTrap::playStop(){
    bPlaying = false;
}

//--------------------------------------------------------------
void ofxMouseTrap::playReset() {
    timePlayStart = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
bool ofxMouseTrap::isPlaying() {
    return bPlaying;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStart() {
    bRecording = true;
    bPlaying = false;
    mouseData.clear();
    timeRecordStart = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStop() {
    bRecording = false;
}

//--------------------------------------------------------------
bool ofxMouseTrap::isRecording() {
    return bRecording;
}

//--------------------------------------------------------------
void ofxMouseTrap::mousePressed(int x, int y, int button) {
    if(bRecording == false) {
        return;
    }
    
    if(bVerbose == true) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Starting path " + ofToString(mouseData.size()+1));
    }
    
    ofxMouseTrapPath newPath;
    mouseData.push_back(newPath);
    
    addMouseEvent(x, y, button);
}

//--------------------------------------------------------------

void ofxMouseTrap::mouseDragged(int x, int y, int button) {
    addMouseEvent(x, y, button);
}

//--------------------------------------------------------------
void ofxMouseTrap::mouseReleased(int x, int y, int button) {
    if(bRecording == false) {
        return;
    }
    
    if(bVerbose == true) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Ending path " + ofToString(mouseData.size() ));
    }
    
    addMouseEvent(x, y, button);
}

//--------------------------------------------------------------
void ofxMouseTrap::addMouseEvent(int x, int y, int button) {
    if(bRecording == false) {
        return;
    }
    
    if(bVerbose == true) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Logging mouse: " + ofToString(x) + "," + ofToString(y));
    }
    
    uint64_t timeNow = ofGetElapsedTimeMillis();
    uint64_t timeRecord = timeNow - timeRecordStart;
    
    ofxMouseTrapEvent e;
    ofxMouseTrapPath *curPath = &mouseData[mouseData.size()-1];
    
    e.x = x;
    e.y = y;
    e.time = timeRecord;
    e.pathIndex = mouseData.size()-1;
    e.index = curPath->size();
    e.button = button;
    
    curPath->push_back(e);
}

//--------------------------------------------------------------
void ofxMouseTrap::save() {
    string filename = ofGetTimestampString() + ".xml";
    save(filename);
}

//--------------------------------------------------------------
void ofxMouseTrap::save(string filename) {
    
    if(bVerbose == true) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Saving " + filename);
    }
    
    ofxXmlSettings xml;
    
    for(int i=0; i<mouseData.size(); i++) { //for each path
        xml.addTag("path");
        xml.pushTag("path", i);
        for(int j=0; j<mouseData[i].size(); j++) { //for each mouseevent in path
            ofxMouseTrapEvent e = mouseData[i][j];
            xml.addTag("item");
            xml.addAttribute("item", "x", e.x, j);
            xml.addAttribute("item", "y", e.y, j);
            xml.addAttribute("item", "timeMS" ,int(e.time), j);
        }
        xml.popTag();
    }
    
    xml.save(filename);
    mouseData.clear();
}

//--------------------------------------------------------------
bool ofxMouseTrap::load(string filename){
    //check if XML
    ofFile fileToRead(ofToDataPath(filename));
    if(!fileToRead.exists()) {
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_FATAL_ERROR, filename + " does not exist");
        }
        return false;
    }
    
    ofxXmlSettings xml;
    xml.load(filename);
    int numPaths = xml.getNumTags("path");
    if(bVerbose == true) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "Got " + ofToString(numPaths) + " paths");
    }
    
    for(int i=0; i<numPaths; i++) {
        xml.pushTag("path", i);
        int numItems = xml.getNumTags("item");
        vector <ofxMouseTrapEvent> mv;
        for(int j=0; j<numItems; j++) {
            ofxMouseTrapEvent m;
            m.x = xml.getAttribute("item", "x", -1, j);
            m.y = xml.getAttribute("item", "y", -1, j);
            m.time = xml.getAttribute("item", "timeMS", -1, j);
            mv.push_back(m);
        }
        xml.popTag();
        mouseData.push_back(mv);
    }
    
    return true;
}

//--------------------------------------------------------------
const ofxMouseTrapData & ofxMouseTrap::getMouseData(){
    return mouseData;
}

//--------------------------------------------------------------
const ofxMouseTrapEvent * ofxMouseTrap::getCurrentMouseEvent() {
    if(bPlaying == false) {
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_ERROR, "You've requested the current mouseevent, but the player is not currently running. Run play() first.");
        }
    }
    
    return mouseEventCurrent;
}

//--------------------------------------------------------------
vector<ofPolyline> ofxMouseTrap::getPathPolylines() {
    vector<ofPolyline> lines;
        int numPaths = mouseData.size();
        for(int i=0; i<numPaths; i++) {
            ofPolyline line;
            vector<ofxMouseTrapEvent> curPath = mouseData[i];
            for(int i=0; i<curPath.size(); i++) {
                line.addVertex(curPath[i].x, curPath[i].y);
            }
            lines.push_back(line);
            line.clear();
        }
    return lines;
}
