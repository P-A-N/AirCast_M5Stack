#include <M5Stack.h>
#include <WiFi.h>
#include "Account.h"
#include "Co2Sensor.h"
#include "AppConfig.h"

Account _account;
Co2Sensor _co2sensor;
unsigned int _cur_time = 0;
struct tm _tm;
AppConfig _config;
int invalidResponseCodeCount = 0;
int patchResponseCode;

void setup(){
  // Initialize the M5Stack object
  M5.begin();
  Serial.begin(115200);

  //application and wifi config restore and connect to wifi
  _config.setup();
  //sensor setup(pin mode)
  _co2sensor.setup();
}

// the loop routine runs over and over again forever 
void loop() {
  M5.update();
  if(_config.manageWifiConnection())//return true when wifi config is not active, force restart when it is required
  {
    if(_cur_time == 0) _cur_time = _timestamp(_config.isWifiEnabled());
    _config.update();
    //get timestamp when wifi is not available this return millis()
    bool samplingResult = false;
    //update basic issue
    _cur_time = _timestamp(_config.isWifiEnabled());
    if(_config.isWifiEnabled()) _account.update(_cur_time);
    bool bSampling = _co2sensor.isToUpdate(_cur_time);
    //sensor
    if( bSampling )
    {
      samplingResult = _co2sensor.sample(_cur_time, _config.voltage_adjust());
      //M5.Lcd.clear(BLACK);
    }
    //draw stuffs
    if(!_config.isConfigMode())
    {
      if(_config.isWifiEnabled()) drawTimestamp();
      _co2sensor.drawSensorValue();
    }
    _config.drawConfig();
    //update to server
    if ( samplingResult && _config.isWifiEnabled()){
      String today = String(_tm.tm_year + 1900);
      today += (_tm.tm_mon < 10 ) ? "0" + String(_tm.tm_mon+1) : String(_tm.tm_mon+1);
      today += (_tm.tm_mday < 10 ) ? "0" + String(_tm.tm_mday) : String(_tm.tm_mday);
      int resCode = 0;
      bool patched = _account.patchIfRequired(today, _cur_time, _co2sensor.getConcentration(), _config, resCode);
      if(patched) 
      {
        patchResponseCode = resCode;
        if(patchResponseCode == 200) invalidResponseCodeCount = 0;
        else 
        {
          invalidResponseCodeCount++;
          _config.storeErrorMsg(_account.getPayload());
        }
        if(invalidResponseCodeCount > 30) ESP.restart();
      }
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor( 280, 10 );
      M5.Lcd.printf("%d", patchResponseCode);
      _config.drawErrorString();
    }
  }
  else
  {
    _config.updateWebServer();
  }
}

unsigned int _timestamp(bool wifi_enabled)
{
  unsigned int now_l = millis() / 1000;
  if ( wifi_enabled && getLocalTime(&_tm))
  {
    time_t now = mktime(&_tm);
    now_l = now;
  }
  if( !wifi_enabled )
  {
    if(now_l > 60*60*24*49) ESP.restart();//50日でmillisはoverflowするため
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
