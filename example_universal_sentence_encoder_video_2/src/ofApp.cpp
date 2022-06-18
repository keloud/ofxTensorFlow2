#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	cppflow::tensor tensor;
	std::vector<double> vec;
	std::vector<std::string> dialogue;
	int counter = -1;

	ofSetWindowTitle("example_universal_sentence_encoder");
	videoPlayer.load("Frenzy.mp4");

	// use only a portion of the GPU memory & grow as needed
	if (!ofxTF2::setGPUMaxMemory(ofxTF2::GPU_PERCENT_70, true)) {
		ofLogError() << "failed to set GPU Memory options!";
	}

	// load the model, bail out on error
	if (!model.load("model")) {
		std::exit(EXIT_FAILURE);
	}
	model.setup({ {"serving_default_inputs:0"} }, { {"StatefulPartitionedCall_1:0"} });

	SubtitleParserFactory* subParserFactory = new SubtitleParserFactory(ofToDataPath("Frenzy.srt"));
	SubtitleParser* parser = subParserFactory->getParser();
	sub = parser->getSubtitles();
	for (auto element : sub) {
		dialogue.push_back(element->getDialogue());
		if (element->getDialogue().back() == '.' || element->getDialogue().back() == '?' || element->getDialogue().back() == '!' || element->getDialogue().back() == '"' || element->getDialogue().back() == '\'' || element->getDialogue().back() == ';') {
			std::string currentDialogue = ofJoinString(dialogue, " ");
			dialogue.clear();
			tensor = model.runModel(cppflow::reshape(cppflow::tensor(currentDialogue), { -1 }));
			ofxTF2::tensorToVector(tensor, vec);
			vector_sub.push_back(std::make_pair(vec, std::make_pair(element->getSubNo() - counter, counter)));
			counter = -1;
		}
		counter++;
	}
	vector_sub_copy = vector_sub;
	std::cout << "Subtitles loaded." << std::endl;
	currentVector = vector_sub_copy[0].first;
	currentSubNo = vector_sub_copy[0].second.first;
	currentSubLenght = vector_sub_copy[0].second.second;
	vector_sub_copy.erase(vector_sub_copy.begin());
	videoPlayer.play();
}

//--------------------------------------------------------------
void ofApp::update() {
	videoPlayer.update();
	if (sub[currentSubNo - 1. + currentSubLenght]->getEndTime() + ((sub[currentSubNo + currentSubLenght]->getStartTime() - sub[currentSubNo - 1. + currentSubLenght]->getEndTime()) / 2.) < videoPlayer.getPosition() * videoPlayer.getDuration() * 1000 ||  videoPlayer.getIsMovieDone()) {
		std::vector<double> cosine;
		for (int x = 0; x < vector_sub_copy.size(); x++) {
			double cosine_similarity = 0;
			for (int i = 0; i < vector_sub_copy[x].first.size(); i++) {
				cosine_similarity += currentVector[i] * vector_sub_copy[x].first[i];
			}
			cosine.push_back(cosine_similarity);
		}
		int maxElementIndex = std::max_element(cosine.begin(), cosine.end()) - cosine.begin();
		double maxElement = *std::max_element(cosine.begin(), cosine.end());
		show = "Subtitle: " + ofToString(vector_sub_copy[maxElementIndex].second.first) + " - " + ofToString(vector_sub_copy[maxElementIndex].second.first + vector_sub_copy[maxElementIndex].second.second) + ".\n\nCosine similarity: " + ofToString(maxElement) + ".\n\nSubtitles left: " + ofToString(vector_sub_copy.size() - 1) + ".";
		currentVector = vector_sub_copy[maxElementIndex].first;
		currentSubNo = vector_sub_copy[maxElementIndex].second.first;
		currentSubLenght = vector_sub_copy[maxElementIndex].second.second;
		vector_sub_copy.erase(vector_sub_copy.begin() + maxElementIndex);
		if (vector_sub_copy.size() < 1) {
			vector_sub_copy = vector_sub;
		}
		if (currentSubNo > 1) {
			videoPlayer.setPosition((sub[currentSubNo - 2.]->getEndTime() + ((sub[currentSubNo - 1.]->getStartTime() - sub[currentSubNo - 2.]->getEndTime()) / 2.)) / videoPlayer.getDuration() / 1000);
		}
		else {
			videoPlayer.setPosition(0);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	videoPlayer.draw(450, 30, 300, 200);
	ofDrawBitmapStringHighlight("Press a key!", 50, 50);
	ofDrawBitmapString(show, 50,100);
	for (int i = currentSubNo - 1; i <= currentSubNo - 1 + currentSubLenght; i++) {
		if (sub[i]->getStartTime() < videoPlayer.getPosition() * videoPlayer.getDuration() * 1000 && sub[i]->getEndTime() > videoPlayer.getPosition() * videoPlayer.getDuration() * 1000) {
			ofDrawBitmapString(sub[i]->getDialogue(), 460, 250);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	std::vector<double> cosine;
	for (int x = 0; x < vector_sub_copy.size(); x++) {
		double cosine_similarity = 0;
		for (int i = 0; i < vector_sub_copy[x].first.size(); i++) {
			cosine_similarity += currentVector[i] * vector_sub_copy[x].first[i];
		}
		cosine.push_back(cosine_similarity);
	}
	int maxElementIndex = std::max_element(cosine.begin(), cosine.end()) - cosine.begin();
	double maxElement = *std::max_element(cosine.begin(), cosine.end());
	show = "Subtitle: " + ofToString(vector_sub_copy[maxElementIndex].second.first) + " - " + ofToString(vector_sub_copy[maxElementIndex].second.first + vector_sub_copy[maxElementIndex].second.second) + ".\n\nCosine similarity: " + ofToString(maxElement) + ".\n\nSubtitles left: " + ofToString(vector_sub_copy.size() - 1) + ".";
	currentVector = vector_sub_copy[maxElementIndex].first;
	currentSubNo = vector_sub_copy[maxElementIndex].second.first;
	currentSubLenght = vector_sub_copy[maxElementIndex].second.second;
	vector_sub_copy.erase(vector_sub_copy.begin() + maxElementIndex);
	if (vector_sub_copy.size() < 1) {
		vector_sub_copy = vector_sub;
	}
	if (currentSubNo > 1) {
		videoPlayer.setPosition((sub[currentSubNo - 2]->getEndTime() + ((sub[currentSubNo - 1]->getStartTime() - sub[currentSubNo - 2]->getEndTime()) / 2)) / videoPlayer.getDuration() / 1000);
	}
	else {
		videoPlayer.setPosition(0);
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}