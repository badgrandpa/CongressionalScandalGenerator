// This projet uses: Interact with GPT-2 algorithm from Runway example by Roy Macdonald

 //===========================================================================
#pragma once

#include "ofMain.h"
//ml
#include "ofxRunway.h"
//color trigger
#include "colorDetection.h"

using namespace std;

// make sure you make your ofApp class to inherit from ofxRunwayListener. Has multiple inheritance-isa
class ofApp : public ofBaseApp, public ofxRunwayListener{
	
public:
    //Primitive variables
    const bool bUseRandomSeed = true;// random seed each time a prompt is sent
    const int congressperson_images_limit = 200; // make greater than the max number of files to use all
    const int color_detection_limit = 2000; // max number of seconds it waits untill it tracks again
    const int secs_for_prompt_update = 17; //3 for 50 char, 20 for 550 char aprox
    const int frames_for_prompt_update = ofGetFrameRate() * secs_for_prompt_update;
    const int secs_for_trigger_scandal = 2; //3 for 50, 16 for 500
    const int frames_for_trigger_scandal = ofGetFrameRate() * secs_for_trigger_scandal;
    const int min = 490, max = 520; //min and max characters per scandal
    const string women_image_folder = "congresswomen/"; //where the images are stored
    const string men_image_folder = "congressmen/";
    bool random_gender_is_female;
    string runwayResult, prompt, current_scandal, formatted_scandal;
    int seed, max_characters, congresswoman_counter, congressman_counter;
    
        // classes
    colorDetection color_detection;
    ofVideoPlayer loading_video, welcome_video; //a new scandal is being generated - loading video
    ofColor fontColor, imageColor;
    ofDirectory dir_w = ofDirectory(women_image_folder);
    ofDirectory dir_m = ofDirectory(men_image_folder);
    ofTrueTypeFont myFont, myType;
    ofImage congressperson_to_add, background_image;
    ofImage congressperson_to_draw = ofImage(" ");
    
    ofxRunway runway; //composition-hasa class relationship. Access methods of the class through the object.

    // arrays and queues
    array <string, 4> prompts = {"El congresista", "La congresista", "El excongresista", "La excongresista"}; // declaration - type name [elements];
    //mapping hte string values of the scandal to the images
    //map <string, ofImage> scandal_to_image_map;
    queue <pair <string, ofImage>> scandal_image_queue;
    
    queue <string> buffer; //creating a vector of strings to store the buffer text created form Runwayml
    queue <ofImage> selected_congresswoman; //indirectly defined by text queue
    queue <ofImage> selected_congressman; //indirectly defined by text queue
    
    // methods
	void setup();
	void update();
	void draw();
//	void keyReleased(int key);
    int random_string(int i);
    void setPrompt(const ofJson& text);
	void promptForInput();
	// Callback functions that process what Runway sends back
	void runwayInfoEvent(ofJson& info);
	void runwayErrorEvent(string& message);
    void trigger_scandal();//event handler
};
