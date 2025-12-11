#ifndef __PID_H
#define __PID_H

#include "main.h"
void SPEED_SET(int16_t speed);
int16_t SPEED_PidCtlA(float spd_target,float spd_current);

#endif
