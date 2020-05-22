#include <M5Stack.h>
#include <WiFi.h>
#include "AlertLed.h"
#include "DataPush.h"
#include "Co2Sensor.h"

#define JST (3600L * 9 ) // +9:00 JST

AlertLed led;
DataPush dp;
Co2Sensor co2sensor;
unsigned int cur_time;
struct tm _tm;

void setup(){
  // Initialize the M5Stack object
  M5.begin();
  Serial.begin(9600);
  led.setup();
  dp.setupConnection();
  configTime(JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  cur_time = _timestamp();
  co2sensor.setup();
}

// the loop routine runs over and over again forever 
void loop() {
  cur_time = _timestamp();
  bool bSampling = co2sensor.isToUpdate(cur_time);
  //led.update();
  if( bSampling )
  {
    co2sensor.sample(cur_time);
    M5.Lcd.clear(BLACK);
  }
  drawTimestamp();
  co2sensor.drawSensorValue();
  dp.showWiFiInfo();
  if ( bSampling ){
    String today = String(_tm.tm_year + 1900);
    today += (_tm.tm_mon < 10 ) ? "0" + String(_tm.tm_mon+1) : String(_tm.tm_mon+1);
    today += (_tm.tm_mday < 10 ) ? "0" + String(_tm.tm_mday) : String(_tm.tm_mday);
    dp.patchIfRequired(today, cur_time, co2sensor.getConcentration());
    //Serial.println(payload);
  }
}

unsigned int _timestamp()
{
  unsigned int now_l = 0;
  if ( getLocalTime(&_tm))
  {
    time_t now = mktime(&_tm);
    now_l = now;
  }
  return now_l;
}

void drawTimestamp()
{ 
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor( 5, 40 );
  M5.Lcd.printf("%d/%2d/%2d", _tm.tm_year + 1900, _tm.tm_mon+1, _tm.tm_mday);
  M5.Lcd.setCursor( 10, 50 );
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("%0d:%02d:%02d", _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
}
