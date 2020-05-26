#ifndef WIFICONNECTION
#define WIFICONNECTION

class WiFiConnection
{ 
public:
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
  
  void drawWiFiInfo()
  {
    M5.Lcd.setCursor( 0, 0 );
    M5.Lcd.setTextSize(1);
    M5.Lcd.print("\r\nWiFi [ ");
    M5.Lcd.print(ssid);
    M5.Lcd.print(" ] connectd \r\nIP address: ");
    M5.Lcd.println(WiFi.localIP());
  }

private:
  const char* ssid="CatCatCat_Extreme";
  const char* wifi_password="gogohorristic";
};

#endif
