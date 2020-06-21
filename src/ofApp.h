#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "VAEngine.h"
const int WTCOUNT = 256;
const int WTLEN = 256;
const int maxnumwaveforms = 40;
class ofApp : public ofBaseApp, public ofxMidiListener {

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
		void exit();
		void initWaveforms();
		void pushDisplayMessage(int x, int y, char* message);
		void pushClearLine(int line);
		void pushClearScreen();
		void pushSetPad(int x, int y, int c);
		void pushUserMode();
		void pushLiveMode();
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
		//Midi In
		void newMidiMessage(ofxMidiMessage& eventArgs);

		ofxMidiIn midiIn;
		std::vector<ofxMidiMessage> midiMessages;
		std::size_t maxMessages = 10; //< max number of messages to keep track of

		//Midi Out
		ofxMidiOut midiOut;
		int channel;

		unsigned int currentPgm;
		int note, velocity;
		int pan, bend, touch, polytouch;

		ofxMidiOut midiOut2;
private:
	uint64_t lastTime = 0;
	uint64_t counter = 0;
	bool toggle = true;

				
};
