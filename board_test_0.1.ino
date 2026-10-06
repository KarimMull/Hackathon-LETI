#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>
#include <EncButton.h>
#include <Adafruit_NeoPixel.h>

#include <Wire.h>

#include <GyverMAX7219.h>

#include "timer.h"

// ================= Конфигурация платы =================
// Мультиплексор RS2252XS16
#define MUX_A 6
#define MUX_B 7
#define MUX_X 4
#define MUX_Y 5

// Встроенные кнопки, LEDs, UART
#define BOARD_BTN_LEFT_PIN 9
#define BOARD_BTN_RIGHT_PIN 10
#define LED_PIN 48
#define UART_RX 44

#define BOARD_BNT_LEFT_NUM 0
#define BOARD_BNT_RIGHT_NUM 1

#define NUM_LEDS 8
#define BRIGHTNESS 25  // Set BRIGHTNESS to about 1/5 (max = 255)

// TFT дисплей
#define TFT_CS 14
#define TFT_RST 37
#define TFT_DC 36
#define TFT_BL 35

// Цвета
#define COLOR_BTN_INACTIVE 0xF206
#define COLOR_BTN_ACTIVE 0x4D6A

// ================= Конфигурация модулей =================

// Модуль 1: Кнопки
#define MODULE_BTN_UP_PIN 17
#define MODULE_BTN_LEFT_PIN 18
#define MODULE_BTN_RIGHT_PIN MUX_X // 
#define MODULE_BTN_DOWN_PIN MUX_Y

#define MODULE_BTN_NUM 1
#define MODULE_BTN_TIMER_PERIOD 50

#define BTN_UP_NUM 0
#define BTN_LEFT_NUM 1
#define BTN_RIGHT_NUM 2
#define BTN_DOWN_NUM 3

// Модуль 2: Базер
#define BUZZER_PIN 1

// Модуль 3: Джойстик
#define MODULE_JOY_BTN 15
#define MODULE_JOY_X MUX_X
#define MODULE_JOY_Y MUX_Y

#define MODULE_JOY_NUM 3
#define MODULE_JOY_TIMER_PERIOD 50

// Модуль 4: Матрица
#define MATRIX_CS 1
#define SPI_DATA 42
#define SPI_CLK 38
#define MODULE_MATRIX_TIMER_PERIOD 50

// Курица
#define CHICKEN_TIMER_PERIOD 20
#define CHICKEN_HITBOX_RADIUS 12

// ================= Переменные =================


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// --------------------------------------------------------------
// Button board_btns[2];           // Объекты кнопок приставки (боковых, мы не используем)
// --------------------------------------------------------------

// Модуль кнопок
Button mudule_btns[4];          // Объекты кнопок модуля
struct timer btn_timer;

// Баззер
Beeper buz(BUZZER_PIN);

int currentNote = 0;
struct timer buzzer_timer;

// Джойстик
int joyX = 0;
int joyY = 0;
int joy_dotX = 240;
int joy_dotY = 60;
bool joyBtn_state = false;
struct timer joy_timer;

// Матрица <матриц по вертикали, матриц по горизонтали, CS-пин, DATA-пин, SPI-пин> Объект - mtrx
MAX7219<1, 1, MATRIX_CS, SPI_DATA, SPI_CLK > mtrx;  // подключение к любым пинам (софт SPI)
struct timer matrix_timer;

// Курица
float chickenX = 160, chickenY = 120;       // текущая позиция
float targetX = 160, targetY = 120; // куда ехать
int prevX, prevY;
const float SPEED = 0.05;           // доля пути за кадр
struct timer chicken_timer


// ================= Прототипы функций =================
void selectModule(uint8_t mod);
void drawStaticUI();

void btn_init(void);
void buzzer_init(void);
void joy_init(void);


void matrix_init(void);
void func_init(void);
void chicken_init(void);

void btn_mng(void);
void buzzer_mng(void);
void joy_mng(void);

void matrix_mng(void);
void func_mng(void);
void chicken_mng(void);


// ================= Функции =================

void setup() {

  func_init();
  btn_init();
  buzzer_init();
  joy_init();
  matrix_init();
  chicken_init();

  delay(500);
}

void loop() {

  func_mng();

  btn_mng();
  buzzer_mng();
  joy_mng();
  matrix_mng();
  chicken_mng();
}

// ================= НАСТРОЙКА МУЛЬТИПЛЕКСОРА =================
void selectModule(uint8_t mod) {
  // mod 1: A=0, B=0
  // mod 2: A=1, B=0
  // mod 3: A=0, B=1
  // mod 4: A=1, B=1
  digitalWrite(MUX_A, (mod == 2 || mod == 4) ? HIGH : LOW); 
  digitalWrite(MUX_B, (mod == 3 || mod == 4) ? HIGH : LOW);
  delayMicroseconds(10);  // Время на переключение мультиплексора
}

// ================= ОТРИСОВКА ИНТЕРФЕЙСА =================
void drawStaticUI() {

  tft.fillScreen(0x0);
  // line 1
  tft.drawLine(0, 119, 319, 119, 0xFFFF);
  // line 2
  tft.drawLine(159, 0, 159, 239, 0xFFFF);
  // circle 3
  tft.fillCircle(75, 30, 10, 0xF206);
  // circle 3
  tft.fillCircle(50, 55, 10, 0xF206);
  // circle 3
  tft.fillCircle(100, 55, 10, 0xF206);
  // circle 3
  tft.fillCircle(75, 80, 10, 0xF206);
  // circle 7
  tft.drawCircle(240, 60, 40, 0xFFFF);
  // circle 8
  tft.fillCircle(240, 60, 4, 0xFF47);
  // rect 9
  tft.fillRect(0, 0, 40, 10, 0xF206);
  // rect 9
  tft.fillRect(280, 0, 40, 10, 0xF206);
}

void func_init(void) {

  // Инициализация мультиплексора
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  selectModule(MODULE_BTN_NUM);  // По умолчанию выбираем 1 модуль

  // --------------------------------------------------------------
  // Инициализация кнопок платы
  // board_btns[BOARD_BNT_LEFT_NUM].init(BOARD_BTN_LEFT_PIN);
  // board_btns[BOARD_BNT_RIGHT_NUM].init(BOARD_BTN_RIGHT_PIN);
  // --------------------------------------------------------------

  // Инициализация TFT
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  tft.init(240, 320);  // Init ST7789 320x240
  tft.setRotation(1);  // Горизонтальная ориентация (240x320)
  drawStaticUI();

}

void func_mng(void) {

// --------------------------------------------------------------
  // Опрос кнопок платы (НЕ ИСПОЛЬЗУЕМ)
  // for (int i = 0; i < 2; i++) board_btns[i].tick();

  // // Действия по кнопкам
  // if (board_btns[BOARD_BNT_LEFT_NUM].pressing()) {
  //   tft.fillRect(0, 0, 40, 10, COLOR_BTN_ACTIVE);
  // } else {
  //   tft.fillRect(0, 0, 40, 10, COLOR_BTN_INACTIVE);
  // }

  // if (board_btns[BOARD_BNT_RIGHT_NUM].pressing()) {
  //   tft.fillRect(280, 0, 40, 10, COLOR_BTN_ACTIVE);
  // } else {
  //   tft.fillRect(280, 0, 40, 10, COLOR_BTN_INACTIVE);
  // }
// --------------------------------------------------------------

  if (mudule_btns[BTN_UP_NUM].pressing()) {               // .pressing() проверяет значение флага объекта module_btns (нажат/не нажат)
    tft.fillCircle(75, 30, 10, COLOR_BTN_ACTIVE);
  } else {
    tft.fillCircle(75, 30, 10, COLOR_BTN_INACTIVE);
  }

  if (mudule_btns[BTN_LEFT_NUM].pressing()) {
    tft.fillCircle(50, 55, 10, COLOR_BTN_ACTIVE);
  } else {
    tft.fillCircle(50, 55, 10, COLOR_BTN_INACTIVE);
  }

  if (mudule_btns[BTN_RIGHT_NUM].pressing()) {
    tft.fillCircle(100, 55, 10, COLOR_BTN_ACTIVE);
  } else {
    tft.fillCircle(100, 55, 10, COLOR_BTN_INACTIVE);
  }

  if (mudule_btns[BTN_DOWN_NUM].pressing()) {
    tft.fillCircle(75, 80, 10, COLOR_BTN_ACTIVE);
  } else {
    tft.fillCircle(75, 80, 10, COLOR_BTN_INACTIVE);
  }
}

// --- Инициализация модуля кнопок ---
void btn_init(void) {
  mudule_btns[BTN_UP_NUM].init(MODULE_BTN_UP_PIN);        // Привязка кнопок к цифровым пинам МК
  mudule_btns[BTN_LEFT_NUM].init(MODULE_BTN_LEFT_PIN);
  mudule_btns[BTN_RIGHT_NUM].init(MODULE_BTN_RIGHT_PIN);
  mudule_btns[BTN_DOWN_NUM].init(MODULE_BTN_DOWN_PIN);

  timer_set(&btn_timer, MODULE_BTN_TIMER_PERIOD);         // Инициализация таймера опроса кнопок с периодом 50 мс
}

// --- Обработка модуля кнопок ---
void btn_mng(void) {
  if (timer_expired(&btn_timer)) {                        // Условие задержки таймером btn_timer опроса
    timer_restart(&btn_timer);

    // Перевод выводов в режим цифровых входов
    pinMode(MODULE_BTN_RIGHT_PIN, INPUT);                 
    pinMode(MODULE_BTN_DOWN_PIN, INPUT);
    // Выбор модуля на мультиплексоре
    selectModule(MODULE_BTN_NUM);                       
    // Опрос кнопок                                        // .tick() функция устанавливает флаг объекта module_btns (нажата/не нажата)
    for (int i = 0; i < 4; i++) mudule_btns[i].tick();
  }
}

void buzzer_init(void) {

  // пикнуть нотой NOTE_G1 3 раза, 500мс вкл, 300мс выкл'
  buz.useTone(true);
  buz.invert(true);

  timer_set(&buzzer_timer, 50);
}

void buzzer_mng(void) {

  buz.tick();

  if (timer_expired(&buzzer_timer)) {
    timer_restart(&buzzer_timer);

    if (currentNote > 0) {
      buz.beepNote(currentNote);
      // if (!buz.running()) {
      //   buz.beepNote(currentNote, 1, 50, 0);
      // }
    } else {
      buz.stop();
    }

    static int lastNote = currentNote;
    tft.setTextSize(4);
    tft.setCursor(220, 165);

    if (currentNote > 0) {
      lastNote = currentNote;
      tft.setTextColor(COLOR_BTN_ACTIVE, ST77XX_BLACK);

      switch (currentNote) {
        case NOTE_C4:
          tft.printf("C4");
          break;
        case NOTE_D4:
          tft.printf("D4");
          break;
        case NOTE_E4:
          tft.printf("E4");
          break;
        case NOTE_F4:
          tft.printf("F4");
          break;
        case NOTE_G4:
          tft.printf("G4");
          break;
        case NOTE_A4:
          tft.printf("A4");
          break;
        default:
          break;
      }
    } else {
      if (lastNote > 0) {
        tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
        switch (lastNote) {
          case NOTE_C4:
            tft.printf("C4");
            break;
          case NOTE_D4:
            tft.printf("D4");
            break;
          case NOTE_E4:
            tft.printf("E4");
            break;
          case NOTE_F4:
            tft.printf("F4");
            break;
          case NOTE_G4:
            tft.printf("G4");
            break;
          case NOTE_A4:
            tft.printf("A4");
            break;
          default:
            break;
        }
        lastNote = 0;
      }
    }
  }
}

void joy_init(void) {
  pinMode(MODULE_JOY_BTN, INPUT);

  timer_set(&joy_timer, MODULE_JOY_TIMER_PERIOD);
}

void joy_mng(void) {
  if (timer_expired(&joy_timer)) {
    timer_restart(&joy_timer);

    // --- ЧТЕНИЕ МОДУЛЯ 3 (ДЖОЙСТИК) ---
    pinMode(MODULE_JOY_X, ANALOG);
    pinMode(MODULE_JOY_Y, ANALOG);
    selectModule(3);  // A=0, B=1
    joyX = analogRead(MODULE_JOY_X);
    joyY = analogRead(MODULE_JOY_Y);
    joyBtn_state = !digitalRead(MODULE_JOY_BTN);

    int x_start = 240;
    int y_start = 60;

    // Стираем старую точку
    tft.fillCircle(joy_dotX, joy_dotY, 4, 0x0);

    // Позиция новой точки
    // ADC 12-bit (0-4095). Центр примерно 2048.
    joy_dotX = x_start + map(joyX, 0, 4095, -25, 25);
    joy_dotY = y_start - map(joyY, 0, 4095, -25, 25);

    // Рисуем новую точку
    tft.fillCircle(joy_dotX, joy_dotY, 4, joyBtn_state ? ST77XX_GREEN : ST77XX_YELLOW);
  }
}

void matrix_init(void) {
  mtrx.begin();
  mtrx.setBright(1);

  // for (int j = 0; j < 8; j++) {                           //  Анимация змейкой, НЕ НУЖНО
  //   for (int i = 0; i < 8; i++) {
  //     mtrx.dot(i, j);  // пиксель на координатах i,j
  //     mtrx.update();   // показать
  //     delay(25);
  //   }
  //   delay(25);
  // }

  mtrx.clear();                                               // Очистка буфера (физически матрица горит)
  mtrx.line(0, 0, 7, 7);                                      // Рисование диагонали покоординатам (.line() добавляет линию в буфер)
  mtrx.line(7, 0, 0, 7);
  mtrx.update();

  timer_set(&matrix_timer, MODULE_MATRIX_TIMER_PERIOD);
}

void matrix_mng(void) {
  if (timer_expired(&matrix_timer)) {
    
    timer_restart(&matrix_timer);

    // Стираем старую точку
    mtrx.clear();

    int matrix_dotX = 3 - map(ax, -16384, 16384, -3, 3);
    int matrix_dotY = 3 + map(ay, -16384, 16384, -3, 3);

    // Рисуем новую точку
    mtrx.dot(matrix_dotX, matrix_dotY);
    mtrx.dot(matrix_dotX + 1, matrix_dotY);
    mtrx.dot(matrix_dotX, matrix_dotY + 1);
    mtrx.dot(matrix_dotX + 1, matrix_dotY + 1);
    mtrx.update();
  }
}

bool isHit(int chickenX, int chickenY, int joyX, int joyY) {
  int dx = chickenX - joyX;
  int dy = chickenY - joyY;
  int distSq = dx * dx + dy * dy;
  return distSq <= CHICKEN_HITBOX_RADIUS * CHICKEN_HITBOX_RADIUS;
}

void chicken_init(void) {

  randomSeed(esp_random());
  targetX = random(10, 310);
  targetY = random(10, 230);
  prevX = (int)chickenX;
  prevY = (int)chickenY;
  tft.fillCircle(prevX, prevY, 4, ST77XX_RED);
  timer_set(&chicken_timer, CHICKEN_TIMER_PERIOD);   // 50 FPS

}

void chicken_mng(void) {

  if (timer_expired(&chicken_timer)) {
    timer_restart(&chicken_timer);

    // Сравнение центров прицела и хитбокса курицы
    if (abs(chickenX - targetX) < 2 && abs(chickenY - targetY) < 2) {
      targetX = random(10, 310);
      targetY = random(10, 230);
    }

    // Перемещение курицы
    chickenX += (targetX - chickenX) * SPEED;
    chickenY += (targetY - chickenY) * SPEED;

    int newX = (int)chickenX;
    int newY = (int)chickenY;

    // Рисование курицы
    if (newX != prevX || newY != prevY) {
      tft.fillCircle(prevX, prevY, 4, ST77XX_BLACK);
      tft.fillCircle(newX,  newY,  4, ST77XX_RED);
      prevX = newX;
      prevY = newY;
    }

    // Условие попадания по курице
    if (isHit(chickenX, chickenY, joyX, joyY)) {
      // Звук на buzzer
    }
  }
}