
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

typedef struct MouseEvent {
    int x;
    int y;
    long time;
} MouseEvent;

typedef vector<vector<MouseEvent>> PathCoords; //Vector of paths -> vector of items -> vector of MouseEvents

class ofxMouseTrapData {
public:
    PathCoords paths;
    long startTime;
};

class ofxMouseTrap {
    
public:
    void recordStart();
    void recordStop();
    bool load(string filename);
    void save();
    void save(string filename);
    int getNumPaths();
    int getNumItems(int pathID);
    void recordMouseEvent(int x, int y, int button);
    void play();
    void draw();
    void stop();
    void pause();
    void update();
    void beginPath();
    void endPath();
    void toggleRecordState();
    ofPolyline getPathPolyline();
    MouseEvent getCurrentMouseEvent();
    
private:
    ofxMouseTrapData mouseData;

    int numPaths;
    ofxXmlSettings playXML;
    ofxXmlSettings recordXML;
    long currentTime;
    bool bisPlaying;
    bool bisRecording;
    bool bisRecordingPath;
    int tagCount;
    int pathCount;
    MouseEvent curEvent;
    int curEventIndex;
    int curPathIndex;
};

