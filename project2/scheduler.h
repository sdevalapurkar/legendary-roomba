#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>

#define MAXTASKS	8

#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)

typedef void (*task_cb)();

void Scheduler_init();

void Scheduler_add_periodic_task(int16_t delay, int16_t period, task_cb task, bool state);

uint16_t Scheduler_dispatch_periodic_task();

void Scheduler_add_time_critical_sporadic_task(int16_t delay, task_cb task, bool state, uint16_t duration);

void Scheduler_add_non_time_critical_sporadic_task(task_cb task, bool state);

void Scheduler_delete_sporadic_task(task_cb task);

void Scheduler_dispatch_non_time_critical_sporadic_task(uint16_t idle_time);

void Scheduler_dispatch_time_critical_sporadic_task(uint16_t idle_time);

void Scheduler_modify_periodic_task_period(task_cb task, int16_t modified_period);

void Scheduler_start();

#endif
