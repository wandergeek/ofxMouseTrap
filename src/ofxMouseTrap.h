
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxMouseController.h"

typedef struct MouseEvent {
    int x;
    int y;
    long time;
} MouseEvent;

typedef vector<vector<MouseEvent>> PathCoords; //Vector of paths -> vector of items -> vector of MouseEvents

class ofxMouseTrap {
    
public:
    void recordStart();
    void recordStop();
    bool load(string filename);
    int getNumPaths();
    int getNumItems(int pathID);
    void play();
    void stop();
    void pause();
    void update();
    
private:
    PathCoords paths;
    int numPaths;
};

