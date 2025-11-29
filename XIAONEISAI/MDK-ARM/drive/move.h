#ifndef MOVE_H
#define MOVE_H

#include "stm32f1xx_hal.h"

void LFMotor(int8_t speed);
void RFMotor(int8_t speed);
void LBMotor(int8_t speed);
void RBMotor(int8_t speed);
void FM(void);
void BM(void);
void LM(void);
void RM(void);
void LR(void);
void RR(void);
void Brake(void);

#endif
