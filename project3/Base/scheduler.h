#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>

#define MAXTASKS	8

#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)

typedef void (*task_cb)();

void Scheduler_Init();

void Scheduler_StartPeriodicTask(int16_t delay, int16_t period, task_cb task, bool state);

uint16_t Scheduler_DispatchPeriodicTask();

void Scheduler_AddSporadicTask(int16_t delay, task_cb task, bool state, uint16_t duration);

void Scheduler_AddNonTimeCriticalSporadicTask(task_cb task, bool state);

void Scheduler_DeleteSporadicTask(task_cb task);

void Scheduler_DispatchNonTimeCriticalSporadicTask(uint16_t idle_time);

void Scheduler_DispatchSporadicTask(uint16_t idle_time);

void modifyPeriodicTaskPeriod(task_cb task, int16_t modified_period);

void Scheduler_Start();

#endif
