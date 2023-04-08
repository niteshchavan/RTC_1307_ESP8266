//This is updated version which directly connect to Wifi and sync time with internet..  

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = " ";
const char* password = "   ";
char t[32];

IPAddress timeServerIP;
const char* ntpServerName = "pool.ntp.org";

WiFiUDP udp;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Initialize the LCD display module

void setup() {
  Serial.begin(115200);
  Wire.begin(2,0);
  rtc.begin();
  
  lcd.begin();   // Initialize the LCD display module
  lcd.backlight();   // Turn on the LCD backlight
  lcd.clear();
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Get the IP address of the NTP server
  WiFi.hostByName(ntpServerName, timeServerIP);

  // Set the date and time using NTP
  Serial.println("Setting time using NTP");
  setRTCUsingNTP();
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
void setRTCUsingNTP() {
  const int NTP_PACKET_SIZE = 48;
  byte packetBuffer[NTP_PACKET_SIZE];
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  udp.begin(123);
  udp.beginPacket(timeServerIP, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  
  delay(1000);

  if (udp.parsePacket()) {
    byte ntpBuffer[NTP_PACKET_SIZE];
    udp.read(ntpBuffer, NTP_PACKET_SIZE);

    uint32_t secsSince1900 = (uint32_t)ntpBuffer[40] << 24 | (uint32_t)ntpBuffer[41] << 16 | (uint32_t)ntpBuffer[42] << 8 | (uint32_t)ntpBuffer[43];
    uint32_t epoch = secsSince1900 - 2208988800UL;

    // Convert epoch time to Kolkata time
    DateTime dt = DateTime(epoch + 19800); // 19800 seconds offset for IST (5:30 hours)
    rtc.adjust(dt);
    	
    Serial.println("Time set successfully");
  }
  else {
    Serial.println("Error getting NTP time");
  }
}
