
#include <Wire.h>
int dataHighAddress = 0x01;
int dataLowAddress = 0x02;
byte data = 0xff;
byte dataH;
byte dataL;
int tong;
int tong2;
float Temperature, Humidity;
void setup()
{
  Wire.begin();
  delay(500);
  Serial.begin(115200);
}

void loop()
{

  Wire.beginTransmission(0x40);
  Wire.write(0x03);
  Wire.write(0x11);
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.requestFrom(0x40, 3);
  while (Wire.available())
  {
    data = Wire.read();
    tong2 = (Wire.read() << 8);
    tong2 |= Wire.read();
    tong2 = tong2 >> 2;
  }
  Wire.endTransmission();
  Temperature = (tong2 / 32.00) - 50;
  Serial.print( "Temperature = " );
  Serial.println( Temperature );

  // Start the Humidity sensor
  Wire.beginTransmission(0x40);
  Wire.write(0x03);
  Wire.write(0x01);
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.requestFrom(0x40, 3);
  while (Wire.available())
  {
    data = Wire.read();
    tong = (Wire.read() << 8);
    tong |= Wire.read();
    tong = tong >> 4;
    Wire.endTransmission();
    Humidity = (tong / 16.00) - 24;
    Serial.print( "Humidity = " );
    Serial.println( Humidity );
    delay(100);
  }
}
