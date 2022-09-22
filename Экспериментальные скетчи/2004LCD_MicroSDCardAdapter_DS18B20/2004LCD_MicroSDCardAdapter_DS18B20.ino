#include <microDS18B20.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <SPI.h>
#include <SD.h>

MicroDS18B20<2> sensor1;
MicroDS18B20<3> sensor2;
MicroDS18B20<4> sensor3;
LiquidCrystal_I2C lcd(0x3f,20,4);
float Temp1=0;
float Temp2=0;
float Temp3=0;

String CurrentLogName;
unsigned long TimeStamp=0;

void setup() {
  Serial.begin(9600);
  sensor1.setResolution(12);
  sensor2.setResolution(12);
  sensor3.setResolution(12);
  lcd.init();                            // Инициализация lcd             
  lcd.backlight();                       // Включаем подсветку

  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  else {
    Serial.println("initialization done.");
  }

  int CurrentLogNumber=0;

  do
  {  
    CurrentLogNumber++;
    CurrentLogName = "LOG"+String(CurrentLogNumber)+".CSV";
    Serial.println("Searching "+CurrentLogName);
  }  
  while (SD.exists(CurrentLogName));
  Serial.println("Finally: "+CurrentLogName); 

  File CurrentLogFile = SD.open(CurrentLogName, FILE_WRITE);
    if (CurrentLogFile) {
    CurrentLogFile.println("TIME;T_cube;T_tower;T_node");
    CurrentLogFile.close();
  } else {
    Serial.println(CurrentLogName);
    Serial.println("Couldn't open log file");
  }
}
void loop() {

  // запрос температуры  
  sensor1.requestTemp();
  sensor2.requestTemp();
  sensor3.requestTemp();

  lcd.setCursor(0, 0);
  if (sensor1.readTemp())
    {
     lcd.print("t cube:   ");
     Temp1=sensor1.getTemp();
     lcd.print(Temp1);
    } 
  else {
    lcd.print("t cube:   ERROR");
    Temp1=0;
  }


  lcd.setCursor(0, 1);
  if (sensor2.readTemp())
    {
     lcd.print("t tower:  ");
     Temp2=sensor2.getTemp();
     lcd.print(Temp2);
    } 
  else {
    lcd.print("t tower:  ERROR");
    Temp2=0;
  }
  
  lcd.setCursor(0, 2);  
  if (sensor3.readTemp())
    {
     lcd.print("t node:   ");
     Temp3=sensor3.getTemp();
     lcd.print(Temp3);
    } 
  else {
    lcd.print("t node:   ERROR");
    Temp3=0;
  }

  lcd.setCursor(0, 3);
  File CurrentLogFile = SD.open(CurrentLogName, FILE_WRITE);
  if (CurrentLogFile) {
    CurrentLogFile.println(String(TimeStamp)+";"+String(Temp1)+";"+String(Temp2)+";"+String(Temp3));
    CurrentLogFile.close();
  lcd.print("SD Write: "+CurrentLogName);    
  } else 
  {
    lcd.print("SD Write: ERROR");
  }

  delay(5000);
  TimeStamp=TimeStamp+5;
}
