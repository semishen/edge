#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    //setup gui
    gui.setup();
    gui.setPosition(ofPoint(10,10));
    gui.add(skip.setup("Skip Points",4 ,2, 20));
    gui.add(nThreshold.setup("Near Threshold",100 ,0, 255));
    gui.add(fThreshold.setup("Far Threshold",200 ,0, 255));
    gui.add(nClip.setup("Near Clip",300 ,300, 2000));
    gui.add(fClip.setup("Far Clip",2000 ,30, 2000));
    gui.add(zoomZ.setup("zoom",1000 ,0, 2000));
    gui.add(focalLength.setup("Focal Length",1.0 ,0.0, 10.0));
    gui.add(lightColor.setup("Light Color",ofFloatColor(0.8, 1.0) ,ofFloatColor(0.0, 0.0), ofFloatColor(1.0, 1.0)));
    gui.add(lightX.setup("Light RotateX",0 ,-180, 180));
    gui.add(lightY.setup("Light RotateY",180 ,-180, 180));
    gui.add(lightZ.setup("Light RotateZ",0 ,-180, 180));
    
    //setup kinect
    kinect.init();
    kinect.open();
    kinect.setRegistration(true);
    kinect.setCameraTiltAngle(0);
    
    //allocate ofImage
    gray.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
    edge.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
    
    //switch
    isCanny = true;
    
    //light
    pointLight.setDiffuseColor(lightColor);
    pointLight.setDirectional();
    pointLight.setOrientation( ofVec3f(0, 0, 0) );
    
    counter = 0;
    
    
}

void ofApp::update() {
    //show FPS
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
    
    //update light
    pointLight.setDiffuseColor(lightColor);
    pointLight.setOrientation( ofVec3f(lightX, lightY, lightZ) );
    
    kinect.update();
    
    if(kinect.isFrameNew()) {
        
        convertColor(kinect.getPixels(), gray, CV_RGB2GRAY);
        gray.update();
        Canny(gray, edge, nThreshold, fThreshold, 3);
        edge.update();
        
        //reset
        del.reset();
        vertices.clear();
        colors.clear();
        copy_del.clear();
        
        for (int i = 0; i < kinect.getWidth(); i += skip) {
            for (int j = 0; j < kinect.getHeight(); j += skip) {
                int distance = kinect.getDistanceAt(i, j);
                if (distance >= nClip && distance <= fClip && edge.getColor(i, j).getBrightness() > 0) {
                    ofVec3f vec = ofVec3f((kinect.getWidth()-1) - i, (kinect.getHeight() -1)-j, -distance/focalLength);
                    ofColor c = kinect.getColorAt(i, j);
                    vertices.push_back(vec);
                    colors.push_back(c);
                }
            }
        }
        
        del.addPoints(vertices);
        del.triangulate();
        copy_del = del.triangleMesh;
        del.triangleMesh.addColors(colors);
        
//        string filename = "";
//        filename = ofToString(counter)+".obj";
//        ofxObjLoader::save(filename, del.triangleMesh, false, false, true);
        
        vbos[counter%VBO_SIZE].clear();
        vbos[counter%VBO_SIZE].setMesh(del.triangleMesh, GL_DYNAMIC_DRAW);
        vbos[counter%VBO_SIZE].setColorData( &colors[0], colors.size(), GL_DYNAMIC_DRAW );
        counter++;
    }
}

void ofApp::draw() {
    ofBackground(0);
    
    ofEnableLighting();
    pointLight.enable();
    cam.begin();
    
    ofPushMatrix();
    ofTranslate(-kinect.width/2, -kinect.height/2, zoomZ);
    if (counter >= VBO_SIZE) {
        for (int i = 0; i < VBO_SIZE; i++) {
            int current = counter%VBO_SIZE;
            vbos[(current+i)%VBO_SIZE].drawElements( GL_TRIANGLES, vbos[(current+i)%VBO_SIZE].getNumIndices());
            
        }
    }

//    del.triangleMesh.drawFaces();
//
    ofPushStyle();
    ofSetColor(255, 64);
    copy_del.drawWireframe();
//    copy_del.drawVertices();
//    glPointSize(10.0);
    ofPopStyle();
    ofPopMatrix();
    pointLight.draw();
    cam.end();
    
    ofDisableLighting();
    
    ofPushStyle();
    ofSetColor(255,255,255,255);
    gui.draw();
    if(isCanny){
        edge.draw(kinect.width*6/4, 0, kinect.width/2, kinect.height/2);
    }
    ofPopStyle();
    
}

//--------------------------------------------------------------
void ofApp::exit(){
    kinect.close();
    gray.clear();
    edge.clear();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'c'){
        
        isCanny = !isCanny;
    }
    
}
