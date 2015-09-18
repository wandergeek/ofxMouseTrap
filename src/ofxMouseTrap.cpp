#include "ofxMouseTrap.h"

//--------------------------------------------------------------
ofxMouseTrap::ofxMouseTrap() {
    
    timeRecord = 0;
    timePlay = 0;
    timePrevFrame = 0;

    bPlaying = false;
    bPlayJustFinished = false;
    bPlayFinished = false;
    bLoop = false;
    
    bRecord = false;
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

void ofxMouseTrap::setLoop(bool value) {
    bLoop = value;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStart() {
    if(bRecord == true) {
        return;
    }
    bRecord = true;
    timeRecord = 0;
    
    bPlaying = false;
    mouseData.clear();
}

void ofxMouseTrap::recordStop() {
    if(bRecord == false) {
        return;
    }
    bRecord = false;
}

bool ofxMouseTrap::isRecording() {
    return bRecord;
}

//--------------------------------------------------------------
void ofxMouseTrap::update(float dt) {
    
    double nanoToSeconds = 0.000000001;
    uint64_t timeOfLastFrame = dt / nanoToSeconds;
    if(timeOfLastFrame == 0) {
        timeOfLastFrame = ofGetLastFrameTime() / nanoToSeconds;
    }
    
    if(bRecord == true) {
        timeRecord += timeOfLastFrame;
        return;
    }
    
    if(bPlaying == false) {
        return;
    }
    
    bool bValid = true;
    bValid = bValid && (mouseData.size() > 0);
    if(bValid == false) {
        return;
    }
    
    timePrevFrame = timePlay;
    timePlay += timeOfLastFrame;
    
    mouseEventsForCurrentFrame.clear();

    for(int i=0; i<mouseData.size(); i++) {
        vector<ofxMouseTrapEvent> & path = mouseData[i];
        
        if(path.size() < 2) {
            continue;
        }
        
        for(int j=0; j<path.size(); j++) {
            ofxMouseTrapEvent & event= path[j];
            
            bool bInTimeRange = true;
            bInTimeRange = bInTimeRange && (event.time >= timePrevFrame);
            bInTimeRange = bInTimeRange && (event.time < timePlay);
            
            if(bInTimeRange == false) {
                continue;
            }
            
            mouseEventsForCurrentFrame.push_back(&event);
        }
    }
    
    const ofxMouseTrapPath & pathLast = mouseData[mouseData.size()-1];
    const ofxMouseTrapEvent & mouseEventLast = pathLast[pathLast.size()-1];
    
    bool bPlayFinishedNew = (timePlay >= mouseEventLast.time);
    bool bPlayFinishedChanged = (bPlayFinished != bPlayFinishedNew);
    bPlayFinished = bPlayFinishedNew;
    bPlayJustFinished = (bPlayFinished == false) && (bPlayFinishedChanged == true);
    
    if(bPlayJustFinished == true) {

        if(bLoop == true) {
         
            playStart();
            
        } else {
            
            bPlaying = false;
        }
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::drawDebug(){
    vector<ofPolyline> lines = getPathPolylines();
    ofSetColor(ofColor::black);
    for(int i=0; i<lines.size(); i++) {
        lines[i].draw();
    }
    
    bool bDrawMousePosition = true;
    bDrawMousePosition = bDrawMousePosition && (bPlaying == true);
    bDrawMousePosition = bDrawMousePosition && (mouseEventsForCurrentFrame.size() > 0);
    
    if(bDrawMousePosition == false) {
        return;
    }
    
    const ofxMouseTrapEvent & mouseEventLast = *mouseEventsForCurrentFrame[mouseEventsForCurrentFrame.size()-1];
    
    ofFill();
    ofSetColor(ofColor::fuchsia);
    ofDrawCircle(mouseEventLast.x, mouseEventLast.y, 20);
}

//--------------------------------------------------------------
void ofxMouseTrap::playStart(){
    recordStop();
    
    if(mouseData.size() == 0) {
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_ERROR, "You need to load an XML file before playing it");
        }
        return;
    }
    
    playReset();
    
    bPlaying = true;
    bPlayFinished = false;
    bPlayJustFinished = false;
}

//--------------------------------------------------------------
void ofxMouseTrap::playStop(){
    bPlaying = false;
    bPlayFinished = false;
    bPlayJustFinished = false;
}

//--------------------------------------------------------------
void ofxMouseTrap::playReset() {
    timePlay = 0;
    timePrevFrame = 0;
}

//--------------------------------------------------------------
bool ofxMouseTrap::isPlaying() {
    return bPlaying;
}

bool ofxMouseTrap::isPlayFinished() {
    return bPlayFinished;
}

bool ofxMouseTrap::isPlayJustFinished() {
    return bPlayJustFinished;
}

//--------------------------------------------------------------
void ofxMouseTrap::save() {
    string filename = ofGetTimestampString() + ".xml";
    save(filename);
}

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
void ofxMouseTrap::mousePressed(int x, int y, int button) {
    addMouseEvent(x, y, button, ofxMouseTrapEventTypePressed);
}

void ofxMouseTrap::mouseDragged(int x, int y, int button) {
    addMouseEvent(x, y, button, ofxMouseTrapEventTypeDragged);
}

void ofxMouseTrap::mouseReleased(int x, int y, int button) {
    addMouseEvent(x, y, button, ofxMouseTrapEventTypeReleased);
}

void ofxMouseTrap::addMouseEvent(int x, int y, int button, ofxMouseTrapEventType type) {
    if(bRecord == false) {
        return;
    }
    
    if(type == ofxMouseTrapEventTypePressed) {
        
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_NOTICE, "Starting path " + ofToString(mouseData.size()+1));
        }
        
        ofxMouseTrapPath newPath;
        mouseData.push_back(newPath);
    }
    
    if(type == ofxMouseTrapEventTypeDragged) {

        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_NOTICE, "Logging mouse: " + ofToString(x) + "," + ofToString(y));
        }
    }
    
    if(type == ofxMouseTrapEventTypeReleased) {
        
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_NOTICE, "Ending path " + ofToString(mouseData.size() ));
        }
    }
    
    ofxMouseTrapPath & path = mouseData[mouseData.size()-1];
    ofxMouseTrapEvent event;
    
    event.x = x;
    event.y = y;
    event.button = button;
    event.type = type;

    event.pathIndex = mouseData.size()-1;
    event.index = path.size();
    event.time = timeRecord;
    
    path.push_back(event);
}

//--------------------------------------------------------------
const ofxMouseTrapData & ofxMouseTrap::getMouseData() {
    return mouseData;
}

//--------------------------------------------------------------
const vector<ofxMouseTrapEvent *> ofxMouseTrap::getMouseEventsForCurrentFrame() {
    return mouseEventsForCurrentFrame;
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
