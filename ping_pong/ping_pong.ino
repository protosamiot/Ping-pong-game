/*
  Project: ESP32 Ping Pong Game
  Created by: ProtoSam IoT
  GitHub: https://github.com/ProtoSamIoT
  YouTube: https://www.youtube.com/@protosamiot

  If you find this project useful, consider starring the repo ⭐
  and subscribing to the YouTube channel for more ESP32 & IoT projects.
*/
#include <Arduino.h>
#include <U8g2lib.h>

/* ---------- OLED ---------- */
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

/* ---------- JOYSTICK ---------- */
#define JOY_X 34
#define JOY_Y 35
#define JOY_SW 32

/* ---------- BUZZER ---------- */
#define BUZZER 25

/* ---------- GAME STATE ---------- */
int gameState = 0; // 0 = menu/start, 1 = playing

/* ---------- PONG VARIABLES ---------- */
int paddleY = 24;       // Paddle vertical position
int paddleHeight = 16;  // Paddle height
int ballX = 64;         // Ball starting position
int ballY = 32;
int ballDirX = 1;       // Ball horizontal direction
int ballDirY = 1;       // Ball vertical direction
int ballSpeed = 2;      // Ball speed
int pongScore = 0;
bool pongGameOver = false;

/* ---------- SETUP ---------- */
void setup() {
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  oled.begin();
  oled.setFont(u8g2_font_6x10_tf);
}

/* ---------- LOOP ---------- */
void loop() {
  if (gameState == 0) {
    drawStart();
    int joyY = analogRead(JOY_Y);
    bool btn = digitalRead(JOY_SW) == LOW;
    if (btn || joyY < 1200) {
      gameState = 1; // Start game on joystick up or press
      resetPong();
    }
  }
  else if (gameState == 1) {
    gamePingPong();
  }
}

/* ---------- PONG FUNCTIONS ---------- */
void gamePingPong() {
  int joyY = analogRead(JOY_Y);

  // Paddle movement with joystick
  if (joyY < 1200) paddleY -= 3; // up
  if (joyY > 3000) paddleY += 3; // down
  if (paddleY < 0) paddleY = 0;
  if (paddleY > 64 - paddleHeight) paddleY = 64 - paddleHeight;

  // Move ball
  ballX += ballDirX * ballSpeed;
  ballY += ballDirY * ballSpeed;

  // Ball collision with top/bottom
  if (ballY <= 0 || ballY >= 64) ballDirY = -ballDirY;

  // Ball collision with paddle
  if (ballX <= 10) { // paddle X = 0..10
    if (ballY >= paddleY && ballY <= paddleY + paddleHeight) {
      ballDirX = -ballDirX; // bounce
      pongScore++;
      tone(BUZZER, 800, 50); // hit sound
    } else {
      // Missed the ball
      pongGameOver = true;
    }
  }

  // Ball collision with right wall
  if (ballX >= 128) ballDirX = -ballDirX;

  // Draw everything
  oled.clearBuffer();
  // Paddle
  oled.drawBox(0, paddleY, 10, paddleHeight);
  // Ball
  oled.drawBox(ballX, ballY, 4, 4);
  // Score
  oled.setCursor(100, 10);
  oled.print("S:");
  oled.print(pongScore);

  oled.sendBuffer();

  // Check game over
  if (pongGameOver) {
    oled.clearBuffer();
    oled.drawStr(30, 30, "GAME OVER");
    oled.setCursor(40, 45);
    oled.print("Score:");
    oled.print(pongScore);
    oled.sendBuffer();
    delay(2000);

    resetPong();
    gameState = 0; // Back to start/menu
  }

  delay(30); // frame control
}

void resetPong() {
  paddleY = 24;
  ballX = 64;
  ballY = 32;
  ballDirX = 1;
  ballDirY = 1;
  pongScore = 0;
  pongGameOver = false;
}

void drawStart() {
  oled.clearBuffer();
  oled.drawStr(20, 20, "Single Player Pong");
  oled.drawStr(10, 40, "Push Joystick Up to Start");
  oled.sendBuffer();
}