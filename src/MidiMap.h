#pragma once
class MidiMap {

public :
	int Map(int noteIn, int base)
	{
		if (noteIn < 36 || noteIn>99)
		{
			return noteIn;
		}
		int noteOut = noteIn-36;
		if (noteOut >= 0 && noteOut <= 7)
		{
			noteOut = noteOut;
		}
		else if (noteOut >= 8 && noteOut <= 15)
		{
			noteOut = noteOut - 4;
		}
		else if (noteOut >= 16 && noteOut <= 23)
		{
			noteOut = noteOut - 8;
		}
		else if (noteOut >= 24 && noteOut <= 31)
		{
			noteOut = noteOut - 12;
		}
		else if (noteOut >= 32 && noteOut <= 39)
		{
			noteOut = noteOut - 16;
		}
		else if (noteOut >= 40 && noteOut <= 47)
		{
			noteOut = noteOut - 20;
		}
		else if (noteOut >= 48 && noteOut <= 55)
		{
			noteOut = noteOut - 24;
		}
		else if (noteOut >= 56 && noteOut <= 63)
		{
			noteOut = noteOut - 28;
		}
		return base + noteOut;
	}
	int Map2(int noteIn, int base)
	{
		if (noteIn < 36 || noteIn>99)
		{
			return noteIn;
		}
		int noteOut = noteIn - 36;
		int oct = noteOut / 8;
		noteOut = noteOut - 4 * oct;
		
		return base + noteOut;
	}
	int Map3(int noteIn, int base, int factor)
	{
		if (noteIn < 36 || noteIn>99)
		{
			return noteIn;
		}
		int noteOut = noteIn - 36;
		int oct = noteOut / 8;
		noteOut = noteOut - factor * oct;

		return base + noteOut;
	}
	int MapScale(int noteIn, int base, int scale[])
	{
		
		if (noteIn < 36 || noteIn>99)
		{
			return noteIn;
		}
		int noteOut = noteIn - 36;
		
		noteOut = scale[noteOut]+base;
		return noteOut;
	}
	int majorScale[64] =
	{
		0, 2, 4, 5, 7, 9, 11, 12,
		5, 7, 9, 11,12,14,16, 17,
		11,12,14,16,17,19,21, 23,
		16,17,19,21,23,24,26, 28,
		21,23,24,26,28,29,31, 33,
		26,28,29,31,33,35,36, 38,
		31,33,35,36,38,40,41, 43,
		36,38,40,41,43,45,47, 48

	};
	int scale[64] =
	{
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	};
	
	
	
	int MapMinor(int noteOn, int base)
	{
		
		return MapScale(noteOn, base, scale);
	}
	int MapMajor(int noteIn, int base)
	{
		return MapScale(noteIn, base,  this->majorScale);
	}
	void CreateMidiMap(int* scale, int scalelen, int* scalemap)
	{
		int row = 0;
		int count = 0;
		int currnote = 0;
		int oct = 0;
		int base = 0;
		int baseoct = 0;
		while (count < 64)
		{

			scalemap[count] = scale[currnote % scalelen]+12*oct;
			count++;
			currnote++;
			if (currnote % scalelen == 0)
			{
				oct = oct + 1;
			}
			if (count % 8  == 3)
			{
				base = currnote;
				baseoct = oct;
			}
			if (count % 8 == 0)
			{
				currnote = base;
				oct = baseoct;
			}
			


		}
		for (int row = 7; row >= 0; row--)
		{
			for (int col = 0; col < 8; col++)
			{
				printf("%3d ",scalemap[8*row+col]);
			}
			printf("\r\n");
		}
	}
};

