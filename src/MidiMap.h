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

};

