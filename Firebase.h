#ifndef PLAYER_UTILS
#define PLAYER_UTILS

#include <HTTPClient.h>
#include <ArduinoJson.h>

class Firebase
{
public:
  String login(String key, String email, String password)
  {
    //create JSON
    const int capacity = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<capacity> paramjson;
    JsonObject object = paramjson.to<JsonObject>();
    object["email"] = email.c_str();
    object["password"] = password.c_str();
    object["returnSecureToken"] = "true";
    String param;
    serializeJson(paramjson, param);

    //POST
    String url = LOGIN_HOST + key;
    HTTPClient http;
    http.begin( url );
    http.addHeader("Content-Type", "application/json");
    http.POST(param);
    String response = http.getString();
    http.end();
    return response;
  }

  String getDocuments(String email, String token, int& responseCode)
  {
    String payload = "{}";
    String url = DOCUMENT_HOST + email;
    HTTPClient http;
    http.begin( url );
    http.addHeader("Authorization", token);
    responseCode = http.GET();
    if( responseCode > 0 ) payload = http.getString();
    http.end();
    return payload;
  }

  String createDocuments(String email, String token, int& responseCode)
  {
    
  }

  String patch(String email, String token, String date, String timestamp, float data)
  {
//    D20200522.D1234569
    String payload = "{}";
    String url = DOCUMENT_HOST + email + UPDATE_MASK+"D"+date + ".D" + timestamp; //firestore rest API doesn't accept updatemask without "D"
    HTTPClient http;
    http.begin( url );
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", token);
    String param = "{\"fields\": {\"D" + date + "\": {\"mapValue\": {\"fields\": {\"D" + timestamp + "\": {\"doubleValue\": "+String(data)+"}}}}}}";
    int responseCode = http.PATCH(param);
    if( responseCode > 0 ) payload = http.getString();
    else Serial.print("ERROR" + String(responseCode));
    http.end();
    //Serial.println(payload);
    return payload;
  }
  
private:
  const char* LOGIN_HOST = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=";
  const char* DOCUMENT_HOST = "https://firestore.googleapis.com/v1/projects/co2space-84757/databases/(default)/documents/users/";
  const char* UPDATE_MASK = "?mask.fieldPaths=fields&updateMask.fieldPaths=";
};

#endif
