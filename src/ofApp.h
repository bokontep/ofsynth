#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "VAEngine.h"
#include "MidiMap.h"
#include "Fileops.h"
const int WTCOUNT = 256;
const int WTLEN = 256;
const int maxnumwaveforms = 40;
const int midilines = 20;
enum appstate{AS_PLAY=0,AS_EDIT_WAVETABLE};
class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void draw();
        void drawPlayMode();
        void drawEditWaveTable();

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
        void initWaveforms(bool bandlimit, float f, float q);
		void pushDisplayMessage(int x, int y, char* message);
		void pushClearLine(int line);
		void pushClearScreen();
		void pushSetPad(int x, int y, int c);
		void pushUserMode();
		void pushLiveMode();
		void pushSetPadRGB(int x, int y, uint8_t r, uint8_t g, uint8_t b);
        void updateWavetable(int x, int y, int button);
		int midiEventCount = 0;
		std::list<string> mididisplay;
		std::deque<ofxMidiMessage> midiInMessages;
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
        int wavetablepos=0;
        int startwavetable = 0;
        int endwavetable = 0;
        appstate state = AS_PLAY;
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
    int ypos = 768-128;
    int yoffset = 300;
    int xpos = 16;
    int ywt = 0;
    int xwt = 0;
    bool drawWavetable = false;
    int lastx;
    int lasty;
	ofMutex midiInLock;
	MidiMap midiMap;
	long colorCounter = 0;
};
