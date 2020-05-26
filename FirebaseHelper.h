#ifndef FIREBASE_UTILS
#define FIREBASE_UTILS

#include <HTTPClient.h>
#include <ArduinoJson.h>

class FirebaseHelper
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

  String getDocuments(String localId, String token, int& responseCode)
  {
    String payload = "{}";
    String url = DOCUMENT_HOST + localId + "/";
    HTTPClient http;
    http.begin( url );
    http.addHeader("Authorization", token);
    responseCode = http.GET();
    if( responseCode > 0 ) payload = http.getString();
    http.end();
    return payload;
  }

  String refreshToken(String key, String rToken)
  {
    //create JSON
    const int capacity = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> paramjson;
    JsonObject object = paramjson.to<JsonObject>();
    object["grant_type"] = "refresh_token";
    object["refresh_token"] = rToken.c_str();
    String param;
    serializeJson(paramjson, param);

    //POST
    String url = REFRESH_HOST + key;
    HTTPClient http;
    http.begin( url );
    http.addHeader("Content-Type", "application/json");
    http.POST(param);
    String response = http.getString();
    Serial.println(response);
    http.end();
    return response;
  }
  
  String createDocuments(String email, String token, int& responseCode)
  {
    
  }

  int patch(String url, String _data, String token, String& payload)
  {
    HTTPClient http;
    http.begin( url );
    http.addHeader("Content-Type", "application/json");
    http.addHeader("'Accept", "application/json");
    //http.addHeader("Authorization", "Bearer " + token);
    String param = _data;
    int responseCode = http.PATCH(param);
    if( responseCode > 0 ) 
    {     
      payload = http.getString(); 
    }
    else Serial.print("ERROR" + String(responseCode));
    http.end();
    return responseCode;
  }

  String patch(String key, String localid, String token, String date, String timestamp, float data, bool documentExists = true)
  {
    
    String payload = "{}";
    String url = DOCUMENT_HOST + localid + "/" + "D"+date + "?currentDocument.exists=" + (documentExists ? "true":"false") + UPDATE_MASK +"datas.T" + timestamp; //firestore rest API doesn't accept updatemask without "D"
    url += "&key=" + key;
//    Serial.println(url);
    //Serial.println(token);
    String param = "{\"fields\": {\"datas\": {\"mapValue\": {\"fields\": {\"T"+ timestamp + "\": {\"doubleValue\": " + String(data)+"}}}}}}";
    int responseCode = patch(url, param, token, payload );
    if (responseCode == 404 && documentExists)
    {
        payload = patch(key, localid, token, date, timestamp, data, false);
        patchDocumentId(key, localid, token, date);
    }
//    Serial.println("RESPONSE CODE" + String(responseCode));
//    Serial.println(payload);
    return payload;
  }

  String patchDocumentId(String key, String localId, String token, String date )
  {
    String payload = "{}";
    String url = DOCUMENT_HOST + localId + "/" + "D"+date + "?currentDocument.exists=true"+ UPDATE_MASK +"documentId"; //firestore rest API doesn't accept updatemask without "D"
    url += "&key=" + key;
    String param = "{\"fields\":{\"documentId\":{\"stringValue\":\"D"+date+"\"}}}";
    int responseCode = patch(url, param, token, payload );
  }
  
private:
  const char *LOGIN_HOST = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=";
  const char *DOCUMENT_HOST = "https://firestore.googleapis.com/v1/projects/co2space-84757/databases/(default)/documents/";
  const char *UPDATE_MASK = "&mask.fieldPaths=fields&updateMask.fieldPaths=";
  const char *REFRESH_HOST = "https://securetoken.googleapis.com/v1/token?key=";
};

#endif
