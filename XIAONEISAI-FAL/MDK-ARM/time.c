#include "time.h"
#include "main.h"
#include "tim.h"
#include "pid.h"
#include "motor.h"
static uint32_t systick_ms = 0;

void HAL_SYSTICK_Callback(void){
	static uint8_t motor_run_cnt = 0;
	systick_ms++;
	motor_run_cnt++;
	
	if(motor_run_cnt >= 20){
		motor_run_cnt = 0;
		app_motor_run();
	}
}

uint32_t millis(void){
	return systick_ms;
}

