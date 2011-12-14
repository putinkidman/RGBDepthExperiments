#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	
	ofBackground(255*.2);
	
	cameraFound =  recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	cameraFound &= recordDepth.setup(&recordContext);
	cameraFound &= recordImage.setup(&recordContext);
	if(!cameraFound){
		ofLogError() << "RECORDER --- Camera not found";
	}
	
	recording = false;
	
	currentTab = TabRecord;
	
	downColor = ofColor(255, 120, 0);
	idleColor = ofColor(220, 200, 200);
	hoverColor = ofColor(255*.2, 255*.2, 30*.2);
	
	//setup buttons
	framewidth = 640;
	frameheight = 480;
	thirdWidth = framewidth/3;
	btnheight = 30;
	
	btnSetDirectory = new ofxMSAInteractiveObjectWithDelegate();
	btnSetDirectory->setPosAndSize(0, 0, framewidth, btnheight);
	btnSetDirectory->setLabel("Load Directory");
	btnSetDirectory->setIdleColor(idleColor);
	btnSetDirectory->setDownColor(downColor);
	btnSetDirectory->setHoverColor(hoverColor);
	btnSetDirectory->setDelegate(this);
	
	btnCalibrateTab = new ofxMSAInteractiveObjectWithDelegate();
	btnCalibrateTab->setPosAndSize(0, btnheight, thirdWidth, btnheight);
	btnCalibrateTab->setLabel("Calibrate");
	btnCalibrateTab->setIdleColor(idleColor);
	btnCalibrateTab->setDownColor(downColor);
	btnCalibrateTab->setHoverColor(hoverColor);
	btnCalibrateTab->setDelegate(this);
	
	btnRecordTab = new ofxMSAInteractiveObjectWithDelegate();
	btnRecordTab->setPosAndSize(thirdWidth, btnheight, thirdWidth, btnheight);
	btnRecordTab->setLabel("Record");
	btnRecordTab->setIdleColor(idleColor);
	btnRecordTab->setDownColor(downColor);
	btnRecordTab->setHoverColor(hoverColor);
	btnRecordTab->setDelegate(this);
	
	btnPlaybackTab = new ofxMSAInteractiveObjectWithDelegate();
	btnPlaybackTab->setPosAndSize(thirdWidth*2, btnheight, thirdWidth, btnheight);
	btnPlaybackTab->setLabel("Playback");
	btnPlaybackTab->setIdleColor(idleColor);
	btnPlaybackTab->setDownColor(downColor);
	btnPlaybackTab->setHoverColor(hoverColor);
	btnPlaybackTab->setDelegate(this);
	
	btnRecordBtn = new ofxMSAInteractiveObjectWithDelegate();
	btnRecordBtn->setPosAndSize(0, btnheight*2+frameheight, framewidth, btnheight);
	btnRecordBtn->setLabel("Record");
	btnRecordBtn->setIdleColor(idleColor);
	btnRecordBtn->setDownColor(downColor);
	btnRecordBtn->setHoverColor(hoverColor);
	btnRecordBtn->setDelegate(this);
	
	btnRenderBW = new ofxMSAInteractiveObjectWithDelegate();
	btnRenderBW->setPosAndSize(0, btnheight*3+frameheight, thirdWidth, btnheight);
	btnRenderBW->setLabel("Blaick&White");
	btnRenderBW->setIdleColor(idleColor);
	btnRenderBW->setDownColor(downColor);
	btnRenderBW->setHoverColor(hoverColor);
	btnRenderBW->setDelegate(this);
	
	btnRenderRainbow = new ofxMSAInteractiveObjectWithDelegate();
	btnRenderRainbow->setPosAndSize(thirdWidth, btnheight*3+frameheight, thirdWidth, btnheight);
	btnRenderRainbow->setLabel("Rainbow");
	btnRenderRainbow->setIdleColor(idleColor);
	btnRenderRainbow->setDownColor(downColor);
	btnRenderRainbow->setHoverColor(hoverColor);
	btnRenderRainbow->setDelegate(this);
	
	btnRenderPointCloud = new ofxMSAInteractiveObjectWithDelegate();
	btnRenderPointCloud->setPosAndSize(thirdWidth*2, btnheight*3+frameheight, thirdWidth, btnheight);
	btnRenderPointCloud->setLabel("Pointcloud");
	btnRenderPointCloud->setIdleColor(idleColor);
	btnRenderPointCloud->setDownColor(downColor);
	btnRenderPointCloud->setHoverColor(hoverColor);
	btnRenderPointCloud->setDelegate(this);
	

	updateTakeButtons();
	
	timeline.setup();
	timeline.setOffset(ofVec2f(0,btnRecordBtn->y+btnRecordBtn->height));
	timeline.addElement("depth sequence", &depthSequence);
	timeline.setWidth(ofGetWidth());
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	depthSequence.setup();
	
	ofxXmlSettings defaults;
	if(defaults.loadFile("defaults.xml")){
		loadDirectory(defaults.getValue("currentDir", ""));
	}
	else{
		loadDirectory("depthframes");
		//recorder.setRecordLocation("depthframes", "frame");
		
	}
	recorder.setup();	
}


//--------------------------------------------------------------

void testApp::update(){
	if(!cameraFound){
		return;
	}
	
	recordContext.update();
	if(currentTab == TabCalibrate){
		recordImage.update();
	}
	else if(currentTab == TabRecord){
//		recordImage.update();
		recordDepth.update();	
	}
	
	if(recordDepth.isFrameNew() && recording){
		//recorder.addImage( (unsigned short*)recordDepth.getRawDepthPixels(), recordImage.getIRPixels() );
		recorder.addImage( (unsigned short*)recordDepth.getRawDepthPixels());
	}
}

void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){
}
void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){

}
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	if(object == btnSetDirectory){
		loadDirectory();
	}
	else if(object == btnCalibrateTab){
		currentTab = TabCalibrate; 
	}
	else if(object == btnRecordTab){
		currentTab = TabRecord;
	}
	else if(object == btnPlaybackTab){
		currentTab = TabPlayback;
	}
	else if(object == btnRecordBtn){
		if(currentTab == TabRecord){
			toggleRecord();
		}
		else if(currentTab == TabCalibrate){
			captureCalibrationImage();
		}
		else if(currentTab == TabPlayback){
			//TODO
		}
	}
	else if(object == btnRenderBW){
		currentRenderMode = RenderBW;
	}
	else if(object == btnRenderRainbow){
		currentRenderMode = RenderRainbow;
	}
	else if(object == btnRenderPointCloud){
		currentRenderMode = RenderPointCloud;
	}
	else {
		if (currentTab == TabPlayback) {
			for(int i = 0; i < btnTakes.size(); i++){
				if(object == btnTakes[i]){
					loadSequenceForPlayback( i );
				}
			}
		}

	}
}

void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}


void testApp::loadDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Select Record Directory", true);
	if(r.bSuccess){
		loadDirectory(r.getPath());
	}

}
void testApp::loadDirectory(string path){
	recorder.setRecordLocation(path, "frame");
	btnSetDirectory->setLabel(path);
	updateTakeButtons();
	ofxXmlSettings defaults;
	defaults.loadFile("defaults.xml");
	defaults.setValue("currentDir", path);
	defaults.saveFile("defaults.xml");
}

void testApp::loadSequenceForPlayback( int index ){
	vector<string> paths = recorder.getTakePaths();
	//TODO add timeline stuff in here;
	depthSequence.loadSequence(paths[index]);
	timeline.setDurationInFrames(depthSequence.videoThumbs.size());
}

void testApp::toggleRecord(){
	recording = !recording;
	if(recording){
		recorder.incrementFolder();
	}	
	updateTakeButtons();
}

//--------------------------------------------------------------
void testApp::captureCalibrationImage(){
	calibrationImage.setFromPixels(recordImage.getIRPixels(), 640, 480, OF_IMAGE_GRAYSCALE);
	string filename = "__CalibFile_" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" + ofToString(ofGetSeconds()) +".png";			
	ofSaveImage( calibrationImage, filename);			
}

//--------------------------------------------------------------
void testApp::updateTakeButtons(){
	vector<string> files = recorder.getTakePaths();
	
	for(int i = 0; i < btnTakes.size(); i++){
		delete btnTakes[i];
	}
	btnTakes.clear();
	
	for(int i = 0; i < files.size(); i++){
		ofxMSAInteractiveObjectWithDelegate* btnTake = new ofxMSAInteractiveObjectWithDelegate();		
		btnTake->setPosAndSize(framewidth, btnheight/2*i, thirdWidth, btnheight/2);
		btnTake->setLabel( ofFilePath::getFileName(files[i]) );
		btnTake->setIdleColor(idleColor);
		btnTake->setDownColor(downColor);
		btnTake->setHoverColor(hoverColor);
		btnTake->setDelegate(this);
		btnTakes.push_back( btnTake );
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	

	if(currentTab == TabCalibrate){
		recordImage.draw(0, btnheight*2, 640, 480);
	}
	else if(currentTab == TabRecord){
		//TODO render modes
		recordDepth.draw(0,btnheight*2,640,480);
	}
	else {
		depthSequence.currentDepthImage.draw(0, btnheight*2, 640, 480);
		//draw timeline
		timeline.draw();
	}

	//draw save meter if buffer is getting full
	if(recording){
		ofPushStyle();
		ofSetColor(255, 0, 0);
		ofNoFill();
		ofSetLineWidth(5);
		
		ofRect(0, btnheight*2, 640, 480);
		ofPopStyle();
	}
	
	if(recorder.numFramesWaitingSave() > 0){
		ofPushStyle();
		float width = recorder.numFramesWaitingSave()/2000.0 * btnRecordBtn->width;
		ofFill();
		ofSetColor(255,0, 0);
		ofRect(btnRecordBtn->x,btnRecordBtn->y,width,btnRecordBtn->height);
		
		if(ofGetFrameNum() % 30 < 15){
			ofSetColor(255, 0, 0, 40);
			ofRect(*btnRecordBtn);
		}
		ofPopStyle();
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		if(currentTab == TabRecord){
			toggleRecord();
		}
		else if(currentTab == TabCalibrate){
			captureCalibrationImage();
		}
		else if(currentTab == TabPlayback){
			timeline.togglePlay();
		}
	}
    
    if(key == 'c'){
		captureCalibrationImage();
    }
}

void testApp::exit() {

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	timeline.setWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}