#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Initialize the LCD display module

char t[32];
void setup()
{
  Serial.begin(9600);
  Wire.begin();  // for ESP01 you have to change it to Wire.begin(2,0);
  
  lcd.begin();   // Initialize the LCD display module
  lcd.backlight();   // Turn on the LCD backlight
  lcd.clear();


  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__))); // Impontant note after first upload DS_1307 gets syncronize with current time
                                                 // so remove or commnet this line rtc.adjust after first upload and reupload the code
  //rtc.adjust(DateTime(2019, 1, 21, 5, 0, 0));
}
void loop()
{
  DateTime now = rtc.now();
  sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  
  Serial.print(F("Date/Time: "));
  Serial.println(t);
  lcd.setCursor(0, 0);
  lcd.print("Time: ");

  // Convert to 12-hour format
  int hour12 = now.hour() % 12;
  if (hour12 == 0) {
    hour12 = 12; // 12 AM
  }
  lcd.print(hour12);
  lcd.print(":");
  if (now.minute() < 10) {
    lcd.print("0");
  }
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10) {
    lcd.print("0");
  }
  lcd.print(now.second());

  // Print AM/PM indicator
  if (now.hour() < 12) {
    lcd.print(" AM");
  } else {
    lcd.print(" PM");
  }

  // Print abbreviated day and date
  lcd.setCursor(0, 1);
  char dayAbbrev[4];
  switch (now.dayOfTheWeek()) {
    case 0:
      strcpy(dayAbbrev, "SUN");
      break;
    case 1:
      strcpy(dayAbbrev, "MON");
      break;
    case 2:
      strcpy(dayAbbrev, "TUE");
      break;
    case 3:
      strcpy(dayAbbrev, "WED");
      break;
    case 4:
      strcpy(dayAbbrev, "THU");
      break;
    case 5:
      strcpy(dayAbbrev, "FRI");
      break;
    case 6:
      strcpy(dayAbbrev, "SAT");
      break;
  }
  lcd.print(dayAbbrev);
  lcd.print(" ");
  lcd.print(now.day());
  lcd.print("-");
  char monthAbbrev[4];
  switch (now.month()) {
    case 1:
      strcpy(monthAbbrev, "JAN");
      break;
    case 2:
      strcpy(monthAbbrev, "FEB");
      break;
    case 3:
      strcpy(monthAbbrev, "MAR");
      break;
    case 4:
      strcpy(monthAbbrev, "APR");
      break;
    case 5:
      strcpy(monthAbbrev, "MAY");
      break;
    case 6:
      strcpy(monthAbbrev, "JUN");
      break;
    case 7:
      strcpy(monthAbbrev, "JUL");
      break;
    case 8:
      strcpy(monthAbbrev, "AUG");
      break;
    case 9:
      strcpy(monthAbbrev, "SEP");
      break;
    case 10:
      strcpy(monthAbbrev, "OCT");
      break;
    case 11:
      strcpy(monthAbbrev, "NOV");
      break;
    case 12:
      strcpy(monthAbbrev, "DEC");
      break;
  }
  lcd.print(monthAbbrev);
  lcd.print("-");
  lcd.print(now.year());

  delay(1000);
}

