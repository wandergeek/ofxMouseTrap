#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){

    ofSetCircleResolution(300);
    ofSetLogLevel(ofLogLevel::OF_LOG_ERROR);
    
    trap.recordStart();
    
    playIcon.load("play.png");
    iconPos = ofPoint(ofGetWidth()*0.05, ofGetHeight()*0.05);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    trap.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    if(trap.isRecording()) {
        ofFill();
        ofSetColor(ofColor::red);
        ofDrawCircle(iconPos, 20);
        
    } else if(trap.isPlaying()) {
        playIcon.draw(iconPos);
    }
    
    trap.drawDebug();
    
    int x = ofGetWidth()*0.05;
    int y = ofGetHeight()*0.95;
    ofSetColor(ofColor::black);
    ofDrawBitmapString("Click and drag mouse to record path.", x, y);
    ofDrawBitmapString("Press R to re/record, S to Save, and P to re/play recording", x, y+20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == 'r' || key == 'R') {
        
        trap.recordToggle();
        
    } else if(key == 'p' || key  == 'P') {
        
        trap.playStart();
        
    } else if(key == 's' || key == 'S') {
        
        trap.save();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    trap.mouseDragged(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    trap.mousePressed(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    trap.mouseReleased(x,y,button);
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

    trap.load(dragInfo.files[0]);
    trap.playStart();
}
