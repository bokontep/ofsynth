#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToFile("log.txt");
	font.load("unifont-13.0.02.ttf", 32, true);
	
	initWaveforms();
	
	//setup output port to control ableton push parameters
	midiOut.listOutPorts();
	// connect

	//	[notice] ofxMidiOut: 3 ports available
	//	[notice] ofxMidiOut : 0 : Microsoft GS Wavetable Synth 0
	//	[notice] ofxMidiOut : 1 : Ableton Push 1
	//	[notice] ofxMidiOut : 2 : MIDIOUT2(Ableton Push) 2
	midiOut2.openPort("Ableton Push 1");
	midiOut.openPort("MIDIOUT2 (Ableton Push) 2");
	std::vector<string> portNames;
	portNames = midiOut.getOutPortList();
	ofLogNotice() << "Output midi port names" << endl;
	for (int i = 0; i < portNames.size(); i++)
	{
		ofLogNotice()  << portNames[i].c_str() << endl;
	}
	
	
	//midiOut.openPort("MIDIOUT2(Ableton Push)");
	//setup input port to read notes from ableton push
	midiIn.listInPorts();

	// open port by number (you may need to change this)
	midiIn.openPort("MIDIIN2 (Ableton Push) 1");
	// don't ignore sysex, timing, & active sense messages,
		// these are ignored by default
	midiIn.ignoreTypes(false, false, false);

	// add ofApp as a listener
	midiIn.addListener(this);

		
	// print received messages to the console
	midiIn.setVerbose(true);
	//pushLiveMode();
	pushClearScreen();
	pushDisplayMessage(0, 0, "BOKONTEP WAVETABLESYNTH V.0.1");
	pushDisplayMessage(0, 1, "(C) 2020 ***COVID19***");
	engine = new VAEngine<16,256,256>(&this->Waveforms[0]);
	engine->init(44100);
	soundStream = new ofSoundStream();
	engine->setADSR(30, 30, 127, 20);
	lastTime = ofGetElapsedTimeMillis();
	//ofSoundStreamSetup(2, 0);
	soundStream->setup(2, 0, 44100, 256, 1);
	for (int y = 0; y < 9; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			pushSetPadRGB(x, y, 8 * x + 8, 8 * y + y, 8 * x + y);
		}
	}
	pushSetPadRGB(0, 0, 127, 127, 127);
	pushSetPadRGB(1, 0, 255, 255, 255);
	pushSetPadRGB(2, 0, 255, 0, 0);
	pushSetPadRGB(3, 0, 0, 255, 0);
	pushSetPadRGB(4, 0, 0, 0, 255);
	pushSetPadRGB(5, 0, 255, 255, 0);
	pushSetPadRGB(6, 0, 0, 255, 255);
	pushSetPadRGB(7, 0, 255, 0, 255);

}


//--------------------------------------------------------------
void ofApp::update(){

	int offset = 0;
	int semitones = 108;
	uint64_t now = ofGetElapsedTimeMillis();
	for (ofxMidiMessage message:midiMessages)
	{
		switch (message.status)
		{
		case MIDI_NOTE_ON:
			if (message.pitch >= 36 && message.pitch <= 99)
			{
				if (triggerline)
					message.velocity = 127;
				engine->handleNoteOn(0, message.pitch, message.velocity);
				{
					int y = (message.pitch - 36) / 8;
					int x = message.pitch - y * 8 - 36;
					if (x >= 0 && x <= 7 && y >= 0 && y <= 7)
					{
						pushSetPad(x, y, 3);
					}
				}
			}
			break;
		case MIDI_NOTE_OFF:
			engine->handleNoteOff(0, message.pitch, message.velocity);
			{
				int y = (message.pitch - 36) / 8;
				int x = message.pitch - y * 8 - 36;
				if (x >= 0 && x <= 7 && y >= 0 && y <= 7)
				{
					pushSetPad(x, y, 0);
				}
			}
			break;
			break;
		case MIDI_PITCH_BEND:
			engine->handlePitchBend(0, message.value);
			break;
		case MIDI_CONTROL_CHANGE:
			engine->handleControlChange(0, message.control, message.value);
			break;

		}
	}
	midiMessages.clear();/*
	if (now - lastTime > notelength)
	{
		lastTime = now;
		if (toggle) {
			engine->handleNoteOn(0, offset + counter % semitones, 127);
			engine->handleNoteOn(0, offset + counter % semitones +5, 127);
			engine->handleNoteOn(0, offset + counter % semitones +9, 127);

			toggle = !toggle;
		}
		else
		{
			engine->handleNoteOff(0, offset + counter % semitones,0);
			engine->handleNoteOff(0, offset + counter % semitones + 5, 0);
			engine->handleNoteOff(0, offset + counter % semitones + 9, 0);

			toggle = !toggle;
			engine->setOsc1Wave(counter % 256);
			engine->setOsc2Wave(counter % 256);
			engine->setPwm(counter % 128);
			this->counter++;
			lastTime = now + notelength;
		}
		
		//this->counter++;
	}
	*/
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0.0,0.0,0.0,255.0);
	ofSetColor(0.0, 255.0, 10, 255.0);
	ofSetLineWidth(3.0);
	int sign = 0;
	if (ringbuf[currwaveform * 256] >= 0)
	{
		sign = 1;
	}
	else
	{
		sign = -1;
	}

	int offset = 0;
	for (int i = 1; i < 255; i++)
	{
		if ((ringbuf[currwaveform*256+i] > 0 && sign == -1)||(ringbuf[currwaveform*256+i]<0 && sign == 1))
		{
			offset = i;
			break;
		}
	}
	if (!triggerline)
	{
		offset = 0;
	}
	int startwaveform = (currwaveform+1)%numwaveforms;
	if (numwaveforms == 1)
	{
		startwaveform = 0;
	}
	for (int i = 0; i < numwaveforms;i++)
	{
		for (int x = 0; x < 255; x++)
		{
			ofSetColor(0.0, (i+1)*(255.0/numwaveforms), 10, 255.0);

			ofLine(4.0*x-i*xoffset, ringbuf[startwaveform*256+(x + offset) % 256] * 500.0 + 300.0+20*i, 4.0*(x + 1)- i*xoffset, ringbuf[startwaveform*256+(x + 1 + offset) % 256] * 500.0 + 300.0+20*i);
		
		}
		startwaveform = (startwaveform + i) % numwaveforms;
	}
	if (triggerline)
	{
		font.drawString("TRIGGER", 20, 32);
	}
	char buf[255];
	sprintf(buf, "keycode:%d", this->currkey);
	font.drawString(buf, 20, 66);
	
	if (!mididisplay.empty())
	{
		while (mididisplay.size() > 20)
		{
			mididisplay.pop_front();
		}
	std:list < std::string >::iterator it;
		int y = 30;
		for (it = mididisplay.begin(); it != mididisplay.end(); it++)
		{
			font.drawString(it->c_str(), 600, y);
			y = y + 30;
		}
		
	}

}

void ofApp::audioOut(float * output, int bufferSize, int nChannels)
{
	int count = 0;
	for (int i = 0; i < nChannels*bufferSize ; i = i+2) {
		float sample = engine->Process(); // generating a sine wave sample
		ringbuf[currwaveform*256+(count++ % 256)] = sample;
		for(int c=0;c<nChannels;c++)
		{
			output[i+c] = sample;
		}
		 // writing to the left channel
		//output[i + 1] = sample; // writing to the right channel
	}
	currwaveform = (currwaveform+1) % numwaveforms;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 't' || key == 'T')
	{
		triggerline = !triggerline;
	}
	if (key == '+')
	{
		xoffset = xoffset + 1;
	}
	if (key == '-')
	{
		xoffset = xoffset - 1;
	}
	if (key == 'q' || key == 'Q')
	{
		if (numwaveforms < maxnumwaveforms)
		{
			numwaveforms++;
		}
	}
	if (key == 'a' || key == 'A')
	{
		if (numwaveforms > 1)
		{
			numwaveforms--;
		}
	}
	if (key == 'z' || key == 'Z')
	{
		if (notelength > 10)
		{
			notelength--;
		}
	}
	if (key == 'x' || key == 'X')
	{
		if (notelength < 5000)
		{
			notelength++;
		}
	}
	currkey = key;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

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

void ofApp::initWaveforms()
{
	float sintable[WTLEN];
	float tritable[WTLEN];
	float squtable[WTLEN];
	float sawtable[WTLEN];
	float rndtable[WTLEN];
	for (int i = 0; i < WTLEN; i++)
	{
		sintable[i] = ((sin(2.0 * (PI / (float)WTLEN) * i)));
	}
	
	
	for (int i = 0; i < 128; i++)
	{
		tritable[i] = ((-1.0 + i * (1.0 / ((double)WTLEN / 2.0))));
	}
	for (int i = 128; i < 256; i++)
	{
		tritable[i] = ((1.0 - i * (1.0 / ((double)WTLEN / 2.0))));
	}
	for (int i = 0; i < 256; i++)
	{
		squtable[i] = (i < (WTLEN / 2) ? 1.0 : -1.0);
	}
	for (int i = 0; i < 256; i++)
	{
		sawtable[i] = ((-1.0 + (2.0 / WTLEN) * i));
	}
	for (int i = 0; i < 256; i++)
	{
		rndtable[i] = ofRandom(-1,1);
	}
	float sinf = 1.0;
	float trif = 0.0;
	float squf = 0.0;
	float sawf = 0.0;
	float rndf = 0.0;
	float l = (float)WTCOUNT / 5.0;
	int counter = 0;
	float f = (1.0 / (float)l);
 	for (int w = 0; w < WTCOUNT; w++)
	{
		for (int i = 0; i < 256; i++)
		{
			Waveforms[w*WTLEN + i] = sinf * sintable[i] + trif * tritable[i] + squf * squtable[i] + sawf * sawtable[i] + rndf * rndtable[i];
		}
		if (counter <= l)
		{
			sinf = sinf - f;
			trif = trif + f;
		}
		else if (counter > l && counter <= 2 * l)
		{
			sinf = 0.0;
			trif = trif - f;
			squf = squf + f;
		}
		else if (counter >= 2 * l && counter < 3 * l)
		{
			trif = 0.0;
			squf = squf - f;
			sawf = sawf + f;
		}
		else if (counter >= 3 * l && counter < 4 * l)
		{
			squf = 0.0;
			sawf = sawf - f;
			rndf = rndf + f;
		}
		else
		{
			sawf = 0.0;
			rndf = rndf - f;
			sinf = sinf + f;
		}
		counter++;
	}
		
		
		
	
}

void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// add the latest message to the message queue
	char buf[255];
	midiMessages.push_back(msg);
	if (msg.status == MIDI_CONTROL_CHANGE)
	{
		sprintf(buf, "CC%02d|%+03d|%+03d", msg.channel, msg.control, msg.value);
		mididisplay.push_back(buf);
	}
	if (msg.status == MIDI_NOTE_ON)
	{
		sprintf(buf, "NN%02d-%03d-%03d", msg.channel, msg.pitch, msg.velocity);
		mididisplay.push_back(buf);
	}
	if (msg.status == MIDI_NOTE_OFF)
	{
		sprintf(buf, "NF%02d-%03d-%03d", msg.channel, msg.pitch, msg.velocity);
		mididisplay.push_back(buf);
	}
	
	// remove any old messages if we have too many
	while (midiMessages.size() > maxMessages) {
		midiMessages.erase(midiMessages.begin());
	}
}

void ofApp::pushDisplayMessage(int x, int y, char* message)
{
	int len = (strlen(message)%68);
	std::vector<uint8_t> msg;
	msg.push_back(MIDI_SYSEX);
	msg.push_back(71);
	msg.push_back(127);
	msg.push_back(21);
	msg.push_back(24 + y % 4);
	msg.push_back(0);
	msg.push_back(len + 1);
	msg.push_back(x % 68);
	for (int i = 0; i < len; i++)
	{
		msg.push_back(message[i]);
	}
	msg.push_back(MIDI_SYSEX_END);
	midiOut.sendMidiBytes(msg);
}

void ofApp::pushClearLine(int line)
{
	std::vector<uint8_t> msg;
	msg.push_back(MIDI_SYSEX);
	msg.push_back(71);
	msg.push_back(127);
	msg.push_back(21);
	msg.push_back(28 + line % 4);
	msg.push_back(0);
	msg.push_back(0);
	msg.push_back(MIDI_SYSEX_END);
	midiOut.sendMidiBytes(msg);
}


void ofApp::pushClearScreen()
{
	pushClearLine(0);
	pushClearLine(1);
	pushClearLine(2);
	pushClearLine(3);

}

void ofApp::pushUserMode()
{
	midiOut.sendMidiByte(MIDI_SYSEX);
	midiOut.sendMidiByte(71);
	midiOut.sendMidiByte(127);
	midiOut.sendMidiByte(21);
	midiOut.sendMidiByte(98);
	midiOut.sendMidiByte(0);
	midiOut.sendMidiByte(1);
	midiOut.sendMidiByte(1);
	midiOut.sendMidiByte(MIDI_SYSEX_END);
}

void ofApp::pushLiveMode()
{
	midiOut.sendMidiByte(MIDI_SYSEX);
	midiOut.sendMidiByte(71);
	midiOut.sendMidiByte(127);
	midiOut.sendMidiByte(21);
	midiOut.sendMidiByte(98);
	midiOut.sendMidiByte(0);
	midiOut.sendMidiByte(1);
	midiOut.sendMidiByte(0);
	midiOut.sendMidiByte(MIDI_SYSEX_END);
}

void ofApp::pushSetPad(int x, int y, int c)
{
	int note = y * 8 + 36 + x;
	//midiOut.sendNoteOn(0,note , 3);
	//midiOut2.sendNoteOn(0, note, 3);
	midiOut.sendNoteOn(1, note, c);
	//midiOut2.sendNoteOn(0, note, 3);
}
void ofApp::pushSetPadRGB(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
	x = x % 8;
	y = y % 9;
	uint8_t r1 = r>>4;
	uint8_t r2 = r%16;
	uint8_t g1 = g>>4;
	uint8_t g2 = g%16;
	uint8_t b1 = b>>4;
	uint8_t b2 = b%16;

	std::vector<uint8_t> msg;
	msg.push_back(MIDI_SYSEX);
	msg.push_back(71);
	msg.push_back(127);
	msg.push_back(21);
	msg.push_back(4);
	msg.push_back(0);
	msg.push_back(8);
	msg.push_back(x+y*8);
	msg.push_back(0);
	msg.push_back(r1);
	msg.push_back(r2);
	msg.push_back(g1);
	msg.push_back(g2);
	msg.push_back(b1);
	msg.push_back(b2);

	msg.push_back(MIDI_SYSEX_END);
	midiOut.sendMidiBytes(msg);
}
void ofApp::exit()
{

	midiIn.closePort();
	midiOut.closePort();
	midiOut2.closePort();
	midiIn.removeListener(this);
	soundStream->close();
}

