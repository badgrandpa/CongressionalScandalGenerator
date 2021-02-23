#pragma once
#include "ofMain.h"

using namespace std;

class colorDetection {
public:
    
    //primitive variables
    const int secs_for_color_detection_update = 2;
    const int frames_for_color_detection_update = ofGetFrameRate() * secs_for_color_detection_update;
    int closestColorX, closestColorY;
    int count;
    bool running;
    bool color_detected;
    //136, 113, 59,
    int yellow_r = 193, yellow_g = 167, yellow_b = 100; //collors to detect
    int blue_r = 57, blue_g = 119, blue_b = 187;
    float rTarget;
    float gTarget;
    float bTarget;
    float threshold = 10;
    
    //classes
    ofVideoGrabber vidGrabber;
    
    //methods
    void setup();
    void update();
    void draw();
    void detect_color();
    bool is_color_detected(); //boolean getter
    void keyPressed(int key);//-
    void mousePressed(int x, int y, int button);//-
};
