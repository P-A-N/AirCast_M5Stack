#include <M5Stack.h>
#include <WiFi.h>
#include "Account.h"
#include "Co2Sensor.h"
#include "ConfigStore.h"

#define JST (3600L * 9 ) // +9:00 JST

Account _account;
Co2Sensor _co2sensor;
unsigned int _cur_time;
struct tm _tm;
ConfigStore _config;
int patchResponseCode;

void setup(){
  // Initialize the M5Stack object
  M5.begin();
  Serial.begin(9600);
  _config.setup();
//  _led.setup();
  configTime(JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  _cur_time = _timestamp();
  _co2sensor.setup();
}

// the loop routine runs over and over again forever 
void loop() {
  M5.update();
  _config.update();
  bool samplingResult = false;
  //update basic issue
  _cur_time = _timestamp();
  _account.update(_cur_time);
  bool bSampling = _co2sensor.isToUpdate(_cur_time);

  //sensor
  if( bSampling )
  {
    samplingResult = _co2sensor.sample(_cur_time, _config.voltage_adjust());
    M5.Lcd.clear(BLACK);
  }

  //draw stuffs
  if(!_config.isConfigMode())
  {
    drawTimestamp();
    _co2sensor.drawSensorValue();
  }
  _config.drawConfig();

  //update to server
  if ( samplingResult ){
    String today = String(_tm.tm_year + 1900);
    today += (_tm.tm_mon < 10 ) ? "0" + String(_tm.tm_mon+1) : String(_tm.tm_mon+1);
    today += (_tm.tm_mday < 10 ) ? "0" + String(_tm.tm_mday) : String(_tm.tm_mday);
    int resCode = 0;
    bool patched = _account.patchIfRequired(today, _cur_time, _co2sensor.getConcentration(), _config, resCode);
    if(patched) patchResponseCode = resCode;
    //Serial.println(payload);
  }
  
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor( 280, 10 );
  M5.Lcd.printf("%d", patchResponseCode);
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
