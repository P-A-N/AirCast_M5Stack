#ifndef CONFIG_STORE
#define CONFIG_STORE
#include "WiFiConnection.h"
#include <EEPROM.h>
#include "ConfigValue.h"
#define DATA_VERSION "DATA1.0"

struct CONFIG_DATA{
  bool bUpload;
  float adjustment;
  char check[7];
};

class ConfigStore
{
public:
 
  void setup()
  {
    Serial.println("Config setup!");
    EEPROM.begin(1024);
    EEPROM.get<CONFIG_DATA>(0, _data);
    if(strcmp(_data.check, DATA_VERSION)){
      Serial.println("init data!");
      _data.bUpload = false;
      _data.adjustment = 0.035;
    }
    
    _bSendValue.setup(0, "Send Data", _data.bUpload);
    _sensorAdjustment.setup(1,"Sensor Adjust",_data.adjustment);
    _exit.setup(2, "Exit", "<->");
    _wifi.setupConnection();
  }

  bool isConfigMode()
  {
    return _configMode;
  }

  void update()
  {
    if(!_configMode && M5.BtnA.wasPressed())
    {
      _configMode = true;
      _focus_index = 0;
      M5.Lcd.clear(BLACK);
    }
    else if(_configMode)
    {
      if(M5.BtnB.wasPressed())
      {
        _focus_index++;
        _focus_index %= 3;
        M5.Lcd.clear(BLACK);
      }
      _bSendValue.update(_focus_index);
      _sensorAdjustment.update(_focus_index);
      _exit.update(_focus_index);
      if((M5.BtnA.wasPressed() || M5.BtnC.wasPressed()) && _exit.isFocused())
      {
        _configMode = false;
        _data.bUpload = _bSendValue;
        _data.adjustment = _sensorAdjustment;
        strcpy(_data.check, DATA_VERSION);
        Serial.println(_data.check);
        EEPROM.put<CONFIG_DATA>(0, _data);
        EEPROM.commit();
        Serial.println("save data!");
        M5.Lcd.clear(BLACK);
      }
    }
  }

  void drawConfig()
  {
    if(_configMode)
    {
      _bSendValue.drawMenu();
      _sensorAdjustment.drawMenu(); 
      _exit.drawMenu(); 
      _wifi.drawWiFiInfo();
    }
    else
    {
      M5.Lcd.setCursor( 45, 225 );
      M5.Lcd.setTextSize(1);
      M5.Lcd.printf("Config");
    }
  }

  bool isSendToCloud()
  {
    return _bSendValue;
  }

  float voltage_adjust()
  {
    return _sensorAdjustment;
  }

private:
  bool _configMode = false;
  ConfigValue<bool> _bSendValue;
  ConfigValue<float> _sensorAdjustment;
  ConfigValue<String> _exit;
  WiFiConnection _wifi;
  unsigned int _focus_index = 0;
  CONFIG_DATA _data;
};
#endif
