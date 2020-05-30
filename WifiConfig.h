#ifndef WIFI_CONFIG_VALUE
#define WIFI_CONFIG_VALUE
class WifiConfig
{
public:
  void enter()
  {
    _bWifiConfigMode = true;
  }

  void update()
  {
    
  }

  void draw()
  {
    
  }

  void exit()
  {
    _bWifiConfigMode = false;
  }
  
  bool isWifiConfigMode()
  {
    return _bWifiConfigMode;
  }
  
private:
  bool _bWifiConfigMode;

};

#endif
