/*
 *
 * Created: 2013-11-27 16:18:12
 *  Author: Krzysztof Dawidowicz, Tomasz Zug
 */ 


#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "Decoder.h"
#include "Scheduler.h"


using namespace TaskScheduler;

unsigned char displayNum = 0;		// Indeks wyœwietlacza (0-3)
const int digitsNum = 4;			// Liczba cyfr
unsigned char digits[digitsNum];	// Tablica przechowuje kody do wyœwetlania cyfr aktualnej liczby

void switchDisplay()
{
	PORTA = ~digits[displayNum];       // Negacja bitowa kodu wyœwietlania - kody s¹ dla licznika sterowanego 1, nasze s¹ sterowane 0
	PORTB = ~(1 << displayNum++);	   // Zapalanie kolejnych wyœwietlaczy
	if(displayNum >3) displayNum = 0;  // By³y zapalone wszystkie wyœwietlacze, zaczynamy od pocz¹tku
}

ISR(TIMER0_COMP_vect)
{
	handleClockTick();
}

// Update licznika sekund
void counter()
{
	//if(stop) return;
	static int count = 0;
	digits[3] = decodeSevenSegment(count%10);			//Kody wyœwietlania kolejnych cyfr
	digits[2] = decodeSevenSegment((count/10)%10);
	digits[1] = decodeSevenSegment((count/100)%10);
	digits[0] = decodeSevenSegment((count/1000)%10);
	count++;
}

void sleep()
{
	_delay_ms(3);
}

int main(void)
{
	DDRA = 0xFF;
	DDRB = 0xFF;
	PORTA = 0x00;
    initTaskList();
	//initdigits();
	addPeriodicTask(sleep, 5);
	addPeriodicTask(switchDisplay, 4);
	addPeriodicTask(counter, 1000);
	schedule();
	
}