#ifndef DATA_PUSH
#define DATA_PUSH

#define EXPIRE_NOT_SET 0

#include <M5Stack.h>
#include <WiFi.h>
#include "Firebase.h"
#include <ArduinoJson.h>

class DataPush
{
public:
  DataPush(){}
  const char* ssid="CatCatCat_Extreme";
  const char* wifi_password="gogohorristic";
  unsigned int now_l;
  
  void setupConnection()
  {
    WiFi.begin(ssid, wifi_password);
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      M5.Lcd.print('.');  
    }
    delay(500);
  }

  void update(unsigned int cur_time)
  {
    if(expireTime == EXPIRE_NOT_SET)
    {
      login(cur_time);
    }
    if( cur_time + 600 > expireTime )
    {
      updateToken(cur_time);  
    }   
  }
  
  void showWiFiInfo()
  {
    M5.Lcd.setCursor( 0, 0 );
    M5.Lcd.print("\r\nWiFi [ ");
    M5.Lcd.print(ssid);
    M5.Lcd.print(" ] connectd \r\nIP address: ");
    M5.Lcd.println(WiFi.localIP());
  }

  bool patchIfRequired(String date, unsigned int timestamp, float co2data)
  {
    bool required = timestamp > last_patch_time + patch_interval;
    if(required)
    {
      String payload = api.patch(account_id, idToken, date, String(timestamp), co2data);
      last_patch_time = timestamp;
    }
    return required;
  }

  bool isDocumentExist()
  {
    int responseCode = 0;
    String payload = api.getDocuments(account_id, idToken, responseCode );
    if( responseCode > 0 )
    {
      Serial.print("HTTP Response code: ");
      Serial.println(responseCode);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(responseCode);
    }
    Serial.println(payload);
    return responseCode == 200;
  }

private:  
  WiFiClientSecure client;

  Firebase api;
  String app_key = "AIzaSyDe3NXAy7YddQfnoZc-0L6YEBDI95dYijs";
  String account_id = "hori.hiroyuki@gmail.com";
  String password = "oi9;NKA3d22G4z69";
  
  int expiresIn = 3600;//token valid sec
  
  unsigned int expireTime = EXPIRE_NOT_SET;
  const char* idToken;
  const char* _refreshToken;

  const int patch_interval = 300;
  unsigned int last_patch_time = 0;

  void login(unsigned int cur_time)
  {
    
    String response = api.login(app_key, account_id, password);
    const size_t capacity = JSON_OBJECT_SIZE(8) + 1920;
    DynamicJsonDocument doc(capacity);
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
    }
    expiresIn = doc["expiresIn"];
    idToken = doc["idToken"];
    _refreshToken = doc["refreshToken"];
    expireTime = cur_time + expiresIn;
  }

  
  void updateToken(unsigned int cur_time)
  {
    
  }
};
#endif
