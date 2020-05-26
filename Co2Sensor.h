#ifndef CO2_SENSOR
#define CO2_SENSOR

#define ADC_PIN 36

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
    if(_sampled_time == 0) _sampled_time = cur_time;
    return cur_time - _sampled_time > _interval;
  }

  bool sample(unsigned int cur_time, float adjustValue)
  {
    sensor_worked = false;
    int mv = analogRead(ADC_PIN);
    vOut = mv / 4095.0 * 3.3 + 0.1132 + adjustValue;
    _sampled_time = cur_time; 
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
        if(tmp == 0) concentration = tmp;
        else if(abs(tmp - concentration) < 1000 && tmp < 2000)
        {
          concentration = tmp + (concentration - tmp ) * filterVal; //lowpass filter
          sensor_worked = true;
        }
      }
      return sensor_worked;
    }
  }
  
  void drawSensorValue() const
  {
    //Serial.println( cur_time - _sampled_time );
    M5.Lcd.setTextSize(5);
    String outStr;
    if( _status == FAULT ) outStr = "FAULT";
    else if ( !sensor_worked ) outStr = "KEEP WAIT";
    else if( _status == PRE_HEATING ) outStr = "HEATING";
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
    bool sensor_worked;
    
};
#endif
