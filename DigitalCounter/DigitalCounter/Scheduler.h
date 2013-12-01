#pragma once

#include <avr/interrupt.h>
#define MAX_TASKS 16
typedef void (*TASK_POINTER) (void);
namespace TaskScheduler
{
	struct Task
	{
		TASK_POINTER function;		// Czynno��, kt�r� wykonuje task. Funkcja bez argument�w zwracaj�ca void.
		uint16_t interval;			// Co ile task jest wykonywany.
		uint16_t timer;				// Czas do nast�pnego wykonania.
		uint16_t timesToExecute;	// Licznik oczekuj�cych wykona�.
		
		Task(): function(0), interval(0), timer(0), timesToExecute(0) {};
			
		Task(TASK_POINTER callback, uint16_t repeatEvery, uint16_t initialDelay)
		{
			function = callback;
			interval = repeatEvery;
			timer = initialDelay;
			timesToExecute = 0;
		}
	};
	
	Task tasks[MAX_TASKS];
	uint8_t task_count = 0;

	// Update'uje licznik czasu danego taska i obs�uguje powi�zane wydarzenia	
	void tickDownTimer(Task* task);
	
	
	// Je�li task oczekuje na wykonanie, wykonuje is zwraca true,
	// w przeciwnym wypadku zwraca false.
	bool executeIfReady(Task* task);
	
	
	// Inicjalizuje tablic� task�w.
	void initTaskList()
	{
		task_count = 0;
		for(uint8_t i=0;i<MAX_TASKS; ++i)
		{
			tasks[i] = Task();
		}
	}
	
	// Ustawia przerwanie czasowe pod przerwanie co milisekund�.
	void initClock()
	{
		TIMSK=0b00000010; //Timer/Counter0 Output Compare Match Interrupt Enable
		TCCR0=0b00000011; //Ustaw prescaler na 64
		OCR0=250;		  //Licz do 250 (64*250=16000, przerwanie co milisekund�)
		sei();			  //Enable interrupts
	}
	
	
	// Obs�uguje sygna�, �e up�yn�a jednostka czasu.
	// (Przerwanie co milsekund�)
	void handleClockTick()
	{
		for(uint8_t i = 0; i<task_count; ++i)
		{
			tickDownTimer(tasks + i);
		}
	}
	
	// Update'uje licznik czasu danego taska i obs�uguje powi�zane wydarzenia.
	void tickDownTimer(Task* task) 
	{
		if(task->timer <= 0) return; // Timer jest 0 dla ju� wykonanych, nieperiodycznych task�w.
		
		task->timer--;		
		if(task->timer > 0)	return;  // Je�li timer nie osi�gn�� 0, nic wi�cej nie trzeba robi�.
		
		task->timesToExecute++;
		if(task->interval != 0)		 // Je�li task jest periodyczny, resetuj timer.
			task->timer = task->interval;
	}
	
	// Zwraca true je�li tablica task�w jest pe�na i nie mo�na doda� wi�cej.
	bool taskListFull()
	{
		return task_count + 1 >= MAX_TASKS;
	}

	bool addPeriodicTask(TASK_POINTER callback, uint32_t interval)
	{
		
		if(taskListFull()) return false;
		
		cli(); //Disable interrupts
		tasks[task_count++] = Task(callback, interval, interval);
		sei(); //Enable interrupts
		return true;
		
	}


	bool addNonPeriodicTask(TASK_POINTER callback, uint32_t delay)
	{
		
		if(taskListFull()) return false;
		
		cli();
		tasks[task_count++] = Task(callback, 0, delay);
		sei();
		return true;
	}

	// Je�li lista task�w jest pusta, nic nie robi.
	// W przeciwnym wypadku wykonuje taski w p�tli niesko�czonej.
	void schedule()
	{
		initClock();
		if(task_count == 0) return;
		
		uint8_t i;
		while(1)
		{
			for(i = 0; i < task_count; ++i) 
			{
				// executeIfReady zwraca true je�li task si� wykona�
				if(executeIfReady(tasks+i)) break;
				
			}
		}
	}
	
	// Je�li task oczekuje na wykonanie, 
	// wywo�uje zwi�zan� z taskiem funkcj� i zwraca true,
	// w przeciwnym wypadku zwraca false.
	bool executeIfReady(Task* task)
	{
		if(task->timesToExecute <= 0) return false;
		
		
		task->function();
		cli();
		task->timesToExecute--;
		sei();
		return true;
		
	}

}


