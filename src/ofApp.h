#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxDelaunay.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxObjLoader.h"

#define VBO_SIZE 30

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    
    ofxPanel gui;
    ofxIntSlider skip;
    ofxIntSlider nThreshold;
    ofxIntSlider fThreshold;
    ofxIntSlider nClip;
    ofxIntSlider fClip;
    ofxIntSlider zoomZ;
    ofxFloatSlider focalLength;
    ofxFloatColorSlider lightColor;
    ofxIntSlider lightX;
    ofxIntSlider lightY;
    ofxIntSlider lightZ;
    
    
    ofxDelaunay del;
    ofxKinect kinect;
    ofImage gray, edge;
    
    ofMesh copy_del;
    vector<ofVec3f> vertices;
    vector<ofFloatColor> colors;
    
    //switch
    bool isCanny;
    
    //camera
    ofEasyCam cam;
    
    //light
    ofLight pointLight;
    
    //vbo
    ofVbo vbos[VBO_SIZE];
    int counter;
    
};
/*
gray = getGray(cam);
edge = canny(gray);
forloop
 if edge.gray > 0
 vector<vec3f> vertices = (x, y, 0)
 vector<floatcolor> fcolor = cam.getcolor(x, y)
 vertices.triangulte()
 del.mesh.addcolors();

*/