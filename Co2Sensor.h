#ifndef CO2_SENSOR
#define CO2_SENSOR

#define ADC_PIN 35

class Co2Sensor
{
public:
  enum STATUS
  {
    FAULT, PRE_HEATING, NORMAL
  } _status;
  void setup()
  {
    _sampled_time = 0;
    pinMode(ADC_PIN, INPUT); 
  }

  bool isToUpdate(unsigned int cur_time)
  {
    return cur_time - _sampled_time > _interval;
  }

  void sample(unsigned int cur_time)
  {
    
    int mv = analogRead(ADC_PIN);
    vOut = mv / 4095.0 * 3.3 + 0.1132;
    if( vOut < 0 ) _status = FAULT;
    else if( vOut < 0.4 ) _status = PRE_HEATING;
    else {
      _status = NORMAL;
      float voltageDiference = vOut - 0.4;
      float tmp = (voltageDiference*5000.0)/1.6;
      if(tmp < 0)
      {
        concentration = 0;
      }
      else
      {
        if(concentration == 0) concentration = tmp;
        else 
        {
          concentration = tmp + (concentration - tmp ) * filterVal; //lowpass filter
        }
      }
      _sampled_time = cur_time;  
    }
  }
  
  void drawSensorValue()
  {
    //Serial.println( cur_time - _sampled_time );
    M5.Lcd.setTextSize(5);
    String outStr;
    if( _status == FAULT ) outStr = "FAULT";
    else if( _status == PRE_HEATING ) outStr = "PRE_HEATING";
    else outStr = String(concentration) + "ppm";
    M5.Lcd.setCursor( 10, 80 );
    M5.Lcd.println(outStr);
    
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor( 10, 120 );
    M5.Lcd.print("voltage:");
    M5.Lcd.println(vOut);
  }

  float getConcentration()
  {
    return concentration;
  }

  private:
    unsigned int _sampled_time;
    const int _interval = 1;
    float vOut;
    float filterVal = 0.1;
    float concentration;
    
};
#endif
