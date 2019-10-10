#include <avr/io.h>
#include <avr/delay.h>

// defines pins numbers
// const int stepPin = 13; 
// const int dirPin = 12; 

void loop();
 
int main() {
  // Sets the two pins as Outputs
  DDRB |= 0b00110000;
  PORTB &= 0b00000000;
  while(1)
  {
	  loop();
  }
}
void loop() {
  // Makes 200 pulses for making one full cycle rotation
  PORTB |= (1<<PORTB4);           // Enables the motor to move in a particular direction
  for (int i = 0; i <= 200; i++) {
	  PORTB |= (1<<PORTB5);
	  _delay_us(500);
	  PORTB &= ~(1<<PORTB5);
	  _delay_us(500);
	  }
  _delay_ms(1000); // One second delay
  
  PORTB &= ~(1<<PORTB4);          // Enables the motor to move in a particular direction
  for (int i = 0; i <= 200; i++) {
	  PORTB |= (1<<PORTB5);
	  _delay_us(500);
	  PORTB &= ~(1<<PORTB5);
	  _delay_us(500);
	  }
  _delay_ms(1000); // One second delay
  
}
