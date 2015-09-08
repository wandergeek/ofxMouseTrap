#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){

//    trap.load("2015-09-07-16-28-56-548.xml");
//    trap.recordStart();
    ofSetCircleResolution(300);
    trap = ofxMouseTrap();
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
        ofDrawCircle(ofGetWidth()*0.05, ofGetHeight()*0.05, 20);
        ofSetColor(ofColor::black);
        debugLine.draw();
        
    } else if(trap.isPlaying()) {
        ofPolyline line = trap.getPathPolyline();
        ofSetColor(ofColor::white);
        line.draw();
        MouseEvent e = trap.getCurrentMouseEvent();
        ofFill();
        ofSetColor(ofColor::fuchsia);
        ofDrawCircle(e.x, e.y, 20);
    }
    int x = ofGetWidth()*0.05;
    int y = ofGetHeight()*0.95;
    ofSetColor(ofColor::black);
    ofDrawBitmapString("Press R to Record, S to Save, P to play last recording, and spacebar to pause playback", x, y);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 's') {
        trap.save();
    } if(key == 'r') {
        trap.recordStart();
    } else if(key == ' ') {
        trap.toggleRecordState();
    } else if(key == 'p') {
        trap.play();
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
    trap.recordMouseEvent(x,y,button);
    debugLine.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    trap.beginPath();
    debugLine.clear();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    trap.endPath();
//    debugLine.close();
    
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
    trap.play();
}
