int8_t state = 0;          //state the game is in
#define COLUMNS  14        //Columns of bricks
#define ROWS 8             //Rows of bricks
float moveX;                  //Initial movement of ball
int8_t moveY;                 //Initial movement of ball
float ballX;                  //Balls starting possition
int8_t ballY;                 //Balls starting possition
boolean released;          //If buttons has been released by the player
boolean Free;              //If the ball is free
int8_t xPaddle;            //X position of paddle
boolean isHit[ROWS][COLUMNS];//Array of if bricks are hit or not
boolean bounced = false;   //Used to fix double bounce glitch
int8_t lives = 3;          //Amount of lives
int8_t level = 1;          //Current level
int16_t score = 0;         //Score for the game
uint16_t tmpScore = 0;     //template score for HSE
// DO NOT CHANGE THE DATATYPE, it will screw up HSE!
int16_t brickCount;        //Amount of bricks hit
char initials[3];          //Initials used in HSE
bool flash = true;         //flashing of different items

#include <Arduboy.h>
Arduboy arduboy;


void Paddle() {
  //Move right
  if (xPaddle < WIDTH - 32) if (arduboy.pressed(RIGHT_BUTTON)) xPaddle = xPaddle + 2;
  //Move left
  if (xPaddle > 0) if (arduboy.pressed(LEFT_BUTTON)) xPaddle = xPaddle - 2;
  //prevent it from going over to the sides
  if (xPaddle < 1) xPaddle = 0;
  if (xPaddle > WIDTH - 34) xPaddle = WIDTH - 34;
  arduboy.drawRect(xPaddle, 62, 16, 1, 1);
}
void Ball() {
  if (Free) {
    //Move ball
    ballX = ballX + moveX;
    ballY = ballY + moveY;
    //Bounce off top edge
    if (ballY <= 1) {
      ballY = 1;
      moveY = -moveY;
      arduboy.tunes.tone(523, 200);
    }
    //Lose a life if bottom edge hit
    if (ballY >= HEIGHT) {
      arduboy.tunes.tone(175, 200);
      delay(250);
      ballY = 60;
      Free = false;
      lives = lives - 1;
    }
    //Bounce off left side
    if (ballX < 1) {
      ballX = 1;
      moveX = -moveX;
      arduboy.tunes.tone(523, 200);
    }
    //Bounce off right side
    if (ballX > WIDTH - 20) {
      ballX = WIDTH - 20;
      moveX = -moveX;
      arduboy.tunes.tone(523, 200);
    }
    //Bounce off paddle
    if (ballX + 1 >= xPaddle && ballX <= xPaddle + 17 && ballY + 2 >= 63 && ballY <= 64) {
      moveY = -moveY;
      moveX = moveX + (ballX - (xPaddle + 7)) / 4; //Applies spin on the ball
      //limit horizontal speed
      if (moveX < -1.5) moveX = -1.5;
      if (moveX > 1.5)  moveX =  1.5;
      arduboy.tunes.tone(200, 200);
    }
    //Reset Bounce
    bounced = false;
  } else {
    //Ball follows paddle
    ballX = xPaddle + 7;
    ballY = 60;
    //Release ball if FIRE pressed
    if (arduboy.pressed(B_BUTTON) && released) {
      Free = true;
      if (arduboy.pressed(LEFT_BUTTON) || arduboy.pressed(RIGHT_BUTTON)) {
        if (arduboy.pressed(LEFT_BUTTON)) moveX = 0.5;
        else moveX = -0.5;
      } else moveX = random(-1, 1) / 2;
      //Ball heads upwards
      moveY = -1;
    }
  }
  arduboy.drawRect(ballX, floor(ballY), 2, 2, 1);
}
void Brick() {
  //Bounce off Bricks
  for (int8_t row = 0; row < ROWS; row++) {
    for (int8_t column = 0; column < COLUMNS; column++) {
      if (!isHit[row][column]) {
        if ((row / 2) % 2 == 0) arduboy.drawRect(8 * column, 2 + 3 * row, 8, 4, 1);
        else arduboy.fillRect(8 * column, 2 + 3 * row, 8, 4, 1);
        //If A collison has occured
        if (ballY <= 3 * row + 4 && ballY + 2 >= 3 * row + 1 &&
            ballX <= 8 * column + 8 && ballX + 2 >= 8 * column) {
          score = score + 8;
          brickCount++;
          isHit[row][column] = true;
          //Vertical collision
          if (ballY + 2 > 3 * row + 4 || ballY < 3 * row + 1) {
            //Only bounce once each ball move
            if (!bounced) {
              moveY = - moveY;
              bounced = true;
              arduboy.tunes.tone(261, 200);
            }
          }
          //Hoizontal collision
          else if (ballX < 10 * column + 15 || ballX + 2 > 10 * column + 25) {
            //Only bounce once brick each ball move
            if (!bounced) {
              moveX = - moveX;
              bounced = true;
              arduboy.tunes.tone(261, 200);
            }
          }
        }
      }
    }
  }
}
void resetlevel() {
  xPaddle = 50;
  brickCount = 0;
  Free = false;
  ballY = 60;
  for (int8_t row = 0; row < ROWS; row++)
    for (int8_t column = 0; column < COLUMNS; column++)
      isHit[row][column] = false;
}
void resetGame() {
  resetlevel();
  level = 1;
  lives = 3;
  score = 0;
  state = 0;
}
void Statistics() {
  arduboy.setCursor(118, 15);
  arduboy.print(level);
  arduboy.setCursor(118, 35);
  arduboy.print(lives);
  arduboy.setCursor(112, 55);
  arduboy.print(score);
  if ((brickCount == ROWS * COLUMNS) && level < 2) {
    resetlevel();
    level = level + 1;
  }
}
void enterInitials() {
  int8_t index = 0;
  initials[0] = ' ';
  initials[1] = ' ';
  initials[2] = ' ';
  while (index < 3) {
    arduboy.clear();
    arduboy.setCursor(16, 0);
    arduboy.print("HIGH SCORE");
    arduboy.setCursor(88, 0);
    arduboy.print(score);
    arduboy.setCursor(56, 20);
    arduboy.print(initials[0]);
    arduboy.setCursor(64, 20);
    arduboy.print(initials[1]);
    arduboy.setCursor(72, 20);
    arduboy.print(initials[2]);
    for (int8_t i = 0; i < 3; i++) {
      arduboy.drawLine(55 + (i * 8), 27, 56 + (i * 8) + 6, 27, 1);
    }
    arduboy.drawLine(56 + (index * 8), 28, 56 + (index * 8) + 6, 28, 1);
    delay(150);
    if (arduboy.pressed(LEFT_BUTTON) && released) {
      released = false;
      arduboy.tunes.tone(1046, 200);
      index--;
      if (index < 0) index = 0;
    }

    if (arduboy.pressed(RIGHT_BUTTON) && released) {
      released = false;
      index++;
      if (index > 2) index = 2;
      arduboy.tunes.tone(1046, 200);
    }

    if (arduboy.pressed(DOWN_BUTTON)) {
      initials[index]++;
      arduboy.tunes.tone(523, 150);
      // A-Z 0-9 :-? !-/ ' '
      if (initials[index] == '0') initials[index] = ' ';
      if (initials[index] == '!') initials[index] = 'A';
      if (initials[index] == '[') initials[index] = '0';
      if (initials[index] == '@') initials[index] = '!';
    }
    if (arduboy.pressed(UP_BUTTON)) {
      initials[index]--;
      arduboy.tunes.tone(523, 150);
      if (initials[index] == ' ') initials[index] = '?';
      if (initials[index] == '/') initials[index] = 'Z';
      if (initials[index] == 31)  initials[index] = '/';
      if (initials[index] == '@') initials[index] = ' ';
    }
    if (arduboy.pressed(A_BUTTON)) {
      if (index >= 2) {
        index = index + 1;
        arduboy.tunes.tone(1046, 200);
      }
    }
    if (arduboy.notPressed(LEFT_BUTTON) && arduboy.notPressed(RIGHT_BUTTON))
      released = true;
    arduboy.display();
  }
}



void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);
  arduboy.display();
  arduboy.initRandomSeed();
  if (arduboy.audio.enabled()) arduboy.audio.on();
  else arduboy.audio.off();
  arduboy.tunes.tone(987, 160);
  delay(160);
  arduboy.tunes.tone(1318, 400);
}
void loop() {
  if ((!arduboy.nextFrame())) return;
  arduboy.clear();
  switch (state) {
    case 0: {
        arduboy.setCursor(18, 22);
        arduboy.setTextSize(2);
        arduboy.print("BREAKOUT");
        arduboy.setTextSize(1);
        arduboy.setCursor(31, 53);
        if (arduboy.everyXFrames(15)) flash = !flash;
        if (flash)arduboy.print("PRESS FIRE!");
        if (arduboy.pressed(B_BUTTON) && released) {
          released = false;
          state = 1;
          xPaddle = 50;
        }
        if (arduboy.pressed(A_BUTTON) && released) {
          released = false;
          state = 3;
        }
        if ((arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) && released) {
          released = false;
          if (arduboy.audio.enabled()) arduboy.audio.off();
          else arduboy.audio.on();
          arduboy.audio.saveOnOff();
        }
      }
      break;

    case 1: {
        if (lives > 0) {
          Paddle();
          Ball();
          Brick();
          Statistics();
        } else {
          delay(3000);
          resetlevel();
          state = 2;
        }
        if (arduboy.pressed(A_BUTTON) && released) {
          released = false;
          state = 4;
        }
      }
      break;

    case 2: {
        char tmpInitials[3];
        // Each block of EEPROM has 7 high scores, and each high score entry
        // is 5 byte long:  3 char for initials and a short for score.
        // High score processing
        for (int8_t i = 0; i < 7; i++) {
          EEPROM.get(100 + (5 * i), tmpScore);
          if (tmpScore == 0xFFFF) tmpScore = 0;
          if (score > tmpScore) {
            enterInitials();
            for (byte j = i; j < 7; j++) {
              EEPROM.get(100 + (5 * j), tmpScore);
              if (tmpScore == 0xFFFF) tmpScore = 0;

              tmpInitials[0] = (char)EEPROM.read(100 + (5 * j) + 2);
              tmpInitials[1] = (char)EEPROM.read(100 + (5 * j) + 3);
              tmpInitials[2] = (char)EEPROM.read(100 + (5 * j) + 4);

              // write score and initials to current slot
              EEPROM.put(100 + (5 * j), score);
              EEPROM.write(100 + (5 * j) + 2, initials[0]);
              EEPROM.write(100 + (5 * j) + 3, initials[1]);
              EEPROM.write(100 + (5 * j) + 4, initials[2]);

              // tmpScore and tmpInitials now hold what we want to
              //write in the next slot.
              score = tmpScore;
              initials[0] = tmpInitials[0];
              initials[1] = tmpInitials[1];
              initials[2] = tmpInitials[2];
            }
            //reset the initials
            initials[0] = ' ';
            initials[1] = ' ';
            initials[2] = ' ';
            break;
          }
        }
      }
      resetGame();
      break;
    case 3:
      // Each block of EEPROM has 7 high scores, and each high score entry
      // is 5 int8_ts long:  3 int8_ts for initials and one int16_t for score
      arduboy.setCursor(32, 0);
      arduboy.print("HIGH SCORES");

      for (int i = 0; i < 7; i++) {
        EEPROM.get(100 + (5 * i), score);
        if (score == 0xFFFF) score = 0;
        initials[0] = (char)EEPROM.read(100 + (5 * i) + 2);
        initials[1] = (char)EEPROM.read(100 + (5 * i) + 3);
        initials[2] = (char)EEPROM.read(100 + (5 * i) + 4);

        arduboy.setCursor(30, 9 + (i * 8));
        arduboy.setTextSize(1);
        arduboy.print(i + 1);
        arduboy.print(" ");
        arduboy.print(initials[0]);
        arduboy.print(initials[1]);
        arduboy.print(initials[2]);
        arduboy.print(" ");
        arduboy.print(score);

        score = 0;
      }
      if (arduboy.pressed(A_BUTTON) && released) {
        released = false;
        state = 0;
      }

      if (arduboy.pressed(LEFT_BUTTON) && released) {
        released = false;
        state = 5;
      }
      break;
    case 4:
      arduboy.setCursor(32, 30);
      arduboy.setTextSize(2);
      arduboy.print("PAUSE");
      if (arduboy.pressed(B_BUTTON) && released) {
        released = false;
        state = 1;
      }
      if (arduboy.pressed(A_BUTTON) && released) {
        released = false;
        resetGame();
      }
      if ((arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) && released) {
        released = false;
        if (arduboy.audio.enabled()) arduboy.audio.off();
        else arduboy.audio.on();
        arduboy.audio.saveOnOff();
      }
      break;
    case 5:
      arduboy.setCursor(14, 32);
      arduboy.setTextSize(1);
      arduboy.print(F("EREASE HIGHSCORE?"));
      arduboy.setCursor(14, 40);
      arduboy.print(F("START = RIGHT + B"));
      if (arduboy.pressed(RIGHT_BUTTON + B_BUTTON)) {
        released = false;
        for (int i = 0; i < 35; i ++) {
          arduboy.clear();
          arduboy.setCursor(36, HEIGHT / 2);
          arduboy.print(F("WORKING..."));
          arduboy.display();
          EEPROM.update(i + 100, 0xff);
        }
        delay(500);
        arduboy.setCursor(32, HEIGHT / 2);
        arduboy.clear();
        arduboy.print(F("EREASE DONE"));
        arduboy.display();
        delay(1000);
        state = 0;

      }
      if (arduboy.pressed(LEFT_BUTTON) && released) {
        released = false;
        state = 0;
      }
      break;
  }
  if (arduboy.audio.enabled()) {
    arduboy.drawRect(WIDTH - 3, HEIGHT - 2, 2, 2, 1);
    arduboy.drawLine(WIDTH - 2, HEIGHT - 5, WIDTH - 2, HEIGHT - 1, 1);
    arduboy.drawPixel(WIDTH - 1, HEIGHT - 4, 1);
  }
  if (arduboy.notPressed(UP_BUTTON) && arduboy.notPressed(DOWN_BUTTON) && arduboy.notPressed(LEFT_BUTTON) &&
      arduboy.notPressed(RIGHT_BUTTON) && arduboy.notPressed(A_BUTTON) && arduboy.notPressed(B_BUTTON))
    released = true;
  arduboy.display();
}
