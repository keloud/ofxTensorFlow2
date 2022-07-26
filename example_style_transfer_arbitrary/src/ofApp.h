#pragma once

#include "ofMain.h"
#include "ofxTensorFlow2.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxTF2::Model model;
	cppflow::tensor input;
	cppflow::tensor style;
	std::vector<cppflow::tensor> inputVector;
	std::vector<cppflow::tensor> output;
	ofVideoPlayer videoPlayer;
	ofFloatPixels floatPixels;
	ofFloatImage imgOut;

	int resultWidth = 640;
	int resultHeight = 480;
};