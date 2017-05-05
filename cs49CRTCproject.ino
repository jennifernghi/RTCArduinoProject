#include <Adafruit_ST7735.h> //library for the 1.8 TFT ST7735
#include "Sodaq_DS3231.h" //library used for rtc DS3231
#include <Wire.h>
#define TFT_CS     10
#define TFT_DC     9
#define TFT_RST    8

//create object Adafruit_ST7735 (constructor specified in Adafruit_ST7735 library)
Adafruit_ST7735 screen = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

//prototypes
void setUpScreen();
void setText(char *text, uint16_t color, int16_t x, int16_t y, uint8_t textSize);
float CToF(float celcius);
String getDayOfWeekString(uint8_t );
String getMonthString(uint8_t);
void stringToCharArray(char * arrayDestination, int size, String string);

//constant strings
char const * TEMP = "Temperature";
char const * DEGREE_SYMBOL = "o";
char const * CELSIUS = "C";
char const * FAHRENHEIT = "F";

/*
 * hold data for the device
 */
struct time {
  float currentCtemperature;
  float previousCTemperature;
  int currentSecond;
  int previousSecond;
  char timeString[50];
  char dateString[50];
  char tempCString[10];
  char tempFString[10];
};
 //initialize struct time
struct time timeControl = {0, 0, 0, 0,"","","",""};
void setup ()
{
  Serial.begin(57600);
  rtc.begin(); //Initializes the internal RTC, rtc is defined in Sodag_DS3231.h : extern Sodaq_DS3231 rtc;
  Wire.begin();
  setUpScreen();//set up screen with colors and fixed texts
  //setManualTime();
}

uint32_t old_ts;

void loop ()
{
  rtc.convertTemperature(); //convert current temp to register
  timeControl.currentCtemperature = rtc.getTemperature();

  DateTime currentTime = rtc.now(); //get the current date-time
  uint32_t ts = currentTime.getEpoch();

  if (old_ts == 0 || old_ts != ts) {
    old_ts = ts;
    timeControl.currentSecond = currentTime.second();
    if (timeControl.currentSecond != timeControl.previousSecond)
    {
      timeControl.previousSecond = timeControl.currentSecond;
      /*
         Arduino's String object
      */
      //ex: 12:04:05
      String timeText = String(currentTime.hour());
      if (currentTime.minute() < 10)
      {
        timeText = timeText + ":0" + String(currentTime.minute());
      } else
      {
        timeText = timeText + ":" + String(currentTime.minute());
      }

      if (currentTime.second() < 10)
      {
        timeText = timeText + ":0" + String(currentTime.second());
      } else
      {
        timeText = timeText + ":" + String(currentTime.second());
      }

      //ex Thurs, May 4 2017
      String dateText = getDayOfWeekString(currentTime.dayOfWeek())
                        + ", "
                        + getMonthString(currentTime.month())
                        + " "
                        + String(currentTime.date())
                        + " "
                        + String(currentTime.year());




      //hours.toCharArray(timeString, 100);
      stringToCharArray(timeControl.timeString, 50, timeText);
      screen.fillRect(0, 0, 160, 65, ST7735_BLACK);
      setText(timeControl.timeString, ST7735_WHITE, 20, 10, 2);

      stringToCharArray(timeControl.dateString, 50, dateText);
      //dateString.toCharArray(dateString, 30);
      setText(timeControl.dateString, ST7735_WHITE, 15, 35, 1);
    }

    if (timeControl.currentCtemperature != timeControl.previousCTemperature)
    {
      timeControl.previousCTemperature = timeControl.currentCtemperature;
      
      String temperatureCString = String(timeControl.currentCtemperature, 1); // using a float and the decimal places
      stringToCharArray(timeControl.tempCString, 10, temperatureCString);

      //update F temperature according to change of C temperature
      String temperatureFString = String(CtoF(timeControl.currentCtemperature), 1); // using a float and the decimal places
      stringToCharArray(timeControl.tempFString, 10, temperatureFString);

      //print the text  of temperature store on temperatureChar to the screen
      screen.fillRect(0, 75, 128, 30, ST7735_BLACK);
      setText(timeControl.tempCString, ST7735_WHITE, 10, 80, 3);
      setText(DEGREE_SYMBOL, ST7735_WHITE, 85, 75, 2);
      setText(CELSIUS, ST7735_WHITE, 105, 80, 3);



      //print the text  of temperature store on temperatureChar to the screen
      screen.fillRect(0, 115, 128, 30, ST7735_BLACK);
      setText(timeControl.tempFString, ST7735_WHITE, 10, 120, 3);
      setText(DEGREE_SYMBOL, ST7735_WHITE, 85, 115, 2);
      setText(FAHRENHEIT, ST7735_WHITE, 105, 120, 3);
    }
  }
  delay(100); //delay each loop for 1 second
}
void setUpScreen() {
  //initialize the TFT screen
  screen.initR(INITR_BLACKTAB);
  //fill the screen with black color
  screen.fillScreen(ST7735_BLACK);

  /*
     print
     ____________Temperature____________
  */
  screen.drawFastHLine( 0,  70,  35,  ST7735_RED);
  setText(TEMP, ST7735_BLUE, 35, 65, 1);
  screen.drawFastHLine( 101,  70,  27,  ST7735_RED);
}

/**
 * convert to farenheight
 */
float CtoF(float celsiusTemp) {
  return (celsiusTemp * 1.8) + 32;
}
/**
 * set time manually
 */
void setManualTime()
{
  rtc.setDateTime(DateTime(2017, 5, 4, 10, 51, 55, 4));
}

/**
 * set color, position and size to text
 * and print to the screen 
 */
void setText(char *text, uint16_t color, int16_t x, int16_t y, uint8_t textSize)
{
  /*
     folowing methods defined in Adafruit_GFX.h library
     Adafruit_ST7735 is derived form Adafruit_GFX.h
     we can call these methods using Adafruit_ST7735 object.
  */
  screen.setTextColor(color);//set text color
  screen.setTextSize(textSize); //set textsize
  screen.setCursor(x, y); //set coordinate
  /*
    print method is defined in Print.h library
    Adafruit_GFX is derived from Print.h
    size_t print(char);
  */
  screen.print(text); //print text on the screen
}

/**
 * convert String object (Arduino) to char array
 */
void stringToCharArray(char * arrayDestination, int size, String string) {
  //using toCharArray(char[], size) of String class
  string.toCharArray(arrayDestination, size);
}

/*
   get month in string form given month number
*/
String getMonthString(uint8_t i) {
  switch (i)
  {
    case 1: return "January";
      break;
    case 2: return "Febuary";
      break;
    case 3: return "March";
      break;
    case 4: return "April";
      break;
    case 5: return "May";
      break;
    case 6: return "June";
      break;
    case 7: return "July";
      break;
    case 8: return "August";
      break;
    case 9: return "september";
      break;
    case 10: return "October";
      break;
    case 11: return "November";
      break;
    case 12: return "December";
      break;
    default: return "December";
      break;
  }
}

/*
   get day of the week in string form given it's number
*/
String getDayOfWeekString(uint8_t i)
{
  switch (i)
  {
    case 1: return "Mon";
      break;
    case 2: return "Tue";
      break;
    case 3: return "Wed";
      break;
    case 4: return "Thurs";
      break;
    case 5: return "Fri";
      break;
    case 6: return "Sat";
      break;
    case 7: return "Sun";
      break;
    default: return "Sun";
      break;
  }
}





