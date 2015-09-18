
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
enum ofxMouseTrapEventType {
    ofxMouseTrapEventTypeUndefined = 0,
    ofxMouseTrapEventTypeMoved,
    ofxMouseTrapEventTypePressed,
    ofxMouseTrapEventTypeDragged,
    ofxMouseTrapEventTypeReleased
};

class ofxMouseTrapEvent {
 
public:
    
    ofxMouseTrapEvent() {
        x = 0;
        y = 0;
        button = 0;
        type = ofxMouseTrapEventTypeUndefined;

        pathIndex = 0;
        index = 0;
        time = 0;
    }
    
    int x;
    int y;
    int button;
    ofxMouseTrapEventType type;

    int pathIndex;
    int index;
    uint64_t time;
};

typedef vector<ofxMouseTrapEvent> ofxMouseTrapPath;
typedef vector<ofxMouseTrapPath> ofxMouseTrapData;

//--------------------------------------------------------------
class ofxMouseTrap {
    
public:
    
    ofxMouseTrap();
    ~ofxMouseTrap();

    void setVerbose(bool value);
    void setLoop(bool value);
    
    void recordStart();
    void recordStop();
    void recordToggle();
    bool isRecording();
    
    void update(float dt=0);
    void drawDebug();
    
    void playStart();
    void playStop();
    void playReset();
    bool isPlaying();
    bool isPlayFinished();
    bool isPlayJustFinished();
    
    void save();
    void save(string filename);
    bool load(string filename);
    
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void addMouseEvent(int x, int y, int button, ofxMouseTrapEventType type);

    const ofxMouseTrapData & getMouseData();
    const vector<ofxMouseTrapEvent *> getMouseEventsForCurrentFrame();
    vector<ofPolyline> getPathPolylines();
    
private:

    ofxMouseTrapData mouseData;
    vector<ofxMouseTrapEvent *> mouseEventsForCurrentFrame;
    
    uint64_t timeRecord;
    uint64_t timePlay;
    uint64_t timePrevFrame;

    bool bPlaying;
    bool bPlayJustFinished;
    bool bPlayFinished;
    bool bLoop;
    
    bool bRecord;
    bool bVerbose;
};

