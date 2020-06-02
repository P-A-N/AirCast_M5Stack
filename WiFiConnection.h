#ifndef WIFICONNECTION
#define WIFICONNECTION

class WiFiConnection
{ 
public:
  void setupConnection(String ssid, String wifi_password)
  {
    WiFi.begin(ssid.c_str(), wifi_password.c_str());
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      M5.Lcd.print('.');  
    }
    delay(500);
  }
  
  void drawWiFiInfo(String ssid)
  {
    M5.Lcd.setCursor( 0, 0 );
    M5.Lcd.setTextSize(1);
    M5.Lcd.print("\r\nWiFi [ ");
    M5.Lcd.print(ssid.c_str());
    M5.Lcd.print(" ] connectd \r\nIP address: ");
    M5.Lcd.println(WiFi.localIP());
  }

private:
};

#endif
