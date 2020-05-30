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

class ConfigStore
{
public:
 
  void setup()
  {
    Serial.println("Config setup!");
    _preferences.begin(KEY_PREF);
    
    _bSendValue.setup(0, "upload data");
    _adcAdjustment.setup(1,"ADC adjustment");
    _wifi_setup.setup(2, "wifi","setup");
    _exit.setup(3, "Exit", "<->");
    if(restoreConfig())
    {
      _wifi.setupConnection();
      _wifi_enabled = true;
      configTime(JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
    }
    else
    {
      _wifiConfig.enter();
      _configMode = true;
      _wifi_enabled = false;
    }
  }

  bool restoreConfig()
  {
    _wifi_ssid = _preferences.getString("WIFI_SSID");
    _wifi_password = _preferences.getString("WIFI_PASSWD");
    _bSendValue = _preferences.getBool(KEY_CLOUD);
    _adcAdjustment = _preferences.getFloat(KEY_ADC);
    
    Serial.print("WIFI-SSID: ");
    Serial.println(_wifi_ssid);
    Serial.print("WIFI-PASSWD: ");
    Serial.println(_wifi_password);
    
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
  void update()
  {
    if(!_wifiConfig.isWifiConfigMode() )updateGlobalConfig();
    else _wifiConfig.update();
  }

  void drawConfig()
  {
    if(!_wifiConfig.isWifiConfigMode() )drawGlobalConfig();
    else _wifiConfig.draw();
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
      if(M5.BtnB.wasPressed())
      {
        _focus_index++;
        _focus_index %= NUM_CONFIG;
        M5.Lcd.clear(BLACK);
      }
      _bSendValue.update(_focus_index);
      _adcAdjustment.update(_focus_index);
      _wifi_setup.update(_focus_index);
      _exit.update(_focus_index);
      if((M5.BtnA.wasPressed() || M5.BtnC.wasPressed()) && _wifi_setup.isFocused())
      {
        _wifiConfig.enter();
        M5.Lcd.clear(BLACK);
      }
      if((M5.BtnA.wasPressed() || M5.BtnC.wasPressed()) && _exit.isFocused())
      {
        _configMode = false;
        
        _preferences.putBool(KEY_CLOUD,_bSendValue);
        _preferences.putFloat(KEY_ADC,_adcAdjustment);
        Serial.println("save data!" + String(_bSendValue));
        M5.Lcd.clear(BLACK);
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
      _wifi.drawWiFiInfo();
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
