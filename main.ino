// Библиотеки
#include <ACE128.h> 
#include <ACE128map12345678.h> 
#include <Wire.h> 
#include <Servo.h> //Include Servo.h

// 747 - 748 - центр шутрвала, влево кручу - 261, вправо - 192 - 191// влево минус 1 когда кручу
// до упора вперед - 33/ до упора назад - 102/ центр - 2(с назада до впереда + 1 с 102 до центра, потом до 33
// 5 4 3 2 1 0 127 нейтраль
// 6 7 8 9 10 11.  128 127 126 125 124 123 до малого 
// 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 до большого 122 121 120 119 118 117 116 115 114 113 112 111 110 109 108 107
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

Servo m1; //Create a virtual servo named SimonKESC
 //Specify potentiometer pin A2 on Arduino
int old_old_pos_s_for_force_feed = 0;
void setup()
{
    m1.attach(14); //Specify here the pin number on which the signal pin of ESC is connected
     //delay(1); // no use of this line -_-
   m1.write(30);// this arms the HW esc
    delay(3000);// this delay is a must.

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
int j = 0;
void loop() { 
 // от 47 до 65
      
  m1.write(j);
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
  
  old_old_pos_s_for_force_feed = pos_s;
      positionG.f = upos - 35;
      positionS.f = pos_s/20 - 25; 

      //char yacht_dir = Serial.read();
      pos_s =  pos_s / 25;
      
      // чекаем позицию энкодера ручки и отправляем
      if (upos-oldPosP == 1){
        if ((upos == 127) || ((upos >= 0) && (upos <= 5))){
        }else if (((upos >= 6) && (upos <= 11)) || ((upos <= 126) && (upos >= 121))){
          Serial.print('3');
        }else if (((upos >= 12) && (upos <= 27)) || ((upos <= 120) && (upos >= 105))){
          Serial.print('3');
          Serial.print('3');
        }
        //j = 65;
   
      }else if (upos-oldPosP == -1){
        
        //j = 50;
        if ((upos == 127) || ((upos >= 0) && (upos <= 5))){
        }else if (((upos >= 6) && (upos <= 11)) || ((upos <= 126) && (upos >= 121))){
          Serial.print('1');
        }else if (((upos >= 12) && (upos <= 27)) || ((upos <= 120) && (upos >= 105))){
          Serial.print('1');
          Serial.print('1');
        }
  
      }else if (upos-oldPosP == 127){
        if ((upos == 127) || ((upos >= 0) && (upos <= 5))){
        }else if (((upos >= 6) && (upos <= 11)) || ((upos <= 126) && (upos >= 121))){
          Serial.print('3');
        }else if (((upos >= 12) && (upos <= 27)) || ((upos <= 120) && (upos >= 105))){
          Serial.print('3');
          Serial.print('3');
        }
        //j = 65; 
      }else if (upos-oldPosP == -127){
        //j = 50;
        if ((upos == 127) || ((upos >= 0) && (upos <= 5))){
        }else if (((upos >= 6) && (upos <= 11)) || ((upos <= 126) && (upos >= 121))){
          Serial.print('1');
        }else if (((upos >= 12) && (upos <= 27)) || ((upos <= 120) && (upos >= 105))){
          Serial.print('1');
          Serial.print('1');
        }
      }else if (millis()-timerR>=200){
        Serial.print('2');
        //j = 47;
        timerR = millis();
      }
      oldPosP = upos;  


      
      
      // добавление ветвления (изменение j)
      // чекаем позицию энкодера штурвала и отправляем
      if (pos_s-pos_s_old == 1){
        //Serial.println("left"); 
        Serial.write(62);
        if ((old_old_pos_s_for_force_feed >= 505) && (old_old_pos_s_for_force_feed <= 991) && (upos >= 6) && (upos <= 126)){
          j = 50; 
        }else if  ((upos >= 6) && (upos <= 126)){
          j = 65; 
        }else{
          j = 0;    
        }
      }else if (pos_s-pos_s_old == -1){
        //Serial.println("right"); 
        Serial.write(60);
        if ((old_old_pos_s_for_force_feed >= 505) && (old_old_pos_s_for_force_feed <= 991) && (upos >= 6) && (upos <= 126)){
          j = 50; 
        }else if  ((upos >= 6) && (upos <= 126)){
          j = 65; 
        }else{
          j = 0;    
        }
      }else if ((pos_s-pos_s_old == 0) && (millis() - timerR1 >= 200)){
        //Serial.println("stop"); 
        Serial.write(61);
        timerR1 = millis();
        //if ((old_old_pos_s_for_force_feed >= 505) && (old_old_pos_s_for_force_feed <= 991)){
        // j = 60; 
        //}
        j = 0;
       
      }
      pos_s_old = pos_s;                 
}
