#include <Wire.h>
#include <SI7021_neo.h>


SI7021 sensor;

void setup() {
    Serial.begin(115200);
    pinMode(13, OUTPUT);
    
    sensor.begin();
    delay(2000);
    
    Serial.println("UDOO Humidity Brick");
    delay(100);
}


void loop() {    
    // this driver should work for SI7001, this returns 6
    int deviceid = sensor.getDeviceId();
    Serial.print("deviceId ");
    Serial.println(deviceid);
    delay(1000);
    
    // temperature is an integer in hundredths
    int temperature = sensor.getCelsiusHundredths();
    temperature = temperature / 100; 
    Serial.print("temp C ");
    Serial.println(temperature);  
    delay(1000);
    
    // humidity is an integer representing percent
    int humidity = sensor.getHumidityPercent(); 
    Serial.print("humidity percent ");
    Serial.println(humidity);    
    delay(1000);

    /* it's possible to enable internal heater for testing
    sensor.setHeater(true);
    delay(20000);
    sensor.setHeater(false);
    
    // see if heater changed temperature
    temperature = sensor.getCelsiusHundredths();
    temperature = temperature / 100;   
    Serial.print("temp C after heating ");
    Serial.println(temperature);  
    
    //cool down
    delay(20000);
    */
    
    // get humidity and temperature in one shot, saves power because sensor takes temperature when doing humidity anyway
    si7021_env data = sensor.getHumidityAndTemperature();
    Serial.print("ONE SHOT: temp ^C ");
    Serial.print(data.celsiusHundredths/100);  
    Serial.print("- hum ");
    Serial.println(data.humidityPercent);  

    delay(1000);
}


