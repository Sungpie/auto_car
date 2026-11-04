/*
 * ultrasonic.h
 *
 * Created: 2026-06-17 오후 1:32:32
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define TRIG_DDR      DDRA
#define TRIG_PORT     PORTA
#define TRIG_L_PIN    0
#define TRIG_F_PIN    1
#define TRIG_R_PIN    2

#define ECHO_DDR      DDRE
#define ECHO_PORT     PINE
#define ECHO_L_PIN    4  // INT4
#define ECHO_F_PIN    5  // INT5
#define ECHO_R_PIN    6  // INT6