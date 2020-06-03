#ifndef WIFICONNECTION
#define WIFICONNECTION

#define JST (3600L * 9 ) // +9:00 JST

class WiFiConnection
{ 
public:
  bool setupConnection(String ssid, String wifi_password, bool& aborted)
  {
    aborted = false;
    WiFi.begin(ssid.c_str(), wifi_password.c_str());
    WiFi.setAutoReconnect(true);
    int timeupCount = 0;
    while(WiFi.status() != WL_CONNECTED)
    {
      M5.update();
      aborted = M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed();
      delay(500);
      M5.Lcd.print('.');  
      timeupCount++;
        if( timeupCount > 30 || aborted) break;
    }
    if(WiFi.status() == WL_CONNECTED)
    {
      configTime(JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
    }
    return WiFi.status() == WL_CONNECTED;
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
