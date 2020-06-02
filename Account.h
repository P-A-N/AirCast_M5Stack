#ifndef DATA_PUSH
#define DATA_PUSH

#define EXPIRE_NOT_SET 0
#define UPLOAD_INTERVAL_SEC 300

#include <M5Stack.h>
#include <WiFi.h>
#include "FirebaseHelper.h"
#include <ArduinoJson.h>
#include "AppConfig.h"

class Account
{
public:
  void update(unsigned int cur_time)
  {
    if(last_patch_time == 0) last_patch_time = cur_time;//skip first time
    if(expireTime == EXPIRE_NOT_SET)
    {
      Serial.println("log in");
      login(cur_time);
    }
    if( cur_time + 600 > expireTime )
    {
      updateToken(cur_time);  
    }
  }

  bool patchIfRequired(String date, unsigned int timestamp, float co2data, AppConfig& config, int& responseCode)
  {
    bool required = timestamp > last_patch_time + patch_interval;
    if(required && config.isSendToCloud())
    {
      String payload;
      responseCode = api.patch(app_key, localId, idToken, date, String(timestamp), co2data, payload);
      if(responseCode != 200)
      {
        Serial.println(payload);
      }
      last_patch_time = timestamp;
    }
    return required;
  }

  bool isDocumentExist()
  {
    int responseCode = 0;
    String payload = api.getDocuments(localId, idToken, responseCode );
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
  FirebaseHelper api;
  unsigned int now_l;
  
  String app_key = "AIzaSyDe3NXAy7YddQfnoZc-0L6YEBDI95dYijs";
  String account_id = "hori.hiroyuki@gmail.com";
  String password = "oi9;NKA3d22G4z69";
  
  int expiresIn = 3600;//token valid sec
  
  unsigned int expireTime = EXPIRE_NOT_SET;
  const char* idToken;
  const char* _refreshToken;

  const int patch_interval = UPLOAD_INTERVAL_SEC;
  unsigned int last_patch_time = 0;
  String localId;

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
    const char* tmp = (doc["localId"]);
    localId = String(tmp);
    expireTime = cur_time + expiresIn;
  }

  
  void updateToken(unsigned int cur_time)
  {
//    String response = api.refreshToken(app_key, _refreshToken);
//    const size_t capacity = JSON_OBJECT_SIZE(8) + 1920;
//    DynamicJsonDocument doc(capacity);
//    DeserializationError err = deserializeJson(doc, response);
//    if (err) {
//      Serial.print(F("deserializeJson() failed with code "));
//      Serial.println(err.c_str());
//    }
//    expiresIn = doc["expiresIn"];
//    idToken = doc["idToken"];
//    _refreshToken = doc["refreshToken"];
//    expireTime = cur_time + expiresIn;
  }
};
#endif
