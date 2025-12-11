#include "time.h"
#include "main.h"
#include "tim.h"

static uint32_t systick_ms = 0;

void HAL_SYSTICK_Callback(void){
	static uint8_t motor_run_cnt = 0;
	systick_ms++;
	motor_run_cnt++;
	
	if(motor_run_cnt >= 20){
		motor_run_cnt = 0;
		//待定(填写电机控制函数)
	}
}

uint32_t millis(void){
	return systick_ms;
}

void MOTOR_CMD(void){
	//电机控制函数，需要结合 move/pid/encounter 三个函数一起写

}
