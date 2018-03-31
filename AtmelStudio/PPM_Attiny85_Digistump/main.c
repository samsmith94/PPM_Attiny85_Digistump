#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t matches = 0;
volatile uint16_t desired_low_match = 0;
volatile uint8_t index = 0;
const uint16_t pwm[2] = {1000, 2000};

volatile uint16_t eddigi_length = 0;
//magas jel
ISR(TIMER0_COMPA_vect)
{
	if (matches < 31) {
		matches++;
		} else {
		matches = 0;
		stop_timer_0();

		if (index == 0) {
			desired_low_match = (pwm[index] >> 4) - (500 >> 4);
			eddigi_length += desired_low_match;
			index++;
			} else if (index == 1) {
			desired_low_match = (pwm[index] >> 4) - (500 >> 4);
			eddigi_length += desired_low_match;

			index++;
			
			
			} else if (index == 2) {
			desired_low_match = (20000 >> 4) - (eddigi_length + (500 >> 4));
			eddigi_length = 0;
			index = 0;
		}
		
		set_gpio_low();
		start_timer_1();
	}
}



//ez az alacsony jel
ISR(TIMER1_COMPA_vect)
{
	if (matches < desired_low_match) {
		matches++;
		} else {
		matches = 0;
		stop_timer_1();
		set_gpio_high();
		start_timer_0();
	}
}


void init_gpio(void)
{
	DDRB = 0b00000001;  //PB0: output
	PORTB = 0b00000001; //PB0: high
}

void set_gpio_high(void)
{
	PORTB = 0b00000001;
}

void set_gpio_low(void)
{
	PORTB = 0;
}

void start_timer_1(void)
{
	TCCR1 |= 1 << CS10;    //prescaler = 1
	OCR1A = 20; //10 us compare match if F_CPU = 20MHz
	TIMSK |= 1 << OCIE1A;    //interrupt enable
}

void stop_timer_1(void)
{
	TCCR1 = 0;
}

void start_timer_0(void)
{
	TCCR0B |= 1 << CS00;    //prescaler = 1
	OCR0A = 20; //10 us compare match if F_CPU = 20MHz
	TIMSK |= 1 << OCIE0A;    //interrupt enable
}

void stop_timer_0(void)
{
	TCCR0B = 0;
}

int main(void)
{
	sei();
	init_gpio();
	start_timer_0();
	set_gpio_high();
	
	while (1)
	{
	}
}
