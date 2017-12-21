//====BEGINNING OF SETTINGS====//
// This is the settings for the VArduino.
// Edit it to add or remove components from your variometer
// To add one, just uncomment (remove the //), and to remove one, just comment it (add //)

// Enable screen functionnality. The screen should be a SSD1306, with size > 128x32 (optimal)
// #define SCREEN
#define BUTTON_LEFT 4 // Pin for left button
#define BUTTON_RIGHT 5 // and for right one.

// Enable bluetooth, through serial. Baudrate is editable a down here too.
#define BLUETOOTH // Comment this to disable the module
#define BT_RX 3 // RX pin
#define BT_TX 2 // TX pin
#define BL_BAUDRATE 115200 // Baudrate

// If you are using a battery-powered device, and you'd like to know the voltage and life, edit down here.
// #define BATTERY // Comment this to disable
const int BAT_ANALOG_PIN = A0; // Pin must be analog.
#define MAXIMUM_VOLTAGE 8400 // The rated voltage of your battery, when it is full
#define DEAD_VOLTAGE 6000 // The voltage at which the battery is considered 0% and will not give enough power
#define VOLTAGE_DIVIDER 2 // The ratio of the voltage divider. If the battery is under 5V, connect it directly to pin and use 1 here; if it is over 5V, use two resistor to divide voltage, and do R1 / R2 to get the ratio.

// Pressure at sea level; normally it should be really hard to change it on the fly, best left untouched
unsigned long p0 = 101325;

// Enables sound
#define SOUND
#define VOLUME 10 // Volume, from 0 to 10
#define fallAlarmThreshold -1.0 // Vario at which the low pitch sound will play
#define upBeepThreshold 0.5 // Vario at which sound will start

// Other settings
#define PRESSURE_SAMPLES 10 //define moving average filter array size (from 2 to 15), more means vario is less sensitive and slower
#define VARIO_SAMPLES 5 // Samples for rounding vario. Can be adjusted to your feeling. More means vario is less sensitive to noise but less reactive
//The more thoses two values above, the slower the vario, but the more precise data it will give (Experiment, this is made for it)
#define SERIAL_BAUDRATE 9600 // Serial (USB or whatever is connected to RX0 / TX1) baudrate

//====END OF SETTINGS====//

//====PROJECT INFORMATIONS====//
/*
  Base code : Arduino Vario by Benjamin PERRIN 2017 / Vari'Up
      Based on Arduino Vario by Jaros, 2012 and vario DFelix 2013
      Credits:
      (1) http://code.google.com/p/bmp085driver/                             //bmp085 library
      (2) http://mbed.org/users/tkreyche/notebook/bmp085-pressure-sensor/    //more about bmp085 and average filter
      (3) http://code.google.com/p/rogue-code/                               //helpfull tone library to make nice beeping without using delay

   Rewrite & sound & screen & optimisation : Daniel THIRION (defvs) 2017

  $LK8EX1,pressure,altitude,vario,temperature,battery,*checksum
*/
/* PROJECT PINOUT

   BMP085 / BMP180 : SDA --> D4 | SCL --> D5

   HC-05 / HC-06 : TX --> defined above | RX --> defined above

   Buzzer : D10 --> RESISTOR --> Buzzer + | Buzzer - --> D9

   If you wish to add battery level checking, connect the defined pin to your battery +. If the voltage is more than 5V, follow this and connect to A0. Change above settings accordingly
   https://cdn.instructables.com/FRW/H8NG/I9EORGP6/FRWH8NGI9EORGP6.MEDIUM.jpg

   SSD1306 Screen : SDA --> D4 | SCL --> D5
*/
//====END OF INFOS====//

//====REAL STUFF BEGINS HERE====//
//===========THE CODE===========//

//====LIBRARIES====//
#include <Battery.h>
#include <RunningAverage.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "BMP085.h"
#include <stdlib.h>
#include <SoftwareSerial.h>
#include <toneAC.h>
#include <OneButton.h>

//====OBJECTS====//
#ifdef BLUETOOTH
SoftwareSerial BTSerial(BT_TX, BT_RX);
#endif

#ifdef SCREEN
Adafruit_SSD1306 display(0);

const unsigned char myBitmap [] PROGMEM = { // if you want, you can create a 32x32 bitmap of your choice to add it to the startscreen
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0f, 0x00, 0x01, 0xfc, 0x3f, 0x80,
  0x03, 0x8e, 0x71, 0xc0, 0x07, 0x07, 0xe0, 0x60, 0x06, 0x03, 0xc4, 0x60, 0x06, 0x71, 0x8e, 0x60,
  0x06, 0x03, 0xc4, 0x60, 0x07, 0x03, 0xe0, 0x60, 0x03, 0x8e, 0x70, 0xc0, 0x01, 0xfc, 0x3f, 0xc0,
  0x00, 0xf8, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
OneButton left(BUTTON_LEFT, true);
OneButton right(BUTTON_RIGHT, true);
byte page = 0;
boolean record = false;
#endif

BMP085 bmp085 = BMP085();
long     Pressure = 101325;
float    Altitude;
				
unsigned long get_time2 = millis();
unsigned long get_time3 = millis();

#define samples 40
#define maxsamples 50

#ifdef SCREEN
unsigned long startTime = 0;
float maxAltitude = 0;
float maxUpVario = 0;
#endif

RunningAverage varioAv(VARIO_SAMPLES);
RunningAverage pressureAv(PRESSURE_SAMPLES);

#ifdef BATTERY
Battery battery(DEAD_VOLTAGE, MAXIMUM_VOLTAGE, BAT_ANALOG_PIN, VOLTAGE_DIVIDER);

float lastVoltage = 0;
int lastLevel = 0;
#endif

//====SETUP====//
void setup()
{
  Serial.begin(SERIAL_BAUDRATE);       // set up arduino serial port
	 Serial.println(F("VArduino is waking up"));
				#ifdef BLUETOOTH
  BTSerial.begin(BL_BAUDRATE);  	  // start communication with the HC-05 using 38400
				#endif
  Wire.begin();             // lets init i2c protocol
  bmp085.init(MODE_ULTRA_HIGHRES, p0, false);
  // BMP085 ultra-high-res mode, 101325Pa = 1013.25hPa, false = using Pa units
  #ifdef SCREEN
		display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false);  // initialize with the I2C addr 0x3C (for the 128x32)
	 #endif
  #ifdef BATTERY 
				battery.begin();
		#endif
  #ifdef SCREEN
	 display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK); // 'inverted' text
  display.println(F("VArduino"));
  display.setTextSize(1);
  display.println(F("Daniel THIRION")); // pilot name here
  display.println(F("Version 2.7"));
  display.drawBitmap(96, 0, myBitmap, 32, 32, WHITE);
  display.display();
  delay(1000);
  display.invertDisplay(true);
  delay(300);
  display.invertDisplay(false);
  delay(300);
  display.invertDisplay(true);
  delay(300);
  display.invertDisplay(false);
  delay(300);
  display.clearDisplay();
  delay(1000);
				
  left.attachClick(leftClick);
  right.attachClick(rightClick);
		left.attachLongPressStart(leftLong);
		right.attachLongPressStart(rightLong);
	 #endif
  varioAv.clear();
	 pressureAv.clear();
}

//====LOOP====//
void loop(void)
{
  float alt[51];
  float tim[51];
  float tempo = millis();
  float vario = 0;
  float N1 = 0;
  float N2 = 0;
  float N3 = 0;
  float D1 = 0;
  float D2 = 0;
  bmp085.calcTruePressure(&Pressure);                                   //get one sample from BMP085 in every loop
		pressureAv.addValue(Pressure);
  long average_pressure = pressureAv.getAverage();                   //put it in filter and take average
  Altitude = (float)44330 * (1 - pow(((float)Pressure / p0), 0.190295)); //take new altitude in meters

  for (int cc = 1; cc <= maxsamples; cc++)                             //samples averaging and vario algorithm
  {
    alt[(cc - 1)] = alt[cc];
    tim[(cc - 1)] = tim[cc];
  };
  alt[maxsamples] = Altitude;
  tim[maxsamples] = tempo;
  float stime = tim[maxsamples - samples];
  for (int cc = (maxsamples - samples); cc < maxsamples; cc++)
  {
    N1 += (tim[cc] - stime) * alt[cc];
    N2 += (tim[cc] - stime);
    N3 += (alt[cc]);
    D1 += (tim[cc] - stime) * (tim[cc] - stime);
    D2 += (tim[cc] - stime);
  };

  vario = 1000 * ((samples * N1) - N2 * N3) / (samples * D1 - D2 * D2);
  varioAv.addValue(vario);
  vario = varioAv.getAverage();


    char str_out[35];
    int variocm = vario * 100;
    sprintf(str_out, "$LK8EX1,%ld,0,%i,99,%d,*", average_pressure, variocm, 
				#ifdef BATTERY
				(lastLevel + 1000)
				#else
				0
				#endif
				);
    byte CRC = 0;
				byte end_with = 0;
				byte ind = 0;
				while ((end_with < 7 || ind > 40)){ // We are looking for the * character and its position
								end_with = (str_out[ind] == '*') ? ind : 0;
								ind++;
				}
				if (ind > 40){ind = 2;} // In case we don't find it, we just break the fun(ction).
				for (byte x = 1; x<end_with; x++){ // XOR every character in between '$' and '*'
								CRC = CRC ^ str_out[x] ;
				}

			#ifdef BLUETOOTH
    BTSerial.print(str_out);
    BTSerial.println(CRC, HEX);
			#endif
			
    Serial.print(str_out);
    Serial.println(CRC, HEX);

    #ifdef SCREEN
    left.tick();
    right.tick();
			if (millis() >= get_time3 + 333){ // timed - every 1/3 second
			if(record){
       maxUpVario = (maxUpVario > vario) ? maxUpVario : vario;
       maxAltitude = (maxAltitude > Altitude) ? maxAltitude : Altitude;
			}

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(BLACK, WHITE); // 'inverted' text
    display.print(vario);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.print((vario >= 0) ? "   " : " ");

    if (page == 0) {
      display.setTextColor(BLACK, WHITE); // 'inverted' text
      display.print(Altitude); display.print("m");
      display.setCursor(66, 8);
      display.print(average_pressure); display.print("Pa");
    } else if ((page/64) == 1) {
      display.setTextColor(BLACK, WHITE);

      unsigned long allSeconds = (millis() - startTime) / 1000;
      byte runHours;
      runHours = allSeconds / 3600;
      long secsRemaining;
      secsRemaining = allSeconds % 3600;
      byte runMinutes;
      runMinutes = secsRemaining / 60;
      byte runSeconds;
      runSeconds = secsRemaining % 60;

      if (runHours < 10) {
        display.print('0');
      }
      display.print(runHours);
      display.print(':');
      if (runMinutes < 10) {
        display.print('0');
      }
      display.print(runMinutes);
      display.print(':');
      if (runSeconds < 10) {
        display.print('0');
      }
      display.println(runSeconds);
      display.setCursor(66, 8);
      display.print(lastLevel);
      display.print("%");
    } else if ((page/64) == 2) {
      display.setTextColor(BLACK, WHITE);
      display.print('+');
      display.println(maxUpVario);
      display.setCursor(66, 8);
      display.print(maxAltitude);
      display.println('m');
    } else {
      display.setCursor(66, 8);
      display.print(lastVoltage);
      display.print('V');
    }

    display.drawRect(1, 17, 126, 14, WHITE);
    display.drawLine(44, 29, 44, 32, WHITE);
    display.drawLine(24, 29, 24, 32, WHITE);
    display.drawLine(4, 29, 4, 32, WHITE);

    display.drawLine(84, 29, 84, 32, WHITE);
    display.drawLine(104, 29, 104, 32, WHITE);
    display.drawLine(124, 29, 124, 32, WHITE);

    int rounded_vario = round(vario * 10) * 2;
    if (rounded_vario > 0 && rounded_vario < 62 / 2) {
      display.fillRect(64, 18, rounded_vario, 12, WHITE);
    } else if (rounded_vario < 0 && rounded_vario > -62 / 2) {
      rounded_vario = -1 * rounded_vario;
      display.drawRect(64 - rounded_vario, 18, rounded_vario, 12, WHITE);
    } else {
      display.drawLine(64, 17, 64, 30, WHITE);
    }
    display.display();
			get_time3 = millis();
			} // timed - every 1/3 second
			#endif
			
				#ifdef SOUND
    if (vario > upBeepThreshold) {
      toneAC((690 + (150 * vario)), VOLUME, 166, true);
    } else if (vario < fallAlarmThreshold) {
      toneAC(123, VOLUME, 166, true);
    } else {
      toneAC();
    }
				#endif

		#ifdef BATTERY
  if ((get_time2 + 10000) <= millis()) {
    updateBattery();
    get_time2 = millis();
  }
	 #endif
}

//====BUTTONS FUNCTIONS====//
#ifdef SCREEN
void leftClick(){
				page -= 64;
}

void rightClick(){
				page += 64;
}

void leftLong(){
				if(!record){
								record = true;
								startTime = millis();
				}else{
								record = false;
				}
}
#endif

void rightLong(){
				
}
//====OTHER FUNCTIONS====//
#ifdef BATTERY
void updateBattery() {
  lastLevel = printLevel();
  lastVoltage = printVolts();
}
int printLevel()
{
  return battery.level();
}

float printVolts() {
  return (float)(battery.voltage() / 1000.0);
}
#endif

/* int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
} */ // This is litteraly debug. If you happen to have issues with stack ram crashes, this will give you how much SRAM is left in the arduino.

