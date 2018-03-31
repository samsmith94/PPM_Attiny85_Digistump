#include <avr/io.h>
#include <avr/interrupt.h>

#define HIGH_PULSE_LENGTH (500>>4)
#define PERIOD  (20100>>4)
#define NUM_OF_CHANNELS 8

const uint16_t pwm[NUM_OF_CHANNELS] = {1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700};

volatile uint16_t matches = 0;
volatile uint16_t desired_low_match = 0;
volatile uint8_t index = 0;

volatile uint16_t len_of_actual_frame = 0;

//high pulse
ISR(TIMER0_COMPA_vect)
{
  if (matches < HIGH_PULSE_LENGTH) {
    matches++;
  } else {
    matches = 0;
    stop_timer_0();

    switch(index) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        desired_low_match = (pwm[index] >> 4) - (HIGH_PULSE_LENGTH);
        len_of_actual_frame += desired_low_match + HIGH_PULSE_LENGTH;
        index++;
        break;
      case 8:
        desired_low_match = PERIOD - (len_of_actual_frame + HIGH_PULSE_LENGTH);
        len_of_actual_frame = 0;
        index = 0;
        break;
    }   
    set_gpio_low();
    start_timer_1();
  }
}

//low
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
