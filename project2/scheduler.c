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
	uint8_t is_ready;
	task_cb callback;
	bool is_ready_state;
	uint16_t duration;
} time_critical_sporadic_task_t;

time_critical_sporadic_task_t time_critical_sporadic_tasks[MAXTASKS];
uint16_t last_runtime_sporadic;


typedef struct {
	uint8_t is_ready;
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


void Scheduler_init() {
	last_runtime_periodic = get_time();
	last_runtime_sporadic = get_time();
}


void Scheduler_start_periodic_task(int16_t delay, int16_t period, task_cb task, bool state) {
	static uint8_t i = 0;

	if (i < MAXTASKS) {
		periodic_tasks[i].remaining_time = delay;
		periodic_tasks[i].period = period;
		periodic_tasks[i].is_running = 1;
		periodic_tasks[i].callback = task;
		periodic_tasks[i].is_ready_state = state;
		i++;
	}
}


uint16_t Scheduler_dispatch_periodic_task() {
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


void Scheduler_add_time_critical_sporadic_task(int16_t delay, task_cb task, bool state, uint16_t duration) {
	uint16_t i;

	for (i = 0; i < MAXTASKS; i++) {
		if (!time_critical_sporadic_tasks[i].is_ready) {
			time_critical_sporadic_tasks[i].delay = delay;
			time_critical_sporadic_tasks[i].callback = task;
			time_critical_sporadic_tasks[i].is_ready_state = state;
			time_critical_sporadic_tasks[i].is_ready = 1;
			time_critical_sporadic_tasks[i].duration = duration;
			break;
		}
	}
}


void Scheduler_add_non_time_critical_sporadic_task(task_cb task, bool state) {
	uint16_t i;

	for (i = 0; i < MAXTASKS; i++) {
		if (!non_time_critical_sporadic_tasks[i].is_ready) {
			non_time_critical_sporadic_tasks[i].callback = task;
			non_time_critical_sporadic_tasks[i].is_ready_state = state;
			non_time_critical_sporadic_tasks[i].is_ready = 1;
            break;
		}
	}
}


void Scheduler_delete_sporadic_task(task_cb task) {
	uint16_t i;

	for (i = 0; i < MAXTASKS; i++) {
		if (time_critical_sporadic_tasks[i].callback == task) {
			time_critical_sporadic_tasks[i].is_ready = 0;
		} else if (non_time_critical_sporadic_tasks[i].callback == task) {
			non_time_critical_sporadic_tasks[i].is_ready = 0;
		}
	}
}


void Scheduler_dispatch_non_time_critical_sporadic_task(uint16_t idle_time) {
	uint16_t i;
	uint16_t now = get_time();
	last_runtime_sporadic = now;
	task_cb t = NULL;
	bool state_val;

	for (i = 0; i < MAXTASKS; i++) {
		if (non_time_critical_sporadic_tasks[i].is_ready && idle_time != 0) {
			t = non_time_critical_sporadic_tasks[i].callback;
			state_val = non_time_critical_sporadic_tasks[i].is_ready_state;
			non_time_critical_sporadic_tasks[i].is_ready = 0;
		}
	}

	if (t != NULL) {
		Disable_Interrupt();
		t(state_val);
		Enable_Interrupt();
	}
}


void Scheduler_dispatch_time_critical_sporadic_task(uint16_t idle_time) {	
	uint16_t i;
	uint16_t now = get_time();
	uint16_t elapsed = now - last_runtime_sporadic;
	last_runtime_sporadic = now;
	task_cb t = NULL;
	bool state_val;

	for (i = 0; i < MAXTASKS; i++) {
		if (time_critical_sporadic_tasks[i].is_ready && idle_time != 0 && time_critical_sporadic_tasks[i].duration < idle_time) {
			time_critical_sporadic_tasks[i].delay -= elapsed;

			if (time_critical_sporadic_tasks[i].delay <= 0) {
				t = time_critical_sporadic_tasks[i].callback;
				state_val = time_critical_sporadic_tasks[i].is_ready_state;
				time_critical_sporadic_tasks[i].is_ready = 0;
			}
		}
	}

	if (t != NULL) {
		Disable_Interrupt();
		t(state_val);
		Enable_Interrupt();
	}
}


void Scheduler_modify_periodic_task_period(task_cb task, int16_t modified_period) {
	for (int i = 0; i < MAXTASKS; i++) {
		if (periodic_tasks[i].callback == task) {
			periodic_tasks[i].period = modified_period;
		}
	}
}


void Scheduler_start() {	
	while(1) {
		timer_testing();
		uint16_t idle_time = Scheduler_dispatch_periodic_task();

		if(idle_time) {
			Scheduler_dispatch_time_critical_sporadic_task(idle_time);
			Scheduler_dispatch_non_time_critical_sporadic_task(idle_time);
		}
	}
}
