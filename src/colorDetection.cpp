#include "colorDetection.h"

//0. vidGrabber initialized on start [setup()]
//1. detect color continuously [update()]
//2. if color detected
//2a. stop the color detection for x secs [update()]
//2b. after x secs restart color detection [update()]

//---------------------
void colorDetection::setup(){
  // 0
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(30);
    vidGrabber.initGrabber(1280, 720);
    //yellow color to be detected
    rTarget = yellow_r;
    gTarget = yellow_g;
    bTarget = yellow_b;
    
}

//---------------------
void colorDetection::update(){
    vidGrabber.update();
    // start/stopped -> started after 0 secs if in beginning|x secs if in middle
    // started -> started if no color|stopped if color
    // 1 & 2a
    //if color not detected start or keep detecting.
    //if (vidGrabber.isFrameNew())
        if(!color_detected)
            detect_color();
        // 2
        // else stop detection and restart after x secs
        else {
            color_detected = false;
            if(ofGetFrameNum() % frames_for_color_detection_update == 0)
                detect_color();
        }
}

//---------------------
bool colorDetection::is_color_detected(){
    
    return color_detected;
}

//---------------------
void colorDetection::detect_color(){
    // these are used in the search for the location of the pixel
    // whose color is the closest to our target color.
    closestColorX = 0;
    closestColorY = 0;
    int w = vidGrabber.getWidth();
    int h = vidGrabber.getHeight();
    count = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            
            // Extract the color components of the pixel at (x,y)
            // from myVideoGrabber (or some other image source)
            ofColor colorAtXY = vidGrabber.getPixels().getColor(x, y);
            float rAtXY = colorAtXY.r;
            float gAtXY = colorAtXY.g;
            float bAtXY = colorAtXY.b;
            
            float colorDistance = ofDist(rAtXY, gAtXY, bAtXY, rTarget, gTarget, bTarget);
            
            if(colorDistance < threshold){
                closestColorX+= x;
                closestColorY+= y;
                count++;
            }
        }
    }
    
    cout << " #=" << count;

    // 2
    if (count > 0) {
        // test - calculate the average coordinate
        closestColorX = closestColorX / count;
        closestColorY = closestColorY / count;
        color_detected = true; //detects color
    }
    else
        color_detected = false;
}

//---------------------
void colorDetection::draw(){
    vidGrabber.draw(0, 0);
    ofDrawEllipse(closestColorX, closestColorY, 40, 40);
}
