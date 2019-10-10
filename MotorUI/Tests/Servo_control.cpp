// Tested on Atmel Studio 7

#include <avr/io.h>
#include <avr/interrupt.h>

float PULSE_WIDTH;

const unsigned long PRESCALER = 64;        // Timer 1 prescaler
const float PULSE_PERIOD = 0.020;          // 20 mS
const float MIN_PULSE = 0.0005;            // 0.5 mS   - Can be different for different servo's
const float MAX_PULSE = 0.0024;            // 2.4 mS   - Can be different for different servo's

volatile int val = 0;                      // Input angle in degrees to rotate
// how far apart the pulses are
const unsigned long PULSE_WIDTH_COUNT  = F_CPU / PRESCALER * PULSE_PERIOD;
const unsigned long MIN_POSITION_COUNT = F_CPU / PRESCALER * MIN_PULSE;
const unsigned long MAX_POSITION_COUNT = F_CPU / PRESCALER * MAX_PULSE;

void loop();

int main() {

  //Timer setup  
  cli();//stop interrupts
  //set timer1 for 20ms
  TCCR1A = 0;//SET TCCR1A REGISTER = 0
  TCCR1B = 0;//SAME
  TCNT1 &= 0;//INITIALIZE COUNTER VALUE TO 0;
  //SET TOP for 20ms period
  ICR1 = 4999;// = (16*10^6)/(50*64) - 1  (64 BEING THE PRESCALER) (50 for 20ms INCREMENTS)
  //USE 16 BIT TIMER1 FOR >255 8 BIT TIMER0 OR 2 FOR <255
  // Turm on Fast PWM Mode
  TCCR1A |= (1<<WGM11);
  TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); // Set Fast PWM & SET CS11 BIT FOR 64 PRESCALER
  TCCR1A |= (1<<COM1A1);  // Clear OC1A/OC1B on Compare Match,

  //pinMode(servoPin,OUTPUT);
  DDRB &= 0b00000000;
  DDRB |= 0b00000010;
  PORTB &= 0b00000000;
  while(1)
  {
	  loop();
  }
}

void loop() {
	PULSE_WIDTH = MIN_POSITION_COUNT + (val * (MAX_POSITION_COUNT - MIN_POSITION_COUNT) / 180) - 1;
	OCR1A = PULSE_WIDTH;
}
