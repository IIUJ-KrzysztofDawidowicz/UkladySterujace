#include <avr/io.h>
#include <avr/interrupt.h>
#include "Scheduler.h"

using namespace TaskScheduler;



void toggleDiodeA0()
{
	PORTA ^= (1<<PINA0);
}
void toggleDiodeA1()
{
	PORTA ^= (1<<PINA1);
}
void toggleDiodeA2()
{
	PORTA ^= (1<<PINA2);
}
void toggleDiodeA3()
{
	PORTA ^= (1<<PINA3);
}


ISR(TIMER0_COMP_vect)
{
	handleClockTick();
	
}


int main(void)
{
	DDRA = 0xFF;
	
	initTaskList();
	addPeriodicTask(toggleDiodeA0, 20);
	addPeriodicTask(toggleDiodeA1, 20);
	addPeriodicTask(toggleDiodeA2, 5);
	addPeriodicTask(toggleDiodeA3, 10);
	schedule();
	return 0;
}