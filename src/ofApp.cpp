#include "ofApp.h"
#include "colorDetection.h"

/*
 Alesandra Miro Quesada
 MA Computational Arts, Creative Coding 2

                         Congressional Scandal Generator
                           A dissobedient Installation
 
 This is a scandal creator that uses computer vision and artificial intelligence to generate congressional scandals. The database used to train both the visual and written Neural Netwrok models, was news articles of real scandals that happened in the Peruvian congress from 2000 untill 2020.
 For the text generator a GPT-2 NN was trained with 641 articles of congressional scandals. Through HTTP, OF sends a randomized prompt to Runway which returns a uniquely generate scandal which, more often than not comes with a twist or unexpected turn.
 For the visual generation, Style-GAN-2 was used and trained with  1084 iamges of real peruvian congerssmen and women. After rouhly 6 hours of training, synthetic images where pregenerated, stored in the data folder and sorted in 2 gategories: congresswomen and congresmen. In unison each text prompt which is sent to runway not only triggers a written scandal but also triggers its corresponding congressperson image.
 For the interaction I have used computer vision and color detection. In order to generate a new congressional scandal. The user needs to show an emblematic yellow sign to the screen and a  unique scandal is created. The use of timers, queues and prompts was very handy in order to create a smooth UX and fun experience. 
 Code walkthrough: https://vimeo.com/417633392/fd0860216e
 Blogpost: http://doc.gold.ac.uk/compartsblog/index.php/work/congressional-scandal-generator/
 
 */

// algorithm for texts
// action - trigger
// r1. load GPT-2 models to runway
// r2. run runway server
// 1. prompt random selection - without queue: user action, e.g. button press
//                               - with queue: time: frame number % 900 (30s) [update()]

// 2a. prompt sending to runway via HTTP [update()]
// r3. scandal ML GPT-2 generation by runway (time consuming)
// 3. reception of generated scandal from runway - runway; only with queue [update()]
// 4. addition of received scandal and loaded image in the scandal-image queue - only with queue [update()]
// 5. scandal and image selection from queue head - only with queue; user action [trigger_scandal()]
// 6a. scandal display [draw()]

//algorithm for images
//2b. find article gender of the current prompt - time [update()]
//2c. load pregenerated StyleGAN2 model congressperson image corresponding to the found gender linearly from image folder
//x2d. addition of loaded image to either congressman or congresswoman queue
//x5b. congressman or congresswoman queue head selection - user action [keyReleased()]
//6b. congressman or congresswoman image display [draw()]

//--------------------------------------------------------------

void ofApp::setup(){
    //main setup
    ofSetFrameRate(30);
    background_image.load("background02.png");
    loading_video.load("loading_video_1024.mp4");
    welcome_video.load("welcome.mp4");
	//uncomment the following line if you want a verbose log (which means a lot of info will be printed)
	ofSetLogLevel(OF_LOG_VERBOSE);

    //fill iamge directories
    dir_m.listDir();
    dir_w.listDir();

    //load font
    ofTrueTypeFont::setGlobalDpi(72);
    myType.load("Bitter-Regular.otf", 28, true, true, true);
    myType.setLineHeight(36);
    myType.setLetterSpacing(2);
        
	// setup Runway
	runway.setup(this, "http://localhost:8000");
	runway.start();
    
    //color tracking
    color_detection.setup();

}
//--------------------------------------------------------------
void ofApp::update(){

    // every secs_for_update do the following
    if(ofGetFrameNum() % frames_for_prompt_update == 0) {
        // 1
        //randomly selecting a string based on the size of prompt
        string randomPrompt =  prompts[random_string(prompts.size())];
        
        cout << randomPrompt << " "; // this will print all the contents of *features*
        
        // 2a
        // sending prompt to runway
        setPrompt(randomPrompt);
    
        // 2b
        // congressperson male or female dependign on the articles La or El
        if(randomPrompt.compare(prompts[1]) == 0 || randomPrompt.compare(prompts[3]) == 0 ) {
            random_gender_is_female = true;
            cout<<"La prompt found";
            // recycling image from woman image directory
            //use mininum between size of directory and image - for testing
            // 2c
            if(++congresswoman_counter > dir_w.size() || congresswoman_counter > congressperson_images_limit) //which is 5
                congresswoman_counter = 1;
            // loading image and dynamically increasing the name of the image
            congressperson_to_add = ofImage(dir_w.getPath(congresswoman_counter - 1));
            
            //x2d
            //selected_congresswoman.push(congressperson_to_add); //adding to queue to be triggered by user
        }
        else if(randomPrompt.compare(prompts[0]) == 0 || randomPrompt.compare(prompts[2]) == 0) {
            random_gender_is_female = false;
            cout<<"El prompt found";
           // recyling image from man image directory and compositing it with the number of congressperson images
            // 2c
            if(++congressman_counter > dir_m.size() || congressman_counter > congressperson_images_limit) //which is 5
                congressman_counter = 1;
            
            // loading image and dynamically increasing the name of the image
            congressperson_to_add = ofImage(dir_m.getPath(congressman_counter - 1));
           
        }

    }

    // r3

    // 3
    // getting scandal from runway
    if(runway.get("generated_text", runwayResult)) {
       /// handy function for setting the clipboard (as if you'd copy the string)
       /// you can now paste this string on any other software
        ofSetClipboardString(runwayResult);

        // 4
        scandal_image_queue.push(make_pair(runwayResult, congressperson_to_add));
       // scandal_to_image_map[runwayResult] = congressperson_to_add;
        // scandal_to_image_map.insert(make_pair(runwayResult, congressperson_to_add));
        //buffer.push(runwayResult);
    }
    
    //loading video
    loading_video.update();
    welcome_video.update();
        
    //color detection
    color_detection.update();
    
    // 5
    if(color_detection.is_color_detected()) {//trigger scandal only if color is detected
        if(ofGetFrameNum() % frames_for_trigger_scandal == 0)
            trigger_scandal();
    }
}
//--------------------------------------------------------------
void ofApp::setPrompt(const ofJson& _prompt){
	if(runway.isBusy()) return;
	if(_prompt.empty()) return;
	prompt = _prompt;
	
	ofxRunwayData data;
	
    //randomizing seed and max character in runway to generate more believable results
	data.setString("prompt", prompt);
	if(bUseRandomSeed) {
		seed = ofRandom(99999);
        max_characters = ofRandom(min, max);
	}
    
    //sending data
	data.setInt("seed", seed);
    data.setInt("max_characters", max_characters);
	
	// query Runway
	runway.send(data);
}

//--------------------------------------------------------------

int ofApp::random_string(int i) {
    
    //this random index will select the string
    return std::rand()%i; // random generator function:
}

//--------------------------------------------------------------
string formatTextWidth(const string& text, float width){
	
	string formated;
	auto lines = ofSplitString(text, "\n");//checks end of sentence and add is to the vector
	if(lines.size()){
        ofBitmapFont myFont;
		float space_width = myFont.getBoundingBox(" ", 0, 0).width; //width of the character for myFont
		
		for(size_t l = 0; l < lines.size(); l++){
			auto words = ofSplitString(lines[l], " "); //loop counter. Splits each line into words and stores it in a vector
			
            if(words.size()){
				formated += words[0];
				float accum = myFont.getBoundingBox(words[0], 0, 0).width + space_width;
				float w;
                
				for(size_t i = 1; i < words.size(); i++){
					w = myFont.getBoundingBox(words[i], 0, 0).width + space_width;
					if(accum + w - space_width > width){
						accum = w;
						formated += "\n" + words[i];
					}else{
						accum += w;
						formated += " " + words[i];
					}
					
				}
				if(l < lines.size()-1){
					formated += "\n";
				}
			}
		}
	}
	return formated;
}

//--------------------------------------------------------------
void ofApp::draw(){
    

     int alpha = 255;

     imageColor = (45, 6, 255, alpha);
     fontColor = 0;

    //computer vision color tracking trigger
    color_detection.draw();
    
    //backgorund iamge
    ofPushStyle();
    ofFill();
    ofSetColor(imageColor);
    background_image.draw(0, 0, 2560, 1440);
    ofPopStyle();
    
    //loading vide
    welcome_video.draw(240, 260, 900, 900);
    
	if(runway.isServerAvailable()){
	//	glm::vec3 margin(0, 30, 30);

		// draw the text returned by runway and image generated from prompt
		if(!runwayResult.empty()){
            //4b -> the other way arond messes upthe style I think
            //left image
            ofPushStyle();
            ofFill();
            ofSetColor(imageColor);
            congressperson_to_draw.draw(240, 260, 900, 900);
            ofPopStyle();
            
            //4a
            //right text
            ofPushStyle();
            formatted_scandal = formatTextWidth(current_scandal, 370); //string, width of text
            ofSetColor(fontColor);
            myType.drawString(formatted_scandal, 1520, 660); // formatted
            ofPopStyle();
		}
    }
}

//--------------------------------------------------------------
void ofApp::trigger_scandal(){
    // 5
    if(!scandal_image_queue.empty()) {
        // choosing to display first mapped scandal and image
        current_scandal = scandal_image_queue.front().first;
        congressperson_to_draw = scandal_image_queue.front().second;
        scandal_image_queue.pop();

    }
    else {
        current_scandal = "The next scandal is being generated!";
        congressperson_to_draw = ofImage("loading02.jpg");
        welcome_video.play();//loading vide
        //welcome_video.draw(240, 260, 900, 900);
        
    }
}


// Runway sends information about the current model
//--------------------------------------------------------------
void ofApp::runwayInfoEvent(ofJson& info){
	ofLogNotice("ofApp::runwayInfoEvent") << info.dump(2);
}
// if anything goes wrong
//--------------------------------------------------------------
void ofApp::runwayErrorEvent(string& message){
	ofLogNotice("ofApp::runwayErrorEvent") << message;
}
//--------------------------------------------------------------

