#include "led.h"
void init_led(void);
void led_all_on(void);
void led_all_off(void);
int led_main(void);
void led_shift_left_on(void);
void led_shift_right_on(void);
void led_shift_left_keep_on(void);
void led_shift_right_keep_on(void);
void led_flower_on(void);
void led_flower_off(void);
void auto_on (void);
void auto_off (void);

extern volatile uint32_t msec_count;
extern int func_state;

#define FUNC_SU 6


int cnt = 0;

void (*fp[]) () =
{
	led_shift_left_on, // func_state = 0
	led_shift_right_on,
	led_shift_left_keep_on,
	led_shift_right_keep_on,
	led_flower_on,
	led_flower_off  // func_state = 5
};

int led_main(void)
{
	uint8_t led_toggle = 0;
	
	led_all_off();
	
	while(1)
	{
#if 1
		fp[func_state] ();

#else
		if(msec_count >= 500) // 500ms
		{
			msec_count = 0;
			led_toggle = !led_toggle;
			if (led_toggle)
				led_all_on();
			else led_all_off();
		}
#endif
	}
	
	return 0;
}

void init_led(void)
{
	DDRA = 0xff;   // PORTA (기존 LED 출력 설정)
	PORTA = 0x00;  // all off
	

	DDRG |= (1 << 3);

	PORTG &= ~(1 << 3);
}

void auto_on (void)
{

	PORTG |= (1 << 3);
}

void auto_off (void)
{

	PORTG &= ~(1 << 3);
}

void led_all_on(void)
{
	PORTA=0xff;
}

void led_all_off(void)
{
	PORTA=0x00;
}

void led_shift_left_on(void)
{	
#if 1
	static int i = 0;
	
	if(msec_count >= 100)
	{
		msec_count = 0;
		*(unsigned char *) 0x3B = 1 << i;
		i = (i + 1) % 8;
// 		if ((i = (i + 1) % 8) == 0)
// 			func_state = (func_state + 1) % FUNC_SU; // 다음 실행할 func로 jump
	}
	
	
#endif
	
#if 0
	static int i = 0;
	
	*(unsigned char *) 0x3B = 1 << i;
	_delay_ms(30);  // 총 240ms 소요
	
	i = (i + 1) % 8;  // 다음 index 값을 계산
	
#endif
}

void led_shift_right_on(void)
{
	static int i = 0;
	
	if(msec_count >= 100)
	{
		msec_count = 0;
		*(unsigned char *) 0x3B = 0x80 >> i;
		i = (i + 1) % 8;
		
		
		
// 		if ((i = (i + 1) % 8) == 0)
// 			func_state = (func_state + 1) % FUNC_SU; // 다음 실행할 func로 jump
	}
	
	
	
}

void led_shift_left_keep_on(void)
{
	static int i = 0;
	
	
	if(msec_count >= 100)
	{
		cnt++;
		msec_count = 0;
		*(unsigned char *) 0x3B <<= 1;
		*(unsigned char *) 0x3B += 0x01;
		i = (i + 1) % 8;
		
		if(cnt == 9)
			{
				cnt = 0;
				PORTA = 0;
			}
		
// 		if ((i = (i + 1) % 8) == 0)
// 		{
// 			func_state = (func_state + 1) % FUNC_SU; // 다음 실행할 func로 jump
// 			led_all_off();
// 			*(unsigned char *) 0x3B >>= 1;
// 			*(unsigned char *) 0x3B += 0x80;
// 		}
	}
	
	

}


void led_shift_right_keep_on(void)
{
	static int i = 0;

	
	if(msec_count >= 100)
	{	
		cnt++;
		msec_count = 0;
		*(unsigned char *) 0x3B >>= 1;
		*(unsigned char *) 0x3B += 0x80;
		
		i = (i + 1) % 8;
		
		if(cnt == 9)
		{
			cnt = 0;
			PORTA = 0;
		}
		i = (i + 1) % 8;
// 		if ((i = (i + 1) % 8) == 0)
// 		{
// 			func_state = (func_state + 1) % FUNC_SU; // 다음 실행할 func로 jump
// 			led_all_off();
// 		}
	}
}

void led_flower_on(void)
{
	static int i = 0;
	
	if(msec_count >= 100)
	{
		cnt++;
		msec_count = 0;
		*(unsigned char *) 0x3B |= 0x10 << i | 0x08 >> i;
		i = (i + 1) % 8;
		
		if(cnt == 9)
		{
			cnt = 0;
			PORTA = 0;
		}
// 		if ((i = (i + 1) % 8) == 0)
// 		{
// 			func_state = (func_state + 1) % FUNC_SU; // 다음 실행할 func로 jump	
// 		}		
	}
}

void led_flower_off(void)
{
	static int i = 0;

	if(msec_count >= 100)
	{
		if(cnt == 8)
		{
			cnt = 0;
			PORTA = 0xff;
		}
		
		cnt++;
		msec_count = 0;
		*(unsigned char *) 0x3B &= ~(0x80 >> i | 0x01 << i);
		i = (i + 1) % 8;	
		
		
// 		if ((i = (i + 1) % 8) == 0)
// 		{
// 			func_state = (func_state + 1) % FUNC_SU; // 다음 실행할 func로 jump
// 			led_all_off();
// 		}
	}
}

