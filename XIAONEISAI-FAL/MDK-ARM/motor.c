#include "motor.h"
#include "main.h"
#include "pid.h"
#include "encounter.h"
#include "stdio.h"



void motor_speed_set(float A,float B,float C,float D)
{
	SPEED_ASET(A);
  SPEED_BSET(B);
  SPEED_CSET(C);
  SPEED_DSET(D);
}




void app_motor_run(void){
		// 通过编码器获取车轮实时转速m/s
	W_A.RT=(float)(ENCODE_1_GETCOUNT() * MEC_WHEEL_SCALE);
	W_B.RT=(float)(ENCODE_2_GETCOUNT() * MEC_WHEEL_SCALE);
	W_C.RT=(float)(ENCODE_3_GETCOUNT() * MEC_WHEEL_SCALE);
	W_D.RT=(float)(ENCODE_4_GETCOUNT() * MEC_WHEEL_SCALE);

    // 利用PID算法计算电机PWM值
    W_A.PWM = SPEED_PidCtlA(-W_A.TG, W_A.RT); // L1
    W_B.PWM = SPEED_PidCtlB(W_B.TG, W_B.RT); // R1
    W_C.PWM = SPEED_PidCtlC(-W_C.TG, W_C.RT); // L2
    W_D.PWM = SPEED_PidCtlD(W_D.TG, W_D.RT); // R2

    // 设置电机PWM值
    SPEED_ASET(-W_A.PWM);
    SPEED_BSET(-W_B.PWM);
    SPEED_CSET(-W_C.PWM);
    SPEED_DSET(-W_D.PWM);
}
