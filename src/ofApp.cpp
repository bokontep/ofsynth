#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	font.load("digi7.ttf", 32, true);
	
	initWaveforms();
	
	//setup output port to control ableton push parameters
	midiOut.listOutPorts();
	// connect

	//	[notice] ofxMidiOut: 3 ports available
	//	[notice] ofxMidiOut : 0 : Microsoft GS Wavetable Synth 0
	//	[notice] ofxMidiOut : 1 : Ableton Push 1
	//	[notice] ofxMidiOut : 2 : MIDIOUT2(Ableton Push) 2
	midiOut2.openPort(1);
	midiOut.openPort(2);
	//midiOut.openPort("MIDIOUT2(Ableton Push)");
	//setup input port to read notes from ableton push
	midiIn.listInPorts();

	// open port by number (you may need to change this)
	midiIn.openPort(1);
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
			engine->handleNoteOn(0, message.pitch, message.velocity);
			{
				int y = (message.pitch - 36) / 8;
				int x = message.pitch - y * 8 - 36;
				if (x >= 0 && x <= 7 && y >= 0 && y <= 7)
				{
					pushSetPad(x, y, 3);
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
		for (int i = 0; i < WTLEN; i++)
		{
			Waveforms[i] = ((sin(2.0 * (PI / (float)WTLEN) * i)));
		}
		for (int i = 0; i < 128; i++)
		{
			Waveforms[WTLEN + i] = ((-1.0 + i * (1.0 / ((double)WTLEN / 2.0))));
		}
		for (int i = 128; i < 256; i++)
		{
			Waveforms[WTLEN + i] = ((1.0 - i * (1.0 / ((double)WTLEN / 2.0))));
		}
		for (int i = 0; i < 256; i++)
		{
			Waveforms[2 * WTLEN + i] = (i < (WTLEN / 2) ? 1.0 : -1.0);
		}
		for (int i = 0; i < 256; i++)
		{
			Waveforms[3 * WTLEN + i] = ((-1.0 + (2.0 / WTLEN) * i));
		}
		for (int i = 0; i < WTLEN; i++)
		{
			Waveforms[4 * WTLEN + i] = -1.0 + rand()* (2.0 / (1UL << 31));
		}

		for (int w = 5; w < WTCOUNT; w++)
		{
			for (int i = 0; i < 256; i++)
			{
				float f1 = (WTCOUNT - w) / 120;
				float f2 = ((WTCOUNT - w) % 120) / 110.0;
				float f3 = ((WTCOUNT - w) % 62) / 60.0;
				float f4 = ((WTCOUNT - w) % 31) / 31.0;
				Waveforms[w*WTLEN + i] = ((f1*Waveforms[i] + f2 * Waveforms[WTLEN + i] + f3 * Waveforms[2 * WTLEN + i] + f4 * Waveforms[3 * WTLEN + i]) / 4);
			}
		}
	
}

void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// add the latest message to the message queue
	midiMessages.push_back(msg);

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

void ofApp::exit()
{

	midiIn.closePort();
	midiOut.closePort();
	midiOut2.closePort();
	midiIn.removeListener(this);
	soundStream->close();
}

