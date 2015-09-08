
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofxMouseTrapEvent {
 
public:
    
    ofxMouseTrapEvent() {
        x = 0;
        y = 0;
        button = 0;
        pathIndex = 0;
        time = 0;
    }
    
    int x;
    int y;
    int button;
    int index;
    int pathIndex;
    uint64_t time;
    
};

typedef vector<ofxMouseTrapEvent> ofxMouseTrapPath;
typedef vector<ofxMouseTrapPath> ofxMouseTrapData;

class ofxMouseTrap {
    
public:
    ofxMouseTrap();
    ~ofxMouseTrap();

    void update();
    void drawDebug();
    
    void playStart();
    void playStop();
    void playReset();
    bool isPlaying();
    
    void recordStart();
    void recordStop();
    bool isRecording();
    
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void addMouseEvent(int x, int y, int button);
    
    void save();
    void save(string filename);
    bool load(string filename);

    const ofxMouseTrapData & getMouseData();
    const ofxMouseTrapEvent * getCurrentMouseEvent();
    vector<ofPolyline> getPathPolylines();
    
private:
    ofxMouseTrapData mouseData;
    ofxMouseTrapEvent * mouseEventCurrent;
    
    uint64_t timeCurrent;
    uint64_t timeRecordStart;
    uint64_t timePlayStart;
    bool bisPlaying;
    bool bisRecording;
};

