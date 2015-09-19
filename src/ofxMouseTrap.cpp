#include "ofxMouseTrap.h"

//--------------------------------------------------------------
ofxMouseTrap::ofxMouseTrap() {
    
    mouseEventCurrent = NULL;
    
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
const ofxMouseTrapData & ofxMouseTrap::getMouseData() const {
    return mouseData;
}

const vector<ofxMouseTrapEvent *> ofxMouseTrap::getMouseEventsForCurrentFrame() const {
    return mouseEventsForCurrentFrame;
}

const ofxMouseTrapEvent * ofxMouseTrap::getMouseEventCurrent() const {
    return mouseEventCurrent;
}

const ofxMouseTrapPath * ofxMouseTrap::getMousePathCurrent() const {
    return mousePathCurrent;
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

void ofxMouseTrap::recordToggle() {
    if(bRecord == true) {
        recordStop();
    } else {
        recordStart();
    }
}

bool ofxMouseTrap::isRecording() const {
    return bRecord;
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

void ofxMouseTrap::playStop(){
    bPlaying = false;
    bPlayFinished = false;
    bPlayJustFinished = false;
}

void ofxMouseTrap::playReset() {
    timePlay = 0;
    timePrevFrame = 0;
}

bool ofxMouseTrap::isPlaying() const {
    return bPlaying;
}

bool ofxMouseTrap::isPlayFinished() const {
    return bPlayFinished;
}

bool ofxMouseTrap::isPlayJustFinished() const {
    return bPlayJustFinished;
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
    mouseEventCurrent = NULL;
    mousePathCurrent = NULL;

    for(int i=0; i<mouseData.size(); i++) {
        ofxMouseTrapPath & mousePath = mouseData[i];
        vector<ofxMouseTrapEvent> & mouseEvents = mousePath.mouseEvents;
        
        for(int j=0; j<mouseEvents.size(); j++) {
            ofxMouseTrapEvent & mouseEvent= mouseEvents[j];
            
            bool bInTimeRange = true;
            bInTimeRange = bInTimeRange && (mouseEvent.time >= timePrevFrame);
            bInTimeRange = bInTimeRange && (mouseEvent.time < timePlay);
            
            if(bInTimeRange == false) {
                continue;
            }
            
            mouseEventsForCurrentFrame.push_back(&mouseEvent);
        }
        
        if(mouseEvents.size() < 2) {
            continue;
        }
        
        for(int j=0; j<mouseEvents.size()-1; j++) {
            ofxMouseTrapEvent & event0 = mouseEvents[j+0];
            ofxMouseTrapEvent & event1 = mouseEvents[j+1];
            
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
            
            mousePathCurrent = &mousePath;
            
            break;
        }
    }
    
    const ofxMouseTrapPath & pathLast = mouseData[mouseData.size()-1];
    const ofxMouseTrapEvent & mouseEventLast = pathLast.mouseEvents[pathLast.mouseEvents.size()-1];
    
    bool bPlayFinishedNew = (timePlay >= mouseEventLast.time);
    bool bPlayFinishedChanged = (bPlayFinished != bPlayFinishedNew);
    bPlayJustFinished = (bPlayFinished == false) && (bPlayFinishedChanged == true);
    bPlayFinished = bPlayFinishedNew;
    
    if(bPlayJustFinished == true) {

        if(bLoop == true) {
         
            playStart();
            
        } else {
            
            bPlaying = false;
        }
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::draw() const {
    
    ofSetColor(ofColor::black);
    drawMousePaths();
    
    ofSetColor(ofColor::fuchsia);
    drawMousePoint();
    
    ofSetColor(ofColor::white);
}

void ofxMouseTrap::drawMousePaths() const {
    for(int i=0; i<mouseData.size(); i++) {
        const ofxMouseTrapPath & mousePath = mouseData[i];
        mousePath.poly.draw();
    }
}

void ofxMouseTrap::drawMousePoint(float size) const {
    if(mouseEventCurrent == NULL) {
        return;
    }
    
    ofDrawCircle(mouseEventCurrent->x, mouseEventCurrent->y, size);
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
    
    for(int i=0; i<mouseData.size(); i++) {
        const ofxMouseTrapPath & mousePath = mouseData[i];
        const vector<ofxMouseTrapEvent> & mouseEvents = mousePath.mouseEvents;
        
        xml.addTag("path");
        xml.pushTag("path", i);
        
        for(int j=0; j<mouseEvents.size(); j++) {
            const ofxMouseTrapEvent & mouseEvent = mouseEvents[j];
            
            xml.addTag("item");
            xml.addAttribute("item", "x", mouseEvent.x, j);
            xml.addAttribute("item", "y", mouseEvent.y, j);
            xml.addAttribute("item", "button", mouseEvent.button, j);
            xml.addAttribute("item", "type", mouseEvent.type, j);
            
            string timeAsStr = ofToString(mouseEvent.time);
            xml.addAttribute("item", "timeMS", timeAsStr, j);
        }
        
        xml.popTag();
    }
    
    xml.save(filename);
}

bool ofxMouseTrap::load(string filename) {
    
    ofxXmlSettings xml;
    bool bLoaded = xml.load(filename);
    
    if(bLoaded == false) {
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_FATAL_ERROR, filename + " does not exist");
        }
        return false;
    }
    
    int numPaths = xml.getNumTags("path");
    if(numPaths == 0) {
        if(bVerbose == true) {
            ofLog(ofLogLevel::OF_LOG_FATAL_ERROR, "ofxMouseTrap::load - no mouse paths found in xml.");
        }
        return false;
    }
    
    if(bVerbose == true) {
        ofLog(ofLogLevel::OF_LOG_NOTICE, "ofxMouseTrap::load - found " + ofToString(numPaths) + "mouse paths");
    }
    
    mouseData.clear();
    
    for(int i=0; i<numPaths; i++) {
        xml.pushTag("path", i);
        int numItems = xml.getNumTags("item");
        
        mouseData.push_back(ofxMouseTrapPath());
        ofxMouseTrapPath & mousePath = mouseData.back();
        vector<ofxMouseTrapEvent> & mouseEvents = mousePath.mouseEvents;

        for(int j=0; j<numItems; j++) {
            
            mouseEvents.push_back(ofxMouseTrapEvent());
            ofxMouseTrapEvent & mouseEvent = mouseEvents.back();
            
            mouseEvent.x = xml.getAttribute("item", "x", -1, j);
            mouseEvent.y = xml.getAttribute("item", "y", -1, j);
            mouseEvent.button = xml.getAttribute("item", "button", -1, j);
            mouseEvent.type = (ofxMouseTrapEventType)xml.getAttribute("item", "type", -1, j);
            
            string timeAsStr = xml.getAttribute("item", "timeMS", "", j);
            mouseEvent.time = ofToInt64(timeAsStr);
            
            mouseEvent.mousePathIndex = i;
            mouseEvent.mouseEventIndex = j;
            
            mousePath.poly.addVertex(mouseEvent.x, mouseEvent.y);
        }

        xml.popTag();
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
    
    ofxMouseTrapPath & mousePath = mouseData[mouseData.size()-1];
    vector<ofxMouseTrapEvent> & mouseEvents = mousePath.mouseEvents;
    mouseEvents.push_back(ofxMouseTrapEvent());
    ofxMouseTrapEvent & mouseEvent = mouseEvents.back();
    
    mouseEvent.x = x;
    mouseEvent.y = y;
    mouseEvent.button = button;
    mouseEvent.type = type;
    mouseEvent.time = timeRecord;

    mouseEvent.mousePathIndex = mouseData.size()-1;
    mouseEvent.mouseEventIndex = mouseEvents.size()-1;
    
    mousePath.poly.addVertex(x, y);
}
