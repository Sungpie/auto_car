/*
 * 07.DCMOTOR.c
 *
 * Created: 2026-06-18
 * Author : Cho
 */ 

#define F_CPU 16000000UL
#include <avr/io.h> // PORT에 대한 정의, PORTA 등
#include <util/delay.h> // _delay_ms 등
#include <avr/interrupt.h> // sei 등 함수 사용하기 위함
#include <stdio.h>

#include "def.h"
#include "button.h"

extern volatile int dist_left;
extern volatile int dist_front;
extern volatile int dist_right;

extern int led_main(void);
extern void init_led(void);
extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern pc_commnad_processing(void);
extern void init_ultrasonic(void);
extern void ultrasonic_processing(void);
extern int get_button(int button_num, int button_pin);

extern void init_timer1_pwm(void);
extern void init_motor_driver(void);
extern void init_button(void);
extern void init_uart1(void);
extern volatile uint8_t bt_data;

extern void forward (int speed);
extern void backward (int speed);
extern void turn_left (int speed);
extern void turn_right (int speed);
extern void stop (void);

extern void auto_on (void);
extern void auto_off (void);

void manual_mode(void);
void auto_mode(void);
void auto_mode_check(void);
void distance_check(void);
void init_timer0(void);


FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE); // comportMaster로 출력하기 위함

volatile uint32_t msec_count = 0; // volatile 최적화 방지
volatile int ultrasonic_check_time = 0;

int func_state = MANUAL_MODE;
int auto_state = 0;

// 함수 포인터 배열보다 함수가 위에 먼저 선언되어야 함.
void(*pfunc[])()=
{
	manual_mode,
	auto_mode_check,
	auto_mode,
	distance_check
};

void manual_mode(void)
{
	switch(bt_data)
	{
		case 'F':
		case 'f':
			forward(500);	// 4us x 500 = 0.002sec (2ms)
			break;
		case 'B':
		case 'b':
			backward(500);
			break;
		case 'L':
		case 'l':
			turn_left(700);	// 4us x 700 = 2800 0.002sec
			break;
		case 'R':
		case 'r':
			turn_right(700);
			break;
		case 'S':
		case 's':
			stop();
			break;
		default:
			break;
	}
}

void auto_mode(void)
{
	ultrasonic_processing();
	
	if(dist_front > 20)
	{
		forward(500);
	}
	else if(dist_front <= 20) // if를 else if로 묶어주는 것이 안전합니다.
	{
		turn_left(700);
	}
}

void auto_mode_check(void)
{
	if(auto_state == 0)
	{
		func_state = AUTO_MODE;
		auto_state = 1;
		auto_on();  
	}
	else
	{
		func_state = MANUAL_MODE;
		auto_state = 0;
		auto_off(); 
	}
}

void distance_check(void) {}

int main(void)
{
	init_led();
	init_timer0();
	init_uart0();
	init_uart1();
	init_button();
	init_motor_driver();
	init_timer1_pwm();
	init_ultrasonic();
	
	stdout = &OUTPUT;
	sei();

	int btn_press_flag = 0; // 💡 버튼 중복 입력 방지용 플래그

	while (1)
	{
		// get_button이 눌린 상태(1)이고, 이전에 누른 기억이 없다면(flag==0)
		if (get_button(BUTTON0, BUTTON0PIN))
		{
			if (btn_press_flag == 0)
			{
				btn_press_flag = 1; // 이제 누른 상태로 기억
				func_state = AUTO_MODE_CHECK; // 모드 체크 실행 요청
			}
		}
		else
		{
			btn_press_flag = 0; // 손을 떼면 플래그 리셋
		}
		
		pfunc[func_state](); // 현재 모드 실행
	}
	return 0;
}

/*
1. timer 0을 초기화 한다.
	AVR에서 8bit timer 0번과 2번 두 개가 있다.
	이 중에서 0번을 초기화 한다.
	임베디드에서 가장 신경을 써야 할 부분이 초기화 하는 것이다.
	초기화가 잘못되면 이후가 다 꼬임.
2. 8bit를 통해 1ms를 측정하는 timer/counter를 만들고자 한다.
 2.1 분주비를 설정해야 함. (64분주)
	 16000000Hz (16MHz) / 64 ==> 250,000Hz
 2.2 1주기가 잡아먹는 시간
	 T = 1/f = 1/250,000 ==> 4us (0.004ms)
 2.3 8bit 가지고 count하는 시간을 계산 (8bit timer OV(OverFlow))
	 0.004ms x 256개 ==> 0.001024s (1.024ms)
	 0.004ms x 250개 ==> 0.001s (1ms)
*/

void init_timer0(void)
{	
		TCNT0 = 6; // TCNT0  6~256 :250개 펄스 count 하기 위해
		TCCR0 &=  ~(1 << CS02 | 1 << CS01 | 1 << CS00);
		TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00;  // 64분주
		TIMSK |= 1 << TOIE0;  // TIMER0 Overflow Interrupt
		// sei(); // 전역(대문) 인터럽트 허용
}

