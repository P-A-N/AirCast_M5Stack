#ifndef ALERT_LED
#define ALERT_LED

#define LED_PIN 2

class AlertLed{
public:
  AlertLed(){}
  
  void setup()
  {
    pinMode(LED_PIN, OUTPUT);
  }
  void update()
  {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }

  void turnOn(int interval)
  {
    
  }

  void turnOff()
  {
    
  }
private:
  bool bBlinkLed = false;
};

#endif
