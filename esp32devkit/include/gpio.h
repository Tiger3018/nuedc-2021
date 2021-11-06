#ifndef __GPIO_H__
#define __GPIO_H__

#include <pins_arduino.h>

/* Init in setup() */
#define LED_PIN LED_BUILTIN // 2 - BLUE LED

/* Init in motorSetup(), */
#define MOTOR_LIN1_PIN 21 /* INPUT &B1A */
#define MOTOR_LIN2_PIN 22 /* INPUT &B1B */
#define MOTOR_L1_PIN 14 /* &IN4 */
#define MOTOR_L2_PIN 27 /* &IN3 */
#define MOTOR_LPWM_PIN 33 /* ADC 1_5 &ENB */
#define MOTOR_RIN1_PIN 35 /* INPUT &B2A */
#define MOTOR_RIN2_PIN 34 /* INPUT &B2B */
#define MOTOR_R1_PIN 26 /* &IN2 */
#define MOTOR_R2_PIN 25 /* &IN1 */
#define MOTOR_RPWM_PIN 32 /* ADC 1_4 &ENA */

/* Init in motorSetup(), */
#define SERVO_PIN 23

#define SERIAL_MV_RX_PIN 25
#define SERIAL_MV_TX_PIN 27

#endif /* __GPIO_H__ */