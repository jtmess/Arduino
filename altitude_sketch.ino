//Jake Messner and Javier Chia-Hsing Yu
//Project Defeat Icarus, 2013-2014

#include <Wire.h> 

#define MPL3115A2_ADDRESS 0x60 

void setup()
{
  Wire.begin();       
  Serial.begin(9600);  

  MPL_begin();

 
  setModeAltimeter(); 
  //setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  
  setOversampleRate(4);
  enableEventFlags(); 
  setModeActive(); 
}

void loop()
{
  float altitude = readAltitude();
  Serial.print("Altitude(m):");
  Serial.print(altitude, 2);

  altitude = readAltitudeFt();
  Serial.print(" Altitude(ft):");
  Serial.print(altitude, 2);

  /*float pressure = readPressure();
  Serial.print(" Pressure(Pa):");
  Serial.println(pressure, 2);*/

  //float temperature = readTemp();
  //Serial.print(" Temp(c):");
  //Serial.print(temperature, 2);

  float temperature = readTempF();
  Serial.print(" Temp(f):");
  Serial.print(temperature, 2);

  Serial.println();

  delay(100);
}

boolean MPL_begin()
{
  if(IIC_Read(WHO_AM_I) == 196) return(true); 
  
  return(false); 
}


float readAltitude()
{

  while( (IIC_Read(STATUS) & (1<<2)) == 0) break; 

  // Read pressure registers
  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(OUT_P_MSB);  
  Wire.endTransmission(false); 
  Wire.requestFrom(MPL3115A2_ADDRESS, 3); 

  int counter = 0;
  while(Wire.available() < 3)
  {
    if(counter++ > 100) return 0; 
    delay(1);
  }

  byte msb, csb, lsb;
  msb = Wire.read();
  csb = Wire.read();
  lsb = Wire.read();

  float tempcsb = (lsb>>4)/16.0;

  float altitude = (float)( (msb << 8) | csb) + tempcsb;

  return(altitude);
}

float readAltitudeFt()
{
  return(readAltitude() * 3.28084);
}

float readPressure()
{
 
  while( (IIC_Read(STATUS) & (1<<2)) == 0) break;

  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(OUT_P_MSB);  
  Wire.endTransmission(false); 
  Wire.requestFrom(MPL3115A2_ADDRESS, 3); 

  //Wait for data to become available
  int counter = 0;
  while(Wire.available() < 3)
  {
    if(counter++ > 100) return 0; 
    delay(1);
  }

  byte msb, csb, lsb;
  msb = Wire.read();
  csb = Wire.read();
  lsb = Wire.read();

  long pressure_whole = (long)msb<<16 | (long)csb<<8 | (long)lsb;
  pressure_whole >>= 6; 
  
  lsb &= 0b00110000; 
  lsb >>= 4; 
  float pressure_decimal = (float)lsb/4.0; 
  
  float pressure = (float)pressure_whole + pressure_decimal;
  
  return(pressure);
}

float readTemp()
{

  while( (IIC_Read(STATUS) & (1<<1)) == 0) break; 

  // Read temperature
  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(OUT_T_MSB);  // Address of data to get
  Wire.endTransmission(false); 
  Wire.requestFrom(MPL3115A2_ADDRESS, 2); 


  int counter = 0;
  while(Wire.available() < 2)
  {
    if(counter++ > 100) return 0;
    delay(1);
  }

  byte msb, lsb;
  msb = Wire.read();
  lsb = Wire.read();

  float templsb = (lsb>>4)/16.0; 

  float temperature = (float)(msb + templsb);

  return(temperature);
}

float readTempF()
{
  return((readTemp() * 9.0)/ 5.0 + 32.0); // Convert celsius to fahrenheit
}

//Sets the mode to Barometer

void setModeBarometer()
{
  byte tempSetting = IIC_Read(CTRL_REG1); 
  tempSetting &= ~(1<<7);
  IIC_Write(CTRL_REG1, tempSetting);
}


void setModeAltimeter()
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting |= (1<<7); //Set ALT bit
  IIC_Write(CTRL_REG1, tempSetting);
}

//Puts the sensor in standby mode

void setModeStandby()
{
  byte tempSetting = IIC_Read(CTRL_REG1); 
  tempSetting &= ~(1<<0);
  IIC_Write(CTRL_REG1, tempSetting);
}

//Puts the sensor in active mode
void setModeActive()
{
  byte tempSetting = IIC_Read(CTRL_REG1); 
  tempSetting |= (1<<0); 
  IIC_Write(CTRL_REG1, tempSetting);
}

void setOversampleRate(byte sampleRate)
{
  if(sampleRate > 7) sampleRate = 7; 
  sampleRate <<= 3;
  
  byte tempSetting = IIC_Read(CTRL_REG1);
  tempSetting &= 0b11000111; 
  tempSetting |= sampleRate; 
  IIC_Write(CTRL_REG1, tempSetting);
}


void enableEventFlags()
{
  IIC_Write(PT_DATA_CFG, 0x07); // Enable all three pressure and temp event flags 
}


byte IIC_Read(byte regAddr)
{

  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(regAddr);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPL3115A2_ADDRESS, 1); 
  return Wire.read();
}

void IIC_Write(byte regAddr, byte value)
{

  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(regAddr);
  Wire.write(value);
  Wire.endTransmission(true);
}
