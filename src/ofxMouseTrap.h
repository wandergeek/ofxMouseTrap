
#pragma once

#include "ofMain.h"

typedef vector<vector<vector <int>>> PathCoords; //might only need to be 2d?
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
    void loadPathVectorFromXML();
    PathCoords paths;
};