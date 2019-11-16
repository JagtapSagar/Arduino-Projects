/* Note: I tried using ADC registry to implement non-blocking analogRead(),
 *       but integrating it in the final program would cause the program to reset continuously.
 *       The program given below does not include ADC reg implementation.
*/

#include <Elegoo_GFX.h>              // Core graphics library
#include <Elegoo_TFTLCD.h>           // Hardware-specific library
#include <TouchScreen.h>
#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <SPI.h>
#include <SD.h>
#include "MPU9250.h"

void modeSelect();
void PUSH_TO_SD(String dataString);
void add_header_to_file();
void read_logfile_to_serial();
void file_check_routine();

ISR(TIMER1_COMPA_vect);
ISR(TIMER5_COMPA_vect);

#define YP A3                        // Touchscreen
#define XM A2  
#define YM 9    
#define XP 8    
#define TS_MINX 122                  //Min and Max x and y
#define TS_MAXX 918
#define TS_MINY 72
#define TS_MAXY 912
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
// Colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define BOXSIZE 40
#define MINPRESSURE 10
#define MAXPRESSURE 1000

//----------------------------------------- VARIABLES ------------------------------------------
// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
int status;

//volatile int oldTime, newTime;

int POT_SENSOR_PIN = A5;
float POT_SENSOR_VALUE = 0;
// mode selection True/False
bool MODE_1 = 0;
bool MODE_2 = 0;
bool MODE_3 = 0;
bool MODE_SELECTED_FLAG = 0;

int COUNT_MS = 0;                                 // Counts time with increments of 100ms
const unsigned long PRESCALER_MODE_1 = 64;        // Timer 1 prescaler
const unsigned long PRESCALER_MODE_2 = 256;       // Timer 5 prescaler
const float PULSE_PERIOD_MODE_1 = 40;             // 40Hz
const float PULSE_PERIOD_MODE_2 = 0.100;          // 100 mS
// how far apart the pulses are
const unsigned long PULSE_WIDTH_COUNT_MODE_1 = F_CPU / (PRESCALER_MODE_1 * PULSE_PERIOD_MODE_1);
const unsigned long PULSE_WIDTH_COUNT_MODE_2 = F_CPU / PRESCALER_MODE_2 * PULSE_PERIOD_MODE_2;

//----------------------------------------- SETUP ----------------------------------------------
void setup() {
  //--------------------------------- TIMER 1 & 5 SETUP ----------------------------------------
  cli();                                        //stop interrupts
  //Timer 1 setup                               // SET FOR 40HZ
  TCCR1A = 0;                                   //SET TCCR1A REGISTER = 0
  TCCR1B = 0;                                   //SAME
  TCNT1 &= 0;                                   //INITIALIZE COUNTER VALUE TO 0;
  OCR1A = PULSE_WIDTH_COUNT_MODE_1 - 1;         // = 6249  (64 BEING THE PRESCALER)
  //USE 16 BIT TIMER1 FOR >255 8 BIT
  TCCR1B |= (1<<WGM12)|(1<<CS11)|(1<<CS10);     // Set CTC MODE WITH FOR 64 PRESCALER
  //TCCR1A |= (1<<COM1A1);                        // Clear OC1A/OC1B on Compare Match,
  TIMSK1 |= (1<<OCIE1A);                        //ENABLE TIMER COMPARE INTERRUPT
  
  //Timer 5 setup                               // SET FOR 100ms
  TCCR5A = 0;                                   //SET TCCR5A REGISTER = 0
  TCCR5B = 0;                                   //SAME
  TCNT5 &= 0;                                   //INITIALIZE COUNTER VALUE TO 0;
  OCR5A = PULSE_WIDTH_COUNT_MODE_2 - 1;         // = 6249  (256 BEING THE PRESCALER)
  //USE 16 BIT TIMER5 FOR >255 8 BIT
  TCCR5B |= (1<<WGM32)|(1<<CS32);               // Set CTC MODE WITH FOR 256 PRESCALER
  //TCCR5A |= (1<<COM3A1);                        // Clear OC5A/OC5B/OC5C on Compare Match,
  TIMSK5 |= (1<<OCIE3A);                        //ENABLE TIMER COMPARE INTERRUPT
  sei();                                        //allow interrupts
  
  /*pinMode(10, OUTPUT);  pinMode(11, OUTPUT);  pinMode(12, OUTPUT);  pinMode(13, OUTPUT);*/
  DDRB &= 0b11110000;

  //-------------------------------------- SERIAL SETUP ----------------------------------------
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.println("Serial Initiated");

  //--------------------------------------- SPI SETUP ------------------------------------------
  SPI.begin();      // Init SPI bus

  //--------------------------------- TOUCHSCREEN UI SETUP -------------------------------------
  tft.reset();
  uint16_t  identifier=0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  //Select Mode
  tft.fillRect(0, 0, BOXSIZE*6, BOXSIZE*2, BLUE);
  tft.setCursor(20, 30);   tft.setTextColor(WHITE);    tft.setTextSize(3);   tft.println("Select Mode");
  tft.drawRect(0, 0, BOXSIZE*6, BOXSIZE*2, WHITE);
  //Number buttons
  tft.fillRect(0, BOXSIZE*3, BOXSIZE*2, BOXSIZE*2, GREEN);
  tft.setCursor(BOXSIZE-7, BOXSIZE*4-10);   tft.setTextColor(BLACK);    tft.setTextSize(3);   tft.println("1");
  tft.fillRect(BOXSIZE*2, BOXSIZE*3, BOXSIZE*2, BOXSIZE*2, RED);
  tft.setCursor(BOXSIZE*3-7, BOXSIZE*4-10);   tft.setTextColor(BLACK);    tft.setTextSize(3);   tft.println("2");
  tft.fillRect(BOXSIZE*4, BOXSIZE*3, BOXSIZE*2, BOXSIZE*2, GREEN);
  tft.setCursor(BOXSIZE*5-7, BOXSIZE*4-10);   tft.setTextColor(BLACK);    tft.setTextSize(3);   tft.println("3");
  //Return Button     - Not Used
  tft.fillRect(0, BOXSIZE*6, BOXSIZE*6, BOXSIZE*2, YELLOW);
  tft.setCursor(BOXSIZE*2-10, BOXSIZE*7-10);   tft.setTextColor(BLACK);    tft.setTextSize(3);   tft.println("Return");

  //---------------------------------- DIGITAL SENSORS SETUP -----------------------------------
  // start communication with IMU 
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }
  else {Serial.println("IMU initialization successful");}
  
  //-------------------------------------- SD CARD SETUP ---------------------------------------

  // see if the card is present and can be initialized:
  if (!SD.begin(SS)) {
    Serial.println("SD Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("SD Card initialized.");
  Serial.println("READY!!");
}


//----------------------------------------- MAIN LOOP ------------------------------------------
void loop() {
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  // read the sensor
  IMU.readSensor();
  modeSelect();
  _delay_ms(250);
}

void modeSelect() {
  TSPoint p = ts.getPoint();                          // Gets touch data from the touchscreen
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  if (p.y > BOXSIZE*3  && p.y < BOXSIZE*5) {          // Selecting MODE based on touch input
   if (p.x < BOXSIZE*2) { 
        //Serial.println("Mode 1");
        MODE_1 = 1;  MODE_2 = 0; MODE_3 = 0;
        MODE_SELECTED_FLAG = 1;
   } else if (p.x > BOXSIZE*2 && p.x < BOXSIZE*4){
        //Serial.print("Mode 2");
        MODE_1 = 0;  MODE_2 = 1; MODE_3 = 0;
        Serial.println("Channel 1 (Volts)");
   } else if (p.x > BOXSIZE*4){
       //Serial.println("Mode 3");
       MODE_1 = 0;  MODE_2 = 0; MODE_3 = 1;
       read_logfile_to_serial();
   }
  }
}

float ACCEL_Z, GYRO_Z, TEMP;
//------------------------------------- Interrupt Routines -------------------------------------

// Timer 1 Interrupt
ISR(TIMER1_COMPA_vect){
  if (MODE_1) {

  // MODE 1: Prints sensor readings to the SD card every 40Hz for 10sec

  String dataString = "";
  if (MODE_SELECTED_FLAG) {        // True only during first run when MODE 1 is selected
    file_check_routine();          // Deletes datalog.txt from the SD Card
    add_header_to_file();          // Creates new datalog.txt and adds header data to it
    MODE_SELECTED_FLAG = 0;
    }
  // Recording sensor data
  POT_SENSOR_VALUE = analogRead(POT_SENSOR_PIN);
  ACCEL_Z = IMU.getAccelZ_mss();
  GYRO_Z = IMU.getGyroZ_rads();
  TEMP = IMU.getTemperature_C();
  
  // Preparing dataString to send to datalog.txt
  dataString = "        ";
  dataString+=String(POT_SENSOR_VALUE,3);
  dataString += ",                     ";
  dataString+=String(ACCEL_Z,3);
  dataString += ",";
  dataString+=String(GYRO_Z,3);
  dataString += ",";
  dataString+=String(TEMP,3);
  
  // Sending the sensor readings to the SD Card
  PUSH_TO_SD(dataString);
  }
}

// Timer 5 Interrupt
ISR(TIMER5_COMPA_vect){

  if (COUNT_MS<=100 & MODE_1==1) {COUNT_MS++;}    // Counting to 1000msec = 10sec for MODE_1
  else {COUNT_MS = 0;MODE_1=0;}                   // Disabling MODE 1 once 10sec are completed

  if (MODE_2) {

  // MODE 2: Prints pot reading over serial every 100ms until a keypress is detected

  POT_SENSOR_VALUE = analogRead(POT_SENSOR_PIN);
  Serial.print("     ");
  Serial.println(String(POT_SENSOR_VALUE,3));
  }
  while(Serial.available()) {
   Serial.read();
   MODE_2 = 0;
   }
}

//---------------------------------------- DATA LOGGING ----------------------------------------

void PUSH_TO_SD(String dataString)  {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    //Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

void add_header_to_file() {
  // This function adds header to datalog.txt file
  String headerString = "Channel 1 (Signal Generator)  Channel 2 (Digital compass (I2C))";
  PUSH_TO_SD(headerString);
}

//---------------------------------------- SD TO SERIAL ----------------------------------------

void read_logfile_to_serial()  {
  // re-open the file for reading:
  File myFile = SD.open("datalog.txt");
  if (myFile) {
    Serial.println("datalog.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

//------------------------------------- FILE MANIPULATION --------------------------------------
// Checks if the file exists and deletes it if it does
void file_check_routine() {
  // Check to see if the file exists:
  if (SD.exists("datalog.txt")) {
    //Serial.println("datalog.txt exists.");
    SD.remove("datalog.txt");
    //Serial.println("datalog.txt has been removed.");
  //} else {
    //Serial.println("datalog.txt doesn't exist.");
  }
}
