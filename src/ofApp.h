#pragma once

#include "ofMain.h"

//	Addons
#include "ofxKinectForWindows2.h"
#include "ofxOpenCv.h"

//	Project headers
#include "POP.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:

	//	Window
	float width, height;

	ofPlanePrimitive screenQuad;

	//	Kinect
	ofxKFW2::Device kinect;

	//	Open CV
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	ofxCvGrayscaleImage kDataImg;

	int nearThreshold;
	int farThreshold;

	//	Rain

	POP rain;

	ofFbo rainBuffer;
	ofFbo tmpFbo;
};
