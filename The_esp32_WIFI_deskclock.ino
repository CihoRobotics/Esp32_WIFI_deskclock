//The .ino files here:

#include <WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "time.h"
#include <HTTPClient.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/5, /* data=*/4);

const char* ssid = "YOUR-WIFI-SSID";
const char* password = "YOUR-WIFI-PASSWORD";

const String API_KEY = "API-KEY-FROM-OPENWHEATHERAPP";
const String CITY_NAME = "CITY-NAME";
const String URL = "http://api.openweathermap.org/data/2.5/weather?q=" + CITY_NAME + "&appid=" + API_KEY + "&units=metric";
//this is for germany but you can change it!

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600; // GMT+1
const int   daylightOffset_sec = 3600;

String temperature = "0";
String timeString = "";

void setup() {
  Serial.begin(115200);

  u8g2.begin();

  displayStartupMessage();

  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Connected to WiFi");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  getWeatherData();
}

void loop() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  char buf[80];
  strftime(buf, sizeof(buf), "%H:%M:%S", timeinfo);
  timeString = String(buf);

  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_ncenB24_tr);
  u8g2.setCursor(0, 25);
  u8g2.print(timeString);
  
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 50);
  u8g2.print(temperature + " C");

  String dayOfWeek = getFullDayOfWeek(timeinfo);
  u8g2.setCursor(70, 50);
  u8g2.print(dayOfWeek);

  String date = getFormattedDate(timeinfo);
  u8g2.setCursor(70, 40);
  u8g2.print(date);
  
  int year = 1900 + timeinfo->tm_year;
  u8g2.setCursor(70, 60);
  u8g2.print(year);

  u8g2.sendBuffer();
  delay(1000);
}

void displayStartupMessage() {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_ncenB24_tr);
  int c_width = u8g2.getStrWidth("C");
  u8g2.setCursor((128 - c_width) / 2, 30);
  u8g2.print("C");
  
  u8g2.setFont(u8g2_font_ncenB08_tr);
  int ciho_width = u8g2.getStrWidth("Ciho Robotics");
  u8g2.setCursor((128 - ciho_width) / 2, 50);
  u8g2.print("Ciho Robotics");
  
  u8g2.sendBuffer();
  delay(3000);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(10, 30);
  u8g2.print("Connecting to WiFi...");
  u8g2.sendBuffer();
  delay(3000);
}

void getWeatherData() {
  HTTPClient http;
  http.begin(URL);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    int tempIndex = payload.indexOf("\"temp\":");
    if (tempIndex != -1) {
      temperature = payload.substring(tempIndex + 7, payload.indexOf(",", tempIndex));
    }
  } else {
    Serial.println("Error getting weather data");
    temperature = "N/A";
  }
  
  http.end();
}

String getFullDayOfWeek(struct tm* timeinfo) {
  String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  return days[timeinfo->tm_wday];
}

String getFormattedDate(struct tm* timeinfo) {
  char buf[20];
  strftime(buf, sizeof(buf), "%d/%m", timeinfo); // 28/04 formatı
  return String(buf);
}
