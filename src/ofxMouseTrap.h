
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class MouseEvent {
 
public:
    
    MouseEvent() {
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

typedef vector<MouseEvent> Path;
typedef vector<Path> ofxMouseTrapData;

class ofxMouseTrap {
    
public:
    ofxMouseTrap();
    ~ofxMouseTrap();
    void update();
    void drawPaths();
    
    bool load(string filename);
    void save();
    void save(string filename);
    
    void play();
    void stop();
    void reset();
    void recordStart();
    void recordStop();
    bool isRecording();
    bool isPlaying();
    
    int getNumPaths();
    int getNumItems(int pathID);

    void recordMouseEvent(int x, int y, int button);
    
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    vector<ofPolyline> getPathPolylines();
    const MouseEvent * getCurrentMouseEvent();
    
private:
    ofxMouseTrapData mouseData;
    MouseEvent * mouseEventCurrent;
    
    uint64_t timeCurrent;
    uint64_t timeRecordStart;
    uint64_t timePlayStart;
    bool bisPlaying;
    bool bisRecording;
};

