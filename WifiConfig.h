#ifndef WIFI_CONFIG_VALUE
#define WIFI_CONFIG_VALUE
#include <WiFi.h>
#include "WebServer.h"

class WifiConfig
{
public:
   WifiConfig():_webServer(80), _apIP(192,168,44,1){}
  void enter()
  {
    _bWifiConfigMode = true;
    _ssid = "";
    _pass = "";
    setupMode();
  }

  void exit()
  {
    _bWifiConfigMode = false;
  }

  bool getSetupData(String& ssid, String& pass)
  {
    if(_bWifiConfigMode && _ssid != "")
    {
      ssid = _ssid;
      pass = _pass;
      return true;
    }
    return false;
  }

  void update()
  {
    _webServer.handleClient();
  }
  
  bool isWifiConfigMode()
  {
    return _bWifiConfigMode;
  }
  
private:
  bool _bWifiConfigMode;
  IPAddress _apIP;
  const char* _apSSID = "AIRCAST_SETUP";
  String _pass, _ssid;
  String _ssidList;
  WebServer _webServer;

  void setupMode() {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    delay(300);
    Serial.println("");
    M5.Lcd.println("");
    for (int i = 0; i < n; ++i) {
      _ssidList += "<option value=\"";
      _ssidList += WiFi.SSID(i);
      _ssidList += "\">";
      _ssidList += WiFi.SSID(i);
      _ssidList += "</option>";
    }
    delay(100);
    WiFi.softAPConfig(_apIP, _apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(_apSSID);
    WiFi.mode(WIFI_MODE_AP);
    // WiFi.softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
    // WiFi.softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0);
    // dnsServer.start(53, "*", apIP);
    startWebServer();
    Serial.print("Starting Access Point at \"");
    M5.Lcd.print("Please Choose WIFI of \"");
    Serial.print(_apSSID);
    M5.Lcd.print(_apSSID);
    Serial.println("\"");
    M5.Lcd.println("\"");

    String _url = WiFi.softAPIP().toString();
    M5.Lcd.print("and setup with http://"+_url+"/settings");
    M5.Lcd.qrcode("http://"+_url+"/settings", 5, 45, 190);//url, posx, posy, size, version
    
  }

  void startWebServer() {
    Serial.print("Starting Web Server at ");
    M5.Lcd.print("Starting Web Server at ");
    Serial.println(WiFi.softAPIP());
    M5.Lcd.println(WiFi.softAPIP());
    _webServer.on("/settings", [&]() {
      String s = "<h1>Wi-Fi Settings</h1><p>Please enter your password by selecting the SSID.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += _ssidList;
      s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\"><input type=\"submit\"></form>";
      s = makePage("Wi-Fi Settings", s);
      _webServer.send(200, "text/html", s);
    });
    _webServer.on("/setap", [&]() {
      
      _ssid = WebServer::urlDecode(_webServer.arg("ssid"));
      Serial.print("SSID: ");
      M5.Lcd.print("SSID: ");
      Serial.println(_ssid);
      M5.Lcd.println(_ssid);
      _pass = WebServer::urlDecode(_webServer.arg("pass"));
      Serial.print("Password: ");
      M5.Lcd.print("Password: ");
      Serial.println(_pass);
      M5.Lcd.println(_pass);
      Serial.println("Writing SSID to EEPROM...");
      M5.Lcd.println("Writing SSID to EEPROM...");
      // Store wifi config
      Serial.println("Writing Password to nvr...");
      M5.Lcd.println("Writing Password to nvr...");

      Serial.println("Write nvr done!");
      M5.Lcd.println("Write nvr done!");
      String s = "<h1>Setup complete.</h1><p>device will be connected to \"";
      s += _ssid;
      s += "\" after the restart.";
      s = makePage("Wi-Fi Settings", s);
      _webServer.send(200, "text/html", s);
    });
      _webServer.onNotFound([&]() {
      String s = "<h1>AP mode</h1><p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      s = makePage("AP mode", s);
      _webServer.send(200, "text/html", s);
    });
    _webServer.begin();
  }

  String makePage(String title, String contents) {
    String s = "<!DOCTYPE html><html><head>";
    s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
    s += "<title>";
    s += title;
    s += "</title></head><body>";
    s += contents;
    s += "</body></html>";
    return s;
  }
};

#endif
