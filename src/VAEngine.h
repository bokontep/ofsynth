#ifndef VAEngine_h_
#define VAEngine_h_
#include "SynthVoice.h"
 
template <int numvoices,int WAVEFORM_COUNT, int WTLEN> class VAEngine 
{
  public:
    VAEngine(float* newwaveforms)
    {
      this->waveforms = newwaveforms;
    }
    ~VAEngine()
    {
      
    }
    void init(float sampleRate)
    {
      for(int i=0;i<numvoices;i++)
      {
        for(int j=0;j<WAVEFORM_COUNT;j++)
        {
          
          mSynthVoice[i].SetSampleRate(sampleRate);
          mSynthVoice[i].AddOsc1SharedWaveTable(WTLEN,&waveforms[j*WTLEN]);
          mSynthVoice[i].AddOsc2SharedWaveTable(WTLEN,&waveforms[j*WTLEN]);
        }
        voices_notes[i]=-1;
      }
      
    }
	
    void update(void)
    {
        /*
      audio_block_t *block;
      uint32_t i;

      block = allocate();
      
      for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
        block->data[i] = Process()*8192;
      }
      transmit(block);
      release(block);
      */
    }
    float Process()
    {
      float s=0.0f;
      for (int i = 0; i < numvoices; i++)
      {
        s = s + (mSynthVoice[i].Process() );
      }
      // return s/numvoices;
	  return s/(activenotes+2);
	  
    }
    
	void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
      //bool found = false;
      int maxnote = -1;
      int maxnoteidx = -1;
      for (int i = 0; i < numvoices; i++)
      {
        if (voices_notes[i] == -1)
        {
          voices_notes[i] = note;
		  
          mSynthVoice[i].MidiNoteOn(note, velocity);
		  activenotes++;
		  //found = true;
          return;
        }
        if (voices_notes[i] > maxnote)
        {
          maxnote = voices_notes[i];
          maxnoteidx = i;
        }
      }
      voices_notes[maxnoteidx] = note;
      mSynthVoice[maxnoteidx].MidiNoteOn(note, velocity);
	  activenotes++;
    }
    
    void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
    {
      //digitalWrite(LED, LOW);
      for (int i = 0; i < numvoices; i++)
      {
        if (voices_notes[i] == note)
        {
          voices_notes[i] = -1;
          mSynthVoice[i].MidiNoteOff();
		  activenotes--;
          //break;
        }
      }
    }
    void handlePitchBend(uint8_t channel, uint8_t bendlsb, uint8_t bendmsb)
    {
      
      uint16_t bend = bendmsb<<7 | bendlsb;
      for(int i=0;i<numvoices;i++)
      {
        
        mSynthVoice[i].MidiBend(bend);
      }
        
    }
    
	void handleSetTet(float tet)
	{
		for (int i = 0; i < numvoices; i++)
		{
			mSynthVoice[i].SetTet(tet);
		}
	}
	void handleSetTune(float tune)
	{
		for (int i = 0; i < numvoices; i++)
		{
			mSynthVoice[i].SetTune(tet);
		}
	}
    void handlePitchBend(uint8_t channel, int bend)
    {
      
      for(int i=0;i<numvoices;i++)
      {
        if(mSynthVoice[i].IsPlaying())
        {
          mSynthVoice[i].MidiBend(bend);
        }
      }
      
      
    }
    
	void setADSR(uint8_t a, uint8_t d, uint8_t s, uint8_t r)
	{
		for (int i = 0; i < numvoices; i++)
		{
			mSynthVoice[i].SetOsc1ADSR(a,d,s,r);
			mSynthVoice[i].SetOsc2ADSR(a,d,s,r);
		}
	}
	void setOsc1Wave(uint8_t w)
	{
		this->osc1Wave = w;
		for (int i = 0; i < numvoices; i++)
		{
			mSynthVoice[i].MidiOsc1Wave(w);
		}
	}
	uint8_t getOsc1Wave()
	{
		return this->osc1Wave;
	}
	void setOsc2Wave(uint8_t w)
	{
		for (int i = 0; i < numvoices; i++)
		{
			mSynthVoice[i].MidiOsc2Wave(w);
		}
	}
	uint8_t getOsc2Wave()
	{
		return this->osc2Wave;
	}
	void setPwm(uint8_t pwm)
	{
		for (int i = 0; i < numvoices; i++)
		{
			mSynthVoice[i].MidiPwm(pwm);
		}
	}
	
    void handleControlChange(uint8_t channel, uint8_t control, uint8_t value)
    {
      for(int i=0;i<numvoices;i++)
      {
        mSynthVoice[i].ControlChange(channel,control, value);
      }      
    }

    private:
      SynthVoice mSynthVoice[numvoices];
      int voices_notes[numvoices];
      float* waveforms;
	  int activenotes = 0;
	  int osc1Wave = 0;
	  int osc2Wave = 0;
	  int osc1Vol = 0;
	  int osc2Vol = 0;
	  int osc1Pwm = 0;
	  int osc2Pwm = 0;
};
#endif
