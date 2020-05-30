#ifndef CONFIG_VALUE
#define CONFIG_VALUE

template<class T>
class ConfigValue
{
  public:
  String _title;
  T _targetValue;
  
  void setup(int index, String title )
  {
    _index = index;
    _title = title;
  }
  
  void setup(int index, String title, T value )
  {
    setup(index, title);
    _targetValue = value;
    Serial.print(_targetValue);
  }

  operator T() const
  {
    return (T)_targetValue;
  }

  void update(int focus_index)
  {
    _focused = _index == focus_index;
    updateValue(_targetValue, M5.BtnA.wasPressed(), M5.BtnC.wasPressed());
  }
  
  void drawMenu() const
  {
    M5.Lcd.setCursor( 10, 20 * _index + 30 );
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf(_title.c_str());
    if(_focused) M5.Lcd.drawLine( 5, 20 * _index + 46, 310, 20 * _index + 46, YELLOW);
    drawValue(_targetValue, _focused);
  }

  void drawValue(float __val, bool focus) const
  {
    M5.Lcd.setCursor( 250, 20 * _index + 30 );
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf((String("0.0")+String(int(__val*1000))).c_str());
  }

  void drawValue(bool __val, bool focus) const
  {
    M5.Lcd.setCursor( 250, 20 * _index + 30 );
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf(__val ? "[YES]" : "[NO ]");
  }

  void drawValue(String __val, bool focus) const
  {
    M5.Lcd.setCursor( 250, 20 * _index + 30 );
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf(__val.c_str());
  }

  void updateValue(float __val, bool leftbtn, bool rightbtn)
  {
    if(_focused)
    {
      if(leftbtn)
      {
        _targetValue -= 0.001;
      }
      else if (rightbtn)
      {
        _targetValue += 0.001;
      } 
    }
  }

  void updateValue(bool __val, bool leftbtn, bool rightbtn)
  {
    
    if(_focused)
    {
      if(leftbtn || rightbtn ) _targetValue = !__val;
    }
  }
  
  void updateValue(String __val, bool leftbtn, bool rightbtn) {}

  ConfigValue<T>& operator=(const T& other)
  {
      _targetValue = other;
      return *this;
  }

  bool isFocused() {return _focused;}
private:
  bool _focused = false;
  unsigned int _index = 0;
  
};
#endif
