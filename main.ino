// Библиотеки
#include <ACE128.h> 
#include <ACE128map12345678.h> 
#include <Wire.h> 

// Создание объекта для работы с энкодером ace128
ACE128 myACE(2,3,4,5,6,7,8,9, (uint8_t*)encoderMap_12345678);

// Константы для управления ручки
int oldPosP = 0;

// Константы для энкодера ace128
uint8_t upos = 0;
uint8_t oldPos = 255;
uint8_t pinPos = 0;

// Пины энкодера ems22a
const int PIN_CS = 10;
const int PIN_CLOCK = 13;
const int PIN_DATA = 11;

// Константы для вывода в Serial
int border = 40;

unsigned long long int timerR = 0;
unsigned long long int timerR1 = 0;

// Входы для драйвера
// Выводы управления 1-м мотором
#define INA 16
#define INB 15
#define ENA 14

// Запрос от компьютера, содержащий информацию о движении яхты 
int inChar;

// Представление угла поворота штурвала в байтах
union {
  float f;
  byte b[4];
} positionS;

// Представление угла поворота ручки в байтах
union {
  float f;
  byte b[4];
} positionG;

int pos_s_old = 0;

void setup() {
      // Подключаем сериал
      Serial.begin(9600);

      // Настраиваем энкодер ace128
      myACE.begin();    
      pinPos = myACE.acePins();          
                     
      
      // Настраиваем энкодер ems22a
      pinMode(PIN_CS, OUTPUT);
      pinMode(PIN_CLOCK, OUTPUT);
      pinMode(PIN_DATA, INPUT);
      digitalWrite(PIN_CLOCK, HIGH);
      digitalWrite(PIN_CS, LOW);

      // Настройка мотора
      pinMode(ENA, OUTPUT);
      pinMode(INA, OUTPUT);
      pinMode(INB, OUTPUT);

      oldPosP = myACE.rawPos(); 
}

void loop() { 
      // Получение значений с энкодера ace128               
      upos = myACE.rawPos(); 

      // Получение значений с энкодера ems22a   
      digitalWrite(PIN_CS, HIGH);
      digitalWrite(PIN_CS, LOW);
      int pos_s = 0;
      
      for (int i=0; i<10; i++) {
        digitalWrite(PIN_CLOCK, LOW);
        digitalWrite(PIN_CLOCK, HIGH);
        pos_s= pos_s | digitalRead(PIN_DATA);
        if(i<9) pos_s = pos_s << 1;
      }
      
      for (int i=0; i<6; i++) {
        digitalWrite(PIN_CLOCK, LOW);
        digitalWrite(PIN_CLOCK, HIGH);
      }
      digitalWrite(PIN_CLOCK, LOW);
      digitalWrite(PIN_CLOCK, HIGH);
  
      positionG.f = upos - 35;
      positionS.f = pos_s/20 - 25; 

      // чекаем позицию энкодера ручки и отправляем
      if (upos-oldPosP == 1){
        Serial.print('3');
      }else if (upos-oldPosP == -1){
        Serial.print('1');
      }else if (upos-oldPosP == 127){
        Serial.print('3');
      }else if (upos-oldPosP == -127){
        
        Serial.print('1');
      }else if (millis()-timerR>=200){
        Serial.print('2');
        timerR = millis();
      }
      oldPosP = upos;  


      
      
      // чекаем позицию энкодера штурвала и отправляем
      if (pos_s-pos_s_old == 1){
        Serial.print('3');
      }else if (pos_s-pos_s_old == -1){
        Serial.print('1');
      }else if (pos_s-pos_s_old == 1023){
        Serial.print('3');
      }else if (pos_s-pos_s_old == -1023){
        
        Serial.print('1');
      }else if (millis()-timerR1>=200){
        Serial.print('2');
        timerR1 = millis();
      }
      pos_s_old = pos_s;  

      
            
}
