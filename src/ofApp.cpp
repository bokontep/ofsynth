#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToFile("log.txt");
	font.load("unifont-13.0.02.ttf", 32, true);
	
    initWaveforms(true, 0.5,0.5);
	
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
	midiEventCount = midiInMessages.size();
	int newMessagesCount = midiInMessages.size();
	while(newMessagesCount>0)
	//for (ofxMidiMessage message:midiMessages)
	
	{
		this->midiInLock.lock();
		ofxMidiMessage message = midiInMessages.front();
		midiInMessages.pop_front();
		this->midiInLock.unlock();
		newMessagesCount--;
		int pitch = 0;
		switch (message.status)
		{
		case MIDI_NOTE_ON:
			if (message.pitch >= 36 && message.pitch <= 99)
			{
				pitch = midiMap.Map3(message.pitch, 36,5);
				if (triggerline)
					message.velocity = 127;
				engine->handleNoteOn(0, pitch, message.velocity);
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
			pitch = midiMap.Map3(message.pitch, 36,5);
			engine->handleNoteOff(0, pitch, message.velocity);
			{
				int y = (message.pitch - 36) / 8;
				int x = message.pitch - y * 8 - 36;
				if (x >= 0 && x <= 7 && y >= 0 && y <= 7)
				{
					pushSetPad(x, y, 0);
				}
			}
			
			break;
		case MIDI_PITCH_BEND:
			engine->handlePitchBend(0, message.value);
			break;
		case MIDI_CONTROL_CHANGE:
			engine->handleControlChange(0, message.control, message.value);
			break;

		}
	}
	//midiMessages.clear();
	/*
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

void ofApp::drawEditWaveTable()
{
    ofBackground(0.0,0.0,0.0,255.0);
    ofSetColor(0.0, 255.0, 10, 255.0);
    ofSetLineWidth(3.0);
	font.drawString("volna v0.1",20,33);
    font.drawString("EDIT WAVETABLE",20,66);
    char buf[255];
    sprintf(buf,"xwt=%03d ywt=%03d",xwt,ywt);
    font.drawString(buf,20,99);
    //int ypos = 768-128;
    //int yoffset = 300;
    //int xpos = 16;

    ofSetLineWidth(1.0);
    for(int i=0;i<18;i++)
    {
        int index = (this->wavetablepos+i)%WTCOUNT;
        ofDrawLine(xpos+i*WTLEN/4,ypos-32,xpos+(i+1)*(WTLEN/4),ypos-32);
        ofDrawLine(xpos+i*WTLEN/4,ypos+32,xpos+(i+1)*(WTLEN/4),ypos+32);
        ofDrawLine(xpos+i*WTLEN/4,ypos-32,xpos+i*WTLEN/4,ypos+64);
        ofDrawLine(xpos+(i+1)*(WTLEN/4),ypos-32,xpos+(i+1)*(WTLEN/4),ypos+64);

        for(int w=0;w<WTLEN/4-1;w=w+1)
        {
            int y1 = Waveforms[index*WTLEN+w*4]*32;
            if(y1>32)
            {
                y1=32;
            }
            if(y1<-32)
            {
                y1=-32;
            }

            int y2 = Waveforms[index*WTLEN+(w+1)*4]*32;
            if(y2>32)
            {
                y2=32;
            }
            if(y2<-32)
            {
                y2=-32;
            }
            ofDrawLine(xpos+w+i*WTLEN/4,ypos-y1,xpos+w+1+i*WTLEN/4,ypos-y2);
        }
        sprintf(buf, "%03d",index);
        font.drawString(buf,xpos+i*WTLEN/4,ypos+64);

    }
    for(int w=0;w<WTLEN-1;w++)
    {
        ofDrawLine(xpos + w, ypos-yoffset-Waveforms[this->wavetablepos*WTLEN+w]*128, xpos+w+1,ypos-yoffset-Waveforms[this->wavetablepos*WTLEN+w+1]*128);
    }
}


void ofApp::drawPlayMode()
{
    ofBackground(0.0,0.0,0.0,255.0);
    ofSetColor(0.0, 255.0, 10, 255.0);
    ofSetLineWidth(3.0);
    int sign = 0;
	float a = 2400;
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

            ofLine(4.0*x-i*xoffset, -ringbuf[startwaveform*WTLEN+(x + offset) % WTLEN] * a + 300.0+20*i, 4.0*(x + 1)- i*xoffset, -ringbuf[startwaveform*WTLEN+(x + 1 + offset) % WTLEN] * a + 300.0+20*i);

        }
        startwaveform = (startwaveform + i) % numwaveforms;
    }
	font.drawString("volna v0.1", 20, 33);
    font.drawString("PLAY",20,66);
    if (triggerline)
    {
        font.drawString("TRIGGER", 20, 99);
    }
    char buf[255];
	sprintf(buf, "%d", midiEventCount);
	font.drawString(buf, 20, 132);
    //sprintf(buf, "keycode:%d", this->currkey);
    //font.drawString(buf, 20, 132);

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
//--------------------------------------------------------------
void ofApp::draw(){
    switch(state)
    {
    case AS_PLAY:

        drawPlayMode();
        break;
    case AS_EDIT_WAVETABLE:
        drawEditWaveTable();
        break;
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
    if(key == 'e' || key == 'E')
    {
        state = AS_EDIT_WAVETABLE;
    }
    if(key == 'p' || key == 'P')
    {
        state = AS_PLAY;
    }
    if(key =='<' || key == ',')
    {
        if(state==AS_EDIT_WAVETABLE)
        {
            int v = this->wavetablepos;
            v = v-1;
            if(v<0)
            {
                v=WTCOUNT-1;
            }
            this->wavetablepos = v;
        }
    }
    if(key =='>' || key=='.')
    {
        if(state==AS_EDIT_WAVETABLE)
        {
            int v = this->wavetablepos;
            v = v+1;
            if(v>WTCOUNT-1)
            {
                v=0;
            }
            this->wavetablepos = v;
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
void ofApp::updateWavetable(int x, int y, int button){
    if(state==AS_EDIT_WAVETABLE)
    {
        int xwt = x-xpos;
        this->xwt = xwt;
        int ywt = y-ypos+yoffset;
        this->ywt = ywt;
        if(xwt>=0 && xwt<WTLEN)
        {
            if(ywt>=-128 && ywt<127)
            {
                Waveforms[this->currwaveform*WTLEN+xwt]=-((ywt/128.0));
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    int diffx = x-lastx;
    int diffy = y-lasty;
    int p = 1;
    if(x<lastx)
    {
        for(int xi=lastx;xi>=x;xi=xi-1)
        {
            updateWavetable(xi, y, 0);
        }
    }
    else
    {
        for(int xi=lastx;xi<x;xi=xi+1)
        {
            updateWavetable(xi,y,0);
        }
    }

    lastx = x;
    lasty = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    this->drawWavetable =true;
    updateWavetable(x,y,0);
    lastx = x;
    lasty = y;


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    this->drawWavetable = false;
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
    initWaveforms(false,0,0);
}
void ofApp::initWaveforms(bool bandlimit,float freq,float q)
{
	float sintable[WTLEN];
	float tritable[WTLEN];
	float squtable[WTLEN];
	float sawtable[WTLEN];
	float rndtable[WTLEN];
    LowPass lp;
    lp.SetParameters(freq,q);
	float max = 0.8;
	for (int i = 0; i < WTLEN; i++)
	{
		sintable[i] = max*sin(2.0 * (PI / (float)WTLEN) * i);


	}
	if (bandlimit)
	{
		lp.reset();
		for (int i = 0; i < 256; i++)
		{
	
			sintable[i] = lp.Process(sintable[i]);
		}
	}

    for (int i = 0; i <WTLEN/4;i++)
    {
        tritable[i] = i*(max/(WTLEN / 4));

    }
    for (int i = 0; i<128;i++)
    {
        tritable[i+64] = max*(+1.0-i*(2.0 / ((double)WTLEN / 2)));
    }

    for (int i = 0;i<64;i++)
    {
        tritable[i+128+64] = max*(-1.0 +i*(1.0 / ((double)WTLEN / 2)));
    }
    if(bandlimit)
    {
		lp.reset();
        for(int i=0;i<256;i++)
        {
			
            tritable[i] = lp.Process(tritable[i]);
        }
    }
    /*
	for (int i = 0; i < 128; i++)
	{
        tritable[i] = ((-1.0 + i * (1.0 / ((double)WTLEN / 2.0))));
        if(bandlimit)
        {
            tritable[i]=lp.Process(tritable[i]);
        }
	}
    for (int i = 0; i < 128; i++)
	{
        tritable[i+128] = ((1.0 - i * (1.0 / ((double)WTLEN / 2.0))));
        if(bandlimit)
        {
            tritable[i]=lp.Process(tritable[i]);
        }
	}
    */
	for (int i = 0; i < WTLEN; i++)
	{
		squtable[i] = max*(i < (WTLEN / 2) ? 1.0 : -1.0);
	}
	if (bandlimit)
	{
		lp.reset();
		for (int i = 0; i < WTLEN; i++)
		{
			squtable[i] = lp.Process(squtable[i]);
		}
	}

	for (int i = 0; i < 256; i++)
	{
		sawtable[i] = max*((-1.0 + (2.0 / WTLEN) * i));
        
	}
	if (bandlimit)
	{
		lp.reset();
		for (int i = 0; i < WTLEN; i++)
		{
			sawtable[i] = lp.Process(sawtable[i]);
		}
	}
	for (int i = 0; i < 256; i++)
	{
		rndtable[i] = max*ofRandom(-1,1);
		
	}
	if (bandlimit)
	{
		lp.reset();
		for (int i = 0; i < WTLEN; i++)
		{
			rndtable[i] = lp.Process(rndtable[i]);
		}
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
	midiInLock.lock();
	this->midiInMessages.push_back(msg);
	midiInLock.unlock();
	// add the latest message to the message queue
	char buf[255];
	//midiMessages.push_back(msg);
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
	/*
	while (midiMessages.size() > maxMessages) {
		midiMessages.erase(midiMessages.begin());
	}*/
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

