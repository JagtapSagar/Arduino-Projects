#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
//-----------------------------------Defining Touchscreen---------------------------------------
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define TS_MINX 122   //Min and Max x and y
#define TS_MAXX 918
#define TS_MINY 72
#define TS_MAXY 912
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4
// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
//#define CYAN    0x07FF
//#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define BOXSIZE 40
//#define PENRADIUS 3
#define MINPRESSURE 10
#define MAXPRESSURE 1000
//---------------------------------Variables----------------------------------------
#define dirPin 12
#define stepPin 13
int steps = 0;
int stepsper = 200;
int motorselect = 0;
int rotationselect = 0;
int numberselect = 0;
int oldnumberselect = 0;
int firstnum = 0;
int secondnum = 0;
int thirdnum = 0;
int n = 0;
int oldAngle = 0;
volatile int total = 0;
float PULSE_WIDTH;
const unsigned long PRESCALER = 64;        // Timer 1 prescaler
const float PULSE_PERIOD = 0.020;          // 20 mS
const float MIN_PULSE = 0.0005;            // 0.5 mS   - default
const float MAX_PULSE = 0.0024;            // 2.4 mS   - default
volatile int val = 90;
// how far apart the pulses are
const unsigned long PULSE_WIDTH_COUNT = F_CPU / PRESCALER * PULSE_PERIOD;
const unsigned long MIN_POSITION_COUNT = F_CPU / PRESCALER * MIN_PULSE;
const unsigned long MAX_POSITION_COUNT = F_CPU / PRESCALER * MAX_PULSE;

//------------------------------------------Setup------------------------------------------------
void setup(void) {
//------------------------------------------PWM Setup---------------------------------------------
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
  TCCR1A |= bit (COM1B1);  // Clear OC1A/OC1B on Compare Match,

  //pinMode(servoPin,OUTPUT);
  //DDRB &= 0b00000000;
  DDRB |= 0b00110100;
  PORTB &= 0b00000000;
  sei();//allow interrupts
 //-----------------------------------------------touchscreen setup------------------------------------- 
 // pinMode(servoPin, OUTPUT);
  Serial.begin(9600);
  tft.reset();
  uint16_t  identifier=0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, BOXSIZE*3, BOXSIZE*2, BLUE);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE*2, YELLOW);
  tft.setCursor(20, 30);   tft.setTextColor(WHITE);    tft.setTextSize(2);   tft.println("Stepper");
  tft.setCursor(150, 30);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("Servo");
  tft.fillRect(BOXSIZE*3, BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, YELLOW);
  tft.setCursor(150, 110);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("CCW");
  tft.fillRect(0, BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, BLUE);
  tft.setCursor(20, 110);   tft.setTextColor(WHITE);    tft.setTextSize(2);   tft.println("CW");
  tft.drawRect(0, 0, BOXSIZE*3, BOXSIZE*2, WHITE);
  tft.drawRect(0, BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, WHITE);
 //First number row 
  tft.fillRect(0, BOXSIZE*4, BOXSIZE, BOXSIZE, GREEN);
  tft.setCursor(17, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("0");
  tft.fillRect(BOXSIZE, BOXSIZE*4, BOXSIZE, BOXSIZE, RED);
  tft.setCursor(57, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("1");
  tft.fillRect(BOXSIZE*2, BOXSIZE*4, BOXSIZE, BOXSIZE, GREEN);
  tft.setCursor(97, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("2");
  tft.fillRect(BOXSIZE*3, BOXSIZE*4, BOXSIZE, BOXSIZE, RED);
  tft.setCursor(137, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("3");
  tft.fillRect(BOXSIZE*4, BOXSIZE*4, BOXSIZE, BOXSIZE, GREEN);
  tft.setCursor(177, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("4");
//Second number row
  tft.fillRect(0, BOXSIZE*5, BOXSIZE, BOXSIZE, RED);
  tft.setCursor(17, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("5");
  tft.fillRect(BOXSIZE, BOXSIZE*5, BOXSIZE, BOXSIZE, GREEN);
  tft.setCursor(57, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("6");
  tft.fillRect(BOXSIZE*2, BOXSIZE*5, BOXSIZE, BOXSIZE, RED);
  tft.setCursor(97, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("7");
  tft.fillRect(BOXSIZE*3, BOXSIZE*5, BOXSIZE, BOXSIZE, GREEN);
  tft.setCursor(137, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("8");
  tft.fillRect(BOXSIZE*4, BOXSIZE*5, BOXSIZE, BOXSIZE, RED);
  tft.setCursor(177, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("9");

  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.setCursor(20,290);
  tft.print("Stepper 000-360 and Servo 000-090");
}

void pulse(){
    val=val+total;
    PULSE_WIDTH = MIN_POSITION_COUNT + (val * (MAX_POSITION_COUNT - MIN_POSITION_COUNT) / 180) - 1;
    OCR1B = PULSE_WIDTH;
    }
void stepper(){
   PORTB |= (1<<PORTB5);
   for (int i = 0; i <= steps; i++) {
    PORTB |= (1<<PORTB5);
    delayMicroseconds(500);
    PORTB &= ~(1<<PORTB5);
    delayMicroseconds(500);
  }
}
//------------------------------------------LOOP START-------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (n==3) {
    
    if (motorselect == 1) {
      if (rotationselect == 1)  {total = -total;}
      pulse();
      _delay_ms(5000);}
      
    if (motorselect == 0) {
        if (total>360) {total = 360;}
        if (rotationselect == 1){
          PORTB |= (1<<PORTB4);
          stepper();
          _delay_ms(5000);
          PORTB &= (0<<PORTB4);
          stepper();
        }
        if (rotationselect == 0){
          PORTB &= (0<<PORTB4);
          stepper();
          _delay_ms(5000);
          PORTB |= (1<<PORTB4);
          stepper();
        }
     
    }
    
    
    n=0; firstnum=0; secondnum=0; thirdnum=0; total=0; val=90;
    tft.fillRect(0, 250, 140, 30, BLACK);
    }
pulse();

   if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
//    Serial.print("X = "); Serial.print(p.x);
//    Serial.print("\tY = "); Serial.print(p.y);
//    Serial.print("\tPressure = "); Serial.println(p.z);
    
//   }
   
//Converting x and y touchscreen reads to image pixel position
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
//---------------------------------------Motor Select-------------------------------------------------

  if (p.y < BOXSIZE*2) {
    if (p.x < BOXSIZE*3) { 
         tft.drawRect(0, 0, BOXSIZE*3, BOXSIZE*2, WHITE);
         Serial.print("Stepper");
         motorselect=0;
       //  _delay_ms(500);
    } else if (p.x > BOXSIZE*3){
         tft.drawRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE*2, WHITE); 
         Serial.print("Servo");
         motorselect=1;
     //    _delay_ms(500);
       }
    if (motorselect == 0) {
      tft.fillRect(BOXSIZE*3, 0, BOXSIZE*3, BOXSIZE*2, YELLOW);
      tft.setCursor(150, 30);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("Servo");
    } else if (motorselect == 1) {
      tft.fillRect(0, 0, BOXSIZE*3, BOXSIZE*2, BLUE);
      tft.setCursor(20, 30);   tft.setTextColor(WHITE);    tft.setTextSize(2);   tft.println("Stepper");
    }
    _delay_ms(500);
  }
  
//-----------------------------------------CW/CCW-------------------------------------------------------
  if (p.y > BOXSIZE*2  && p.y < BOXSIZE*4) {
    if (p.x < BOXSIZE*3) { 
         tft.drawRect(0, BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, WHITE);
         Serial.print("CW");
         rotationselect=0;
    //     _delay_ms(500);
    } else if (p.x > BOXSIZE*3){
         tft.drawRect(BOXSIZE*3, BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, WHITE); 
         Serial.print("CCW");
         rotationselect=1;
    //     _delay_ms(500);
       }
    if (rotationselect == 0) {
      tft.fillRect(BOXSIZE*3, BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, YELLOW);
      tft.setCursor(150, 110);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("CCW");
    } else if (rotationselect == 1) {
      tft.fillRect(0, BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, BLUE);
      tft.setCursor(20, 110);   tft.setTextColor(WHITE);    tft.setTextSize(2);   tft.println("CW");
    }
    _delay_ms(500);
  }
//------------------------------------------Numbers--------------------------------------------------
//selecting boxes - Numbers First ROW
  if (p.y > BOXSIZE*4  && p.y < BOXSIZE*5) {
    oldnumberselect = numberselect;
    n++;
    if (p.x < BOXSIZE) { 
         tft.drawRect(0, BOXSIZE*4, BOXSIZE, BOXSIZE, WHITE);
         numberselect=0;
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
       //  _delay_ms(500);
         
    } else if (p.x < BOXSIZE*2){
         tft.drawRect(BOXSIZE, BOXSIZE*4, BOXSIZE, BOXSIZE, WHITE); 
         numberselect=1;
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
       //  _delay_ms(500);
         
    } else if (p.x < BOXSIZE*3){
         tft.drawRect(BOXSIZE*2, BOXSIZE*4, BOXSIZE, BOXSIZE, WHITE);
         numberselect=2;
         Serial.print(numberselect); 
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
        // _delay_ms(500);
         
    } else if (p.x < BOXSIZE*4){
         tft.drawRect(BOXSIZE*3, BOXSIZE*4, BOXSIZE, BOXSIZE, WHITE);
         numberselect=3;
         Serial.print(numberselect); 
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
       //  _delay_ms(500);
         
    } else if (p.x < BOXSIZE*5){
         tft.drawRect(BOXSIZE*4, BOXSIZE*4, BOXSIZE, BOXSIZE, WHITE);
         numberselect=4; 
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
    //     _delay_ms(500);
         
    }
  }
//selecting boxes - Numbers Second Row  
  if (p.y > BOXSIZE*5 && p.y < BOXSIZE*6) {
    oldnumberselect = numberselect;
    n++;
    if (p.x < BOXSIZE) { 
         tft.drawRect(0, BOXSIZE*5, BOXSIZE, BOXSIZE, WHITE);
         numberselect=5;
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
      //   _delay_ms(500);
         
    } else if (p.x < BOXSIZE*2){
         tft.drawRect(BOXSIZE, BOXSIZE*5, BOXSIZE, BOXSIZE, WHITE); 
         numberselect=6;
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
      //   _delay_ms(500);
         
    } else if (p.x < BOXSIZE*3){
         tft.drawRect(BOXSIZE*2, BOXSIZE*5, BOXSIZE, BOXSIZE, WHITE);
         numberselect=7;
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
      //   _delay_ms(500);
         
    } else if (p.x < BOXSIZE*4){
         tft.drawRect(BOXSIZE*3, BOXSIZE*5, BOXSIZE, BOXSIZE, WHITE);
         numberselect=8;
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
     //    _delay_ms(500);
         
    } else if (p.x < BOXSIZE*5){
         tft.drawRect(BOXSIZE*4, BOXSIZE*5, BOXSIZE, BOXSIZE, WHITE);
         numberselect=9; 
         Serial.print(numberselect);
         tft.setTextColor(BLUE);
         tft.setTextSize(3);
         tft.setCursor(n*20,250);
         tft.print(numberselect);
      //   _delay_ms(500);
         
    }
   // _delay_ms(1000);
  }
//removing unselected boxes    
    if (oldnumberselect != numberselect){
       
      if (oldnumberselect == 0) {
       tft.fillRect(0, BOXSIZE*4, BOXSIZE, BOXSIZE, GREEN);
       tft.setCursor(17, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("0");}
      if (oldnumberselect == 1) {
       tft.fillRect(BOXSIZE, BOXSIZE*4, BOXSIZE, BOXSIZE, RED);
       tft.setCursor(57, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("1");}
      if (oldnumberselect == 2) {
       tft.fillRect(BOXSIZE*2, BOXSIZE*4, BOXSIZE, BOXSIZE, GREEN);
       tft.setCursor(97, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("2");}
      if (oldnumberselect == 3) {
       tft.fillRect(BOXSIZE*3, BOXSIZE*4, BOXSIZE, BOXSIZE, RED);
       tft.setCursor(137, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("3");}
      if (oldnumberselect == 4) {
       tft.fillRect(BOXSIZE*4, BOXSIZE*4, BOXSIZE, BOXSIZE, GREEN);
       tft.setCursor(177, 172);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("4");}
//second row      
      if (oldnumberselect == 5) {
       tft.fillRect(0, BOXSIZE*5, BOXSIZE, BOXSIZE, RED);
       tft.setCursor(17, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("5");}
      if (oldnumberselect == 6) {
       tft.fillRect(BOXSIZE, BOXSIZE*5, BOXSIZE, BOXSIZE, GREEN);
       tft.setCursor(57, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("6");}
      if (oldnumberselect == 7) {
       tft.fillRect(BOXSIZE*2, BOXSIZE*5, BOXSIZE, BOXSIZE, RED);
       tft.setCursor(97, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("7");}
      if (oldnumberselect == 8) {
       tft.fillRect(BOXSIZE*3, BOXSIZE*5, BOXSIZE, BOXSIZE, GREEN);
       tft.setCursor(137, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("8");}
      if (oldnumberselect == 9) {
       tft.fillRect(BOXSIZE*4, BOXSIZE*5, BOXSIZE, BOXSIZE, RED);
       tft.setCursor(177, 212);   tft.setTextColor(BLACK);    tft.setTextSize(2);   tft.println("9");}
    }
    _delay_ms(500);
    
    if (n==1) firstnum = numberselect*100;
    if (n==2) secondnum = numberselect*10;
    if (n==3) {thirdnum = numberselect;

    total = firstnum+secondnum+thirdnum;
    steps =  total/1.8;
    Serial.print("\ntotal = "); Serial.print(total); Serial.print("\n");
    Serial.print("\nsteps = "); Serial.print(steps); Serial.print("\n");
  
    }
  }
 }
