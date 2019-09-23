#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//	OF Settings
	ofSetWindowShape(1920, 1200);
	ofSetFrameRate(60);

	//	Window
	width = ofGetWidth();
	height = ofGetHeight();

	//	Rain
	rain.setup(256 * 128);

	ofFbo::Settings s;
	s.internalformat = GL_RGBA32F_ARB;
	s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
	s.minFilter = GL_NEAREST;
	s.maxFilter = GL_NEAREST;
	s.wrapModeHorizontal = GL_CLAMP;
	s.wrapModeVertical = GL_CLAMP;
	s.width = width;
	s.height = height;
	s.numColorbuffers = 1;
	rainBuffer.allocate(s);

	rainBuffer.begin();
	ofClear(0);
	rainBuffer.end();

	//	CV
	grayImage.allocate(512, 424);
	grayThreshNear.allocate(512, 424);
	grayThreshFar.allocate(512, 424);
	kDataImg.allocate(width, height);

	nearThreshold = 5;
	farThreshold = 0;

	//	Kinect
	kinect.open();
	kinect.initDepthSource();

	tmpFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F_ARB);
}

//--------------------------------------------------------------
void ofApp::update(){

	//	Kinect
	kinect.update();

	//	CV
	if (kinect.isFrameNew())
	{
		grayImage.setFromPixels(kinect.getDepthSource()->getPixels());

		grayThreshNear = grayImage;
		grayThreshFar = grayImage;
		grayThreshNear.threshold(nearThreshold, true);
		grayThreshFar.threshold(farThreshold);
		cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);

		grayImage.flagImageChanged();

		kDataImg.scaleIntoMe(grayImage, CV_INTER_LINEAR);
		kDataImg.updateTexture();

		//	Update POP interaction data
		rain.updateKinectData(kDataImg.getTexture());
	}

	//	Update rain
	rain.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

	//	Kinect Data
	//ofSetColor(64, 64, 64);
	ofBackground(0);
	//kDataImg.draw(0, 0);

	//	Rain
	ofSetColor(255);
	rainBuffer.begin();
	ofBackgroundGradient(ofColor(37, 27, 52, 8), ofColor(8, 4, 13, 8));
	rain.draw();
	rainBuffer.end();
	rainBuffer.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
	case 'r':
		rain.loadShaders();
		break;
	default:
		break;
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

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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

}
