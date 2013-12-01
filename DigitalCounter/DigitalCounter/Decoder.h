/*
 * Decoder.h
 *
 * Created: 2013-11-27 16:23:28
 *  Author: student
 */ 


#ifndef DECODER_H_
#define DECODER_H_

// Dla podanej cyfry (0-9) zwraca kod do wyœwietlania jej przez
// 7-segmentowy wyœwietlacz w kolejnoœci gfedcba sterowany 1.
// W wypadku nieprawed³owego imputu zwraca kod dla E.
unsigned int decodeSevenSegment(unsigned int number)
{
	switch(number)
	{
		case 0:
		return 0x3F;
		case 1:
		return 0x06;
		case 2:
		return 0x5B;
		case 3:
		return 0x4F;
		case 4:
		return 0x66;
		case 5:
		return 0x6D;
		case 6:
		return 0x7D;
		case 7:
		return 0x07;
		case 8:
		return 0x7F;
		case 9:
		return 0x6F;
		default:
		return 0x79;
		
	}
}



#endif /* DECODER_H_ */