#pragma once
#include <iostream>
#include <fstream>
using namespace std;
static bool SaveWavetable(string filename, float* wavetable, int wavetablelen)
{
	ofstream wstream;
	
	wstream.open(filename, ios::out | ios::binary );
	if (wstream.is_open())
	{
		wstream.write(reinterpret_cast<char*>(wavetable), sizeof(float)*wavetablelen);
		wstream.close();
		return true;
	}
	else
	{
		return false;
	}
}

static bool LoadWavetable(string filename, float* wavetable, int wavetablelen)
{
	ifstream rstream;
	rstream.open(filename, ios::in, ios::binary);
	if (rstream.is_open())
	{
		rstream.read(reinterpret_cast<char*>(wavetable), sizeof(float)*(wavetablelen));
		rstream.close();
		return true;
	}
	else
	{
		return false;
	}
}

