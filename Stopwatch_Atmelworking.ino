#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect );
void buttonpush();
void pickDigit(int x);
void pickNumber(int x);
void clearscreen();
void zero();
void one();
void two();
void three();
void four();
void five();
void six();
void seven();
void eight();
void nine();
void loop();

const int button = 0;
long n = 0; //counter
bool toggle = 0;

int main(){
 
  DDRD |= 0b11111000;
  DDRB |= 0b00111111;
  DDRC |= 0b00000001;
  PORTD = 0b00000000;
  PORTB = 0b00000000;
  PORTC = 0b00000000;

//Timer setup  
cli();//stop interrupts
//set timer1 interrupt at 100Hz
TCCR1A = 0;//SET TCCR1A REGISTER = 0
TCCR1B = 0;//SAME
TCNT1 = 0;//INITIALIZE COUNTER VALUE TO 0;
//SET TIMER COUNT FOR INCREMENTS
OCR1A = 19999;// = (16*10^6)/(100*8) - 1  (8 BEING THE PRESCALER) (100 FOR 100HZ INCREMENTS)
//USE 16 BIT TIMER1 FOR >255 8 BIT TIMER0 OR 2 FOR <255
//TURN ON CTC MODE
TCCR1B |= (1<<WGM12);  //(|=)
//SET CS11 BIT FOR 8 PRESCALER
TCCR1B |= (1<<CS11);
//ENABLE TIMER COMPARE INTERRUPT
TIMSK1 |= (1<<OCIE1A);


DDRD &= ~(1<<DDD2);  //Clear the PD2 pin (PCINT0 pin) now an input
PORTD |= (1<<PORTD2); //turn on pullup on PD2
EICRA |= (1<<ISC01);  //set  INT0 to trigger on falling
EIMSK |= (1<<INT0);   //turns on INT0
sei();//allow interrupts


  while(1)
  {
    loop();
  }
}


ISR(TIMER1_COMPA_vect){
  if (toggle){
    if(n<9999){
    n++;
    }
  }
}

ISR(INT0_vect){
  buttonpush();
}


void loop() {
 
  //This is clearing the screen, cycling the segment(1 to 4), 
  //displaying the number according to n(counter)
  //default is n=0, displaying 00.00
  clearscreen();
  pickDigit(1);
  pickNumber((n/1000)%10);
  _delay_ms(5);

  clearscreen();
  pickDigit(2);
  pickNumber((n/100)%10);
  _delay_ms(5);

  clearscreen();
  pickDigit(3);
  pickNumber((n/10)%10);
  _delay_ms(5);

  clearscreen();
  pickDigit(4);
  pickNumber(n%10);
  _delay_ms(5);
  
}

void buttonpush(){  //changes the boolean with each button push
  toggle = !toggle;
  _delay_ms(5);
}

void pickDigit(int x) //changes digit
{
  PORTB |= 0b00111100;
 
  switch(x)
  {
  case 1: 
    PORTB ^= (1<<PORTB5);      // pin 13 is d1
    break;
  case 2: 
    PORTB ^= (1<<PORTB4);      // pin 12 is d2
    PORTC |= (1<<PORTC0);
    break;
  case 3: 
    PORTB ^= (1<<PORTB3);      // pin 11 is d3
    break;
  default: 
    PORTB ^= (1<<PORTB2);      // pin 10 is d4
    break;
  }
}
 
void pickNumber(int x) //changes value of number
{
  switch(x)
  {
  default: 
    zero(); 
    break;
  case 1: 
    one(); 
    break;
  case 2: 
    two(); 
    break;
  case 3: 
    three(); 
    break;
  case 4: 
    four(); 
    break;
  case 5: 
    five(); 
    break;
  case 6: 
    six(); 
    break;
  case 7: 
    seven(); 
    break;
  case 8: 
    eight(); 
    break;
  case 9: 
    nine(); 
    break;
  }
}
 
 
void clearscreen()
{
  PORTD &= 0b00000000;
  PORTB &= (1111<<PORTB2);
  PORTC &= 0b00000000;
}
 
void zero()
{
  PORTD |= (1<<PORTD7)|(1<<PORTD6)|(1<<PORTD5)|(1<<PORTD4);
  PORTB |= (1<<PORTB0)|(1<<PORTB1);
}
 
void one()
{
  PORTD |= 0b10000000;
  PORTB |= 0b00000001;
}
 
void two()
{
  PORTD |= 0b01101000;
  PORTB |= 0b00000011;
}
 
void three()
{
  PORTD |= 0b11001000;
  PORTB |= 0b00000011;
}
 
void four()
{
  PORTD |= 0b10011000;
  PORTB |= 0b00000001;
}
 
void five()
{
  PORTD |= 0b11011000;
  PORTB |= 0b00000010;
}
 
void six()
{
  PORTD |= 0b11111000;
  PORTB |= 0b00000010;
}
 
void seven()
{
  PORTD |= 0b10000000;
  PORTB |= 0b00000011;
}
 
void eight()
{
  PORTD |= 0b11111000;
  PORTB |= 0b00000011;
}
 
void nine()
{
  PORTD |= 0b11011000;
  PORTB |= 0b00000011;
}
