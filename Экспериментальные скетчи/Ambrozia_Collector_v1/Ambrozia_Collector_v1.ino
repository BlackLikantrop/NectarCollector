#define _Debug 0    // режим отладки 1-вкл, 0-выкл

// Подключение библиотек
#include <IRremote.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 

// Режимы работы сборщика Амброзии
#define _OFF 1           // режим "Выключено" 
#define _Open 2          // режим "Клапан открыт"
#define _Closed 3        // режим "Клапан закрыт"

// Настройки
#define IR_Pin A3                           // Пин инфракрасного датчика
#define Relay_Pin 8                        // Пин подключения релейного модуля
#define CycleDuration 1000                  // Длительность цикла в миллисекундах

// Переменные
byte Mode = _OFF;                 // Текущий режим работы
byte TotalCycles = 10;            // Текущее общее количество циклов
byte OpenCycles = 10;              // Текущее количество открытых циклов
byte CurrentMenuLine = 0;         // Текущая строка меню

unsigned long LastOpenTime;   // Время последнего перехода в режим "Клапан открыт"
unsigned long LastCloseTime;  // Время последнего перехода в режим "Клапан открыт"
unsigned long CurrentMillis;  // Служебная переменная учета времени

decode_results results;
LiquidCrystal_I2C lcd(0x27,16,2);
IRrecv irrecv(IR_Pin); 

void setup() {
  irrecv.enableIRIn();  // запускаем прием инфракрасного сигнала
  pinMode(IR_Pin, INPUT); // пин A3 будет входом (англ. «input»)
  pinMode(Relay_Pin, OUTPUT); 
  
  lcd.init();                       // Инициализация дисплея  
  lcd.noBacklight();                // Отключение подсветки
  lcd.setCursor(1,0);               // Установка курсора в начало первой строки
  lcd.print("TotalCycles");         // Установка статичного текста первой строки
  lcd.setCursor(1,1);               // Установка курсора в начало второй строки
  lcd.print("OpenCycles");          // Установка статичного текста второй строки

  if (_Debug)
  {
   Serial.begin (9600); // открываем порт для консоли
   Serial.println("START");
  }
  
}

void loop() {
  lcd.setCursor(13,0);               
  lcd.print(TotalCycles);
  lcd.setCursor(13,1);               
  lcd.print(OpenCycles); 
    
  if (CurrentMenuLine)
  {
    lcd.setCursor(0,0);               
    lcd.print(" ");
    lcd.setCursor(0,1);               
    lcd.print("*"); 
  }
  else
  {
    lcd.setCursor(0,1);               
    lcd.print(" ");
    lcd.setCursor(0,0);               
    lcd.print("*");    
  }
  if (irrecv.decode(&results)) // если данные пришли выполняем команды
  {
    switch (results.value) {
      case 16763190:
        if (_Debug)
        {
          Serial.println(" Power      ");
        }
        if (Mode == _OFF)
        {
          ToOpenMode(); 
        }
        else
        {
          ToOFFMode();
        }
       break;
      case 16755030:
        if (_Debug)
        {
          Serial.println(" UP       ");
        }
        if (CurrentMenuLine > 0)
        {
          CurrentMenuLine--;
        }
       break;
      case 16734630:
        if (_Debug)
        {
          Serial.println(" DOWN          ");
        }
        if (CurrentMenuLine < 1)
        {
          CurrentMenuLine++;
        }
       break;
      case 16750950:
        if (_Debug)
        {
          Serial.println(" Right       ");
        }
        switch (CurrentMenuLine) 
        {
         case 0:
           if (TotalCycles<250)
           {
            lcd.setCursor(13,0);               
            lcd.print("   ");
            TotalCycles++; 
           }
          break;
         case 1:
           if (OpenCycles<TotalCycles)
           {
            lcd.setCursor(13,1);               
            lcd.print("   ");
            OpenCycles++; 
           }
          break;
        }
       break;
      case 16767270:
        if (_Debug)
        {
          Serial.println(" Left   ");
        }
        switch (CurrentMenuLine) 
        {
         case 0:
           if (TotalCycles>1)
           {
            lcd.setCursor(13,0);               
            lcd.print("   ");            
            TotalCycles--;
            if (OpenCycles>TotalCycles)
            {
              lcd.setCursor(13,1);               
              lcd.print("   ");              
              OpenCycles=TotalCycles; 
            }
           }
          break;
         case 1:
           if (OpenCycles>0)
           {
            lcd.setCursor(13,1);               
            lcd.print("   ");
            OpenCycles--; 
           }
          break;
        }
       break;
    }
    irrecv.resume(); // принимаем следующий сигнал на ИК приемнике
  }
  
  switch (Mode) 
  {
   case _Open:
    if ((millis() - LastOpenTime) > CycleDuration*OpenCycles)
    {
      if ((TotalCycles-OpenCycles)>0)
      {
        ToClosedMode();
      }
    }
    break;
   case _Closed:
    if ((millis() - LastCloseTime) > CycleDuration*(TotalCycles-OpenCycles))
    {
      if (OpenCycles>0)
      {
        ToOpenMode();
      }
    }
    break;
  }
}

void ToOpenMode ()
{
  Mode = _Open;
  LastOpenTime = millis();
  lcd.backlight();
  digitalWrite (Relay_Pin, HIGH);
}

void ToOFFMode ()
{
  Mode = _OFF;
  lcd.noBacklight();
  digitalWrite (Relay_Pin, LOW);
}

void ToClosedMode ()
{
  Mode = _Closed;
  LastCloseTime = millis();
  digitalWrite (Relay_Pin, LOW);
}
