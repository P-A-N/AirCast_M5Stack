#ifndef CONFIG_STORE
#define CONFIG_STORE
#include "WiFiConnection.h"
#include "WifiConfig.h"
#include "config/ConfigValue.h"
#include <Preferences.h>
#define NUM_CONFIG (4)
#define KEY_PREF "aircast-config"
#define KEY_CLOUD "upload-data"
#define KEY_ADC "adc-adjustment"
#define KEY_SSID "wifi-ssid"
#define KEY_PASS "wifi-password"

#define JST (3600L * 9 ) // +9:00 JST

class AppConfig
{
public:
 
  void setup()
  {
    Serial.println("Config setup!");
    _preferences.begin(KEY_PREF);
    
    _bSendValue.setup(0, "upload data", true);
    _adcAdjustment.setup(1,"ADC adjustment", 0.035);
    _wifi_setup.setup(2, "wifi","setup");
    _exit.setup(3, "Exit", "<->");
    _settingResored = restoreConfig();
    if(_settingResored)
    {
      _wifi_enabled = _wifi.setupConnection(_wifi_ssid,_wifi_password);
      if(_wifi_enabled)
        configTime(JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
    }
    Serial.print("wifi enabled:");
    Serial.println(_wifi_enabled);
    Serial.print("upload enabled:");
    Serial.println(_bSendValue);
    if(!_wifi_enabled && _bSendValue)
    {
      _wifiConfig.enter();
      _configMode = true;
      _wifi_enabled = false;
    }
  }

  bool restoreConfig()
  {
    _wifi_ssid = _preferences.getString(KEY_SSID);
    _wifi_password = _preferences.getString(KEY_PASS);
    _bSendValue = (boolean)_preferences.getBool(KEY_CLOUD);
    float tmpfloat = (float)_preferences.getFloat(KEY_ADC);
    if(!isnan(tmpfloat)) _adcAdjustment = tmpfloat;
    
    Serial.print("WIFI-SSID: ");
    Serial.println(_wifi_ssid);
    Serial.print("WIFI-PASSWD: ");
    Serial.println(_wifi_password);
    Serial.print("UPDATE-value: ");
    Serial.println(_bSendValue);
    Serial.print("Adjust-value: ");
    Serial.println(_adcAdjustment);
    
    if(String(_wifi_ssid).length() > 0) {
      return true;
    } else {
      return false;
    }
  }

  bool isConfigMode()
  {
    return _configMode;
  }

  bool isWifiEnabled()
  {
    return _wifi_enabled;
  }
  
  bool update()
  {
    if(!_wifiConfig.isWifiConfigMode() )updateGlobalConfig();
    else _wifiConfig.update();
    if(_wifiConfig.getSetupData(_wifi_ssid, _wifi_password))
    {
      Serial.println("save & restart" + _wifi_ssid + "/" + _wifi_password);
      _preferences.putString(KEY_SSID,_wifi_ssid);
      _preferences.putString(KEY_PASS,_wifi_password);
      delay(3000);
      ESP.restart();
    }
    return !_wifiConfig.isWifiConfigMode();
  }

  void drawConfig()
  {
    if(!_wifiConfig.isWifiConfigMode() )drawGlobalConfig();
  }

  bool isSendToCloud()
  {
    return _bSendValue;
  }

  float voltage_adjust()
  {
    return _adcAdjustment;
  }

private:
  bool _configMode = false;
  boolean _settingResored = false;
  ConfigValue<bool> _bSendValue;
  ConfigValue<float> _adcAdjustment;
  ConfigValue<String> _exit;
  ConfigValue<String> _wifi_setup;
  String _wifi_ssid;
  String _wifi_password;
  WiFiConnection _wifi;
  unsigned int _focus_index = 0;
  Preferences _preferences;
  WifiConfig _wifiConfig;
  bool _wifi_enabled = false;

  void updateGlobalConfig()
  {
    if(!_configMode && M5.BtnB.wasPressed())
    {
      _configMode = true;
      _focus_index = 0;
      _bSendValue.update(_focus_index);
      _adcAdjustment.update(_focus_index);
      _wifi_setup.update(_focus_index);
      _exit.update(_focus_index);
      M5.Lcd.clear(BLACK);
    }
    else if(_configMode)
    {
      //focus down
      if(M5.BtnB.wasPressed())
      {
        _focus_index++;
        _focus_index %= NUM_CONFIG;
        M5.Lcd.clear(BLACK);
      }

      //update focus
      _bSendValue.update(_focus_index);
      _adcAdjustment.update(_focus_index);
      _wifi_setup.update(_focus_index);
      _exit.update(_focus_index);

      //change config
      if(M5.BtnA.wasPressed() || M5.BtnC.wasPressed()) 
      {
        if( _wifi_setup.isFocused() && !_wifiConfig.isWifiConfigMode())
        {          
          M5.Lcd.clear(BLACK);
          _wifiConfig.enter();
          _configMode = true;
          _wifi_enabled = false;
        }
        else if( _exit.isFocused())
        {
          _configMode = false;
          _preferences.putBool(KEY_CLOUD,_bSendValue);
          _preferences.putFloat(KEY_ADC,_adcAdjustment);
          Serial.println("save data!" + String(_bSendValue));
          M5.Lcd.clear(BLACK);
        }
      }
    }
  }

  void drawGlobalConfig()
  {
    if(_configMode)
    {
      _bSendValue.drawMenu();
      _adcAdjustment.drawMenu(); 
      _wifi_setup.drawMenu();
      _exit.drawMenu(); 
      _wifi.drawWiFiInfo(_wifi_ssid);
    }
    else
    {
      M5.Lcd.setCursor( 145, 225 );
      M5.Lcd.setTextSize(1);
      M5.Lcd.printf("Config");
    }
  }
};
#endif
