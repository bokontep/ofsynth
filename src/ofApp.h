#pragma once

#include "ofMain.h"
#include "VAEngine.h"
const int WTCOUNT = 256;
const int WTLEN = 256;
const int maxnumwaveforms = 40;
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void audioOut(float * output, int bufferSize, int nChannels);
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
		void initWaveforms();
		ofTrueTypeFont font;
		int currkey=0;
		VAEngine<16,256,256>* engine;
		float Waveforms[WTCOUNT*WTLEN];
		ofSoundStream* soundStream;
		float ringbuf[256*maxnumwaveforms];
		bool triggerline = true;
		int currwaveform = 0;
		int numwaveforms = 1;
		int xoffset = 2;
		int notelength = 300;
private:
	uint64_t lastTime = 0;
	uint64_t counter = 0;
	bool toggle = true;

				
};
