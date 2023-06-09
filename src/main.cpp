/********************************************************
 * Pálmi R. Pétursson 2020.06.19
 * Updated 2020.05.14
 * Code applicable for any of the SenseAir CO2 sensors; S8, S11, Sunrise, Sunlight
 * Uses the SoftwareSerial library
 ********************************************************/
#include <SoftwareSerial.h>
const int LED_PIN = 5;

SoftwareSerial Sunrise_Serial(32,33);  //Sets up a virtual serial port
                                  //Using pin 32 for Tx and pin 33 for Rx
                                    
unsigned long prevReadTime = 0;
unsigned int ReadTimeInterval = 8000; //Update interval in ms
unsigned long valCO2 = 0;
int intCO2=0;
byte readCO2[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
byte response[] = {0,0,0,0,0,0,0};  

int valMultiplier = 1;

void sendRequest(byte packet[])
{
  while(!Sunrise_Serial.available())  //keep sending request until we start to get a response
  {
    Sunrise_Serial.write(readCO2,7);
    delay(1000);
  }

  int timeout=0;  //set a timeoute counter
  while(Sunrise_Serial.available() < 7 ) //Wait to get a 7 byte response
  {
    timeout++;
    if(timeout > 10)    //if it takes to long there was probably an error
      {
        while(Sunrise_Serial.available())  //flush whatever we have
          Sunrise_Serial.read();

          break;                        //exit and try again
      }
      delay(50);
  }

  for (int i=0; i < 7; i++)
  {
    response[i] = Sunrise_Serial.read();
  }
}

unsigned long getValue(byte packet[])
{
    int high = packet[3];                        //high byte for value is 4th byte in packet in the packet
    int low = packet[4];                         //low byte for value is 5th byte in the packet
    unsigned long val = high*256 + low;                //Combine high byte and low byte with this formula to get value
    return val* valMultiplier;
}

void setup()
{
  Serial.begin(115200);    
  delay(10);
  Sunrise_Serial.begin(9600);    
  digitalWrite(LED_PIN, HIGH);

}

void loop()
{
    //Serial.print("Testing 1");
    if(millis() - prevReadTime > ReadTimeInterval){
      prevReadTime = millis();
      sendRequest(readCO2);
      unsigned long tmpValCO2 = getValue(response);
      valCO2 = -1;
      if(tmpValCO2 != valCO2){
        valCO2 = tmpValCO2;
        Serial.print("CO2 ppm = ");
        Serial.println(valCO2);
        //delay(12000);
      }
    }
}

