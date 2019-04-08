#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "scheduler.h"
#include "LED_Test.h"
#include <stdbool.h>


typedef struct {
	int16_t period;
	int16_t remaining_time;
	uint8_t is_running;
	task_cb callback;
	bool is_ready_state;
} periodic_task_t;

periodic_task_t periodic_tasks[MAXTASKS];
uint16_t last_runtime_periodic;


typedef struct {
	int16_t delay;
	uint8_t ready_to_run;
	task_cb callback;
	bool is_ready_state;
	uint16_t duration;
} sporadic_task_t;

sporadic_task_t sporadic_tasks[MAXTASKS];
uint16_t last_runtime_sporadic;


typedef struct {
	uint8_t ready_to_run;
	task_cb callback;
	bool is_ready_state;
} non_time_critical_sporadic_task_t;

non_time_critical_sporadic_task_t non_time_critical_sporadic_tasks[MAXTASKS];


uint16_t current_time;

void init_timer() {
	TCCR0A = 0;
	TCCR0B |= (1 << CS02) | (1 << CS00);
	TCNT0 = 0;
	current_time = 0;
}

int timer_testing() {
	if (TCNT0 >= 156) {
		current_time += 1;
		TCNT0 = 0;
	}
}

int get_time() {
	return current_time;
}


void Scheduler_Init() {
	last_runtime_periodic = get_time();
	last_runtime_sporadic = get_time();
}


void Scheduler_StartPeriodicTask(int16_t delay, int16_t period, task_cb task, bool state) {
	static uint8_t id = 0;

	if (id < MAXTASKS) {
		periodic_tasks[id].remaining_time = delay;
		periodic_tasks[id].period = period;
		periodic_tasks[id].is_running = 1;
		periodic_tasks[id].callback = task;
		periodic_tasks[id].is_ready_state = state;
		id++;
	}
}


uint16_t Scheduler_DispatchPeriodicTask() {
	uint16_t i;
	uint16_t j;
	uint16_t now = get_time();
	uint16_t elapsed = now - last_runtime_periodic;
	last_runtime_periodic = now;
	task_cb t = NULL;
	uint16_t idle_time = 0xFFFFFFFF;
	bool state_val;

	for (i = 0; i < MAXTASKS; i++) {
		if (periodic_tasks[i].is_running) {
			periodic_tasks[i].remaining_time -= elapsed;

			if (periodic_tasks[i].remaining_time <= 0U) {
				if (t == NULL) {
					t = periodic_tasks[i].callback;
					state_val = periodic_tasks[i].is_ready_state;
					periodic_tasks[i].remaining_time += periodic_tasks[i].period;
				}
				idle_time = 0;
			}
			else {
				idle_time = fmin((uint32_t)periodic_tasks[i].remaining_time, idle_time);
			}
		}
	}

	if (t != NULL) {
		t(state_val);
	}

	return idle_time;
}


void Scheduler_AddSporadicTask(int16_t delay, task_cb task, bool state, uint16_t duration) {
	uint16_t i;

	for (i = 0; i < MAXTASKS; i++) {
		if (!sporadic_tasks[i].ready_to_run) {
			sporadic_tasks[i].delay = delay;
			sporadic_tasks[i].callback = task;
			sporadic_tasks[i].is_ready_state = state;
			sporadic_tasks[i].ready_to_run = 1;
			sporadic_tasks[i].duration = duration;
			break;
		}
	}
}


void Scheduler_AddNonTimeCriticalSporadicTask(task_cb task, bool state) {
	uint16_t i;

	for (i = 0; i < MAXTASKS; i++) {
		if (!non_time_critical_sporadic_tasks[i].ready_to_run) {
			non_time_critical_sporadic_tasks[i].callback = task;
			non_time_critical_sporadic_tasks[i].is_ready_state = state;
			non_time_critical_sporadic_tasks[i].ready_to_run = 1;
		}
	}
}


void Scheduler_DeleteSporadicTask(task_cb task) {
	uint16_t i;

	for (i = 0; i < MAXTASKS; i++) {
		if (sporadic_tasks[i].callback == task) {
			sporadic_tasks[i].ready_to_run = 0;
		} else if (non_time_critical_sporadic_tasks[i].callback == task) {
			non_time_critical_sporadic_tasks[i].ready_to_run = 0;
		}
	}
}


void Scheduler_DispatchNonTimeCriticalSporadicTask(uint16_t idle_time) {
	uint16_t i;
	uint16_t now = get_time();
	last_runtime_sporadic = now;
	task_cb t = NULL;
	bool state_val;

	for (i = 0; i < MAXTASKS; i++) {
		if (non_time_critical_sporadic_tasks[i].ready_to_run && idle_time != 0) {
			t = non_time_critical_sporadic_tasks[i].callback;
			state_val = non_time_critical_sporadic_tasks[i].is_ready_state;
			non_time_critical_sporadic_tasks[i].ready_to_run = 0;
		}
	}

	if (t != NULL) {
		Disable_Interrupt();
		t(state_val);
		Enable_Interrupt();
	}
}


void Scheduler_DispatchSporadicTask(uint16_t idle_time) {	
	uint16_t i;
	uint16_t now = get_time();
	uint16_t elapsed = now - last_runtime_sporadic;
	last_runtime_sporadic = now;
	task_cb t = NULL;
	bool state_val;

	for (i = 0; i < MAXTASKS; i++) {
		if (sporadic_tasks[i].ready_to_run && idle_time != 0 && sporadic_tasks[i].duration < idle_time) {
			sporadic_tasks[i].delay -= elapsed;

			if (sporadic_tasks[i].delay <= 0) {
				t = sporadic_tasks[i].callback;
				state_val = sporadic_tasks[i].is_ready_state;
				sporadic_tasks[i].ready_to_run = 0;
			}
		}
	}

	if (t != NULL) {
		Disable_Interrupt();
		t(state_val);
		Enable_Interrupt();
	}
}


void modifyPeriodicTaskPeriod(task_cb task, int16_t modified_period) {
	for (int i = 0; i < MAXTASKS; i++) {
		if (periodic_tasks[i].callback == task) {
			periodic_tasks[i].period = modified_period;
		}
	}
}


void Scheduler_Start() {	
	while(1) {
		timer_testing();
		uint16_t idle_time = Scheduler_DispatchPeriodicTask();

		if(idle_time) {
			Scheduler_DispatchSporadicTask(idle_time);
			Scheduler_DispatchNonTimeCriticalSporadicTask(idle_time);
		}
	}
}
