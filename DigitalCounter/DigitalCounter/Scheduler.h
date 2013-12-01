#pragma once

#include <avr/interrupt.h>
#define MAX_TASKS 16
typedef void (*TASK_POINTER) (void);
namespace TaskScheduler
{
	struct Task
	{
		TASK_POINTER function;		// Czynnoœæ, któr¹ wykonuje task. Funkcja bez argumentów zwracaj¹ca void.
		uint16_t interval;			// Co ile task jest wykonywany.
		uint16_t timer;				// Czas do nastêpnego wykonania.
		uint16_t timesToExecute;	// Licznik oczekuj¹cych wykonañ.
		
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

	// Update'uje licznik czasu danego taska i obs³uguje powi¹zane wydarzenia	
	void tickDownTimer(Task* task);
	
	
	// Jeœli task oczekuje na wykonanie, wykonuje is zwraca true,
	// w przeciwnym wypadku zwraca false.
	bool executeIfReady(Task* task);
	
	
	// Inicjalizuje tablicê tasków.
	void initTaskList()
	{
		task_count = 0;
		for(uint8_t i=0;i<MAX_TASKS; ++i)
		{
			tasks[i] = Task();
		}
	}
	
	// Ustawia przerwanie czasowe pod przerwanie co milisekundê.
	void initClock()
	{
		TIMSK=0b00000010; //Timer/Counter0 Output Compare Match Interrupt Enable
		TCCR0=0b00000011; //Ustaw prescaler na 64
		OCR0=250;		  //Licz do 250 (64*250=16000, przerwanie co milisekundê)
		sei();			  //Enable interrupts
	}
	
	
	// Obs³uguje sygna³, ¿e up³ynê³a jednostka czasu.
	// (Przerwanie co milsekundê)
	void handleClockTick()
	{
		for(uint8_t i = 0; i<task_count; ++i)
		{
			tickDownTimer(tasks + i);
		}
	}
	
	// Update'uje licznik czasu danego taska i obs³uguje powi¹zane wydarzenia.
	void tickDownTimer(Task* task) 
	{
		if(task->timer <= 0) return; // Timer jest 0 dla ju¿ wykonanych, nieperiodycznych tasków.
		
		task->timer--;		
		if(task->timer > 0)	return;  // Jeœli timer nie osi¹gn¹³ 0, nic wiêcej nie trzeba robiæ.
		
		task->timesToExecute++;
		if(task->interval != 0)		 // Jeœli task jest periodyczny, resetuj timer.
			task->timer = task->interval;
	}
	
	// Zwraca true jeœli tablica tasków jest pe³na i nie mo¿na dodaæ wiêcej.
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

	// Jeœli lista tasków jest pusta, nic nie robi.
	// W przeciwnym wypadku wykonuje taski w pêtli nieskoñczonej.
	void schedule()
	{
		initClock();
		if(task_count == 0) return;
		
		uint8_t i;
		while(1)
		{
			for(i = 0; i < task_count; ++i) 
			{
				// executeIfReady zwraca true jeœli task siê wykona³
				if(executeIfReady(tasks+i)) break;
				
			}
		}
	}
	
	// Jeœli task oczekuje na wykonanie, 
	// wywo³uje zwi¹zan¹ z taskiem funkcjê i zwraca true,
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


