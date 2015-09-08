
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

typedef struct MouseEvent {
    int x;
    int y;
    long time;
} MouseEvent;

typedef vector<MouseEvent> Path;
typedef vector<Path> PathCollection; 


class ofxMouseTrapData {
public:
    PathCollection paths;
    long startTime;
};

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
    void recordStart();
    void recordStop();
    bool isRecording();
    bool isPlaying();
    bool isPaused();
    
    int getNumPaths();
    int getNumItems(int pathID);
    void recordMouseEvent(int x, int y, int button);
    
    void beginPath();
    void endPath();
    vector<ofPolyline> getPathPolylines();
    MouseEvent getCurrentMouseEvent();
    
private:
    ofxMouseTrapData mouseData;
    long currentTime;
    bool bisPlaying;
    bool bisRecording;
    bool bisRecordingPath;
    bool bisPaused;
    MouseEvent curPlayEvent;
    int eventIndex;
    int pathIndex;
};

