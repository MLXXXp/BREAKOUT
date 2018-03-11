uint8_t state = 0;
const uint8_t COLUMNS = 10; //Columns of bricks
const uint8_t ROWS = 4;     //Rows of bricks
int8_t dx;        //Initial movement of ball
int8_t dy;        //Initial movement of ball
uint8_t xb;           //Balls starting possition
uint8_t yb;           //Balls starting possition
boolean released;     //If buttons has been released by the player
boolean Free;   //If the ball is free
uint8_t xPaddle;       //X position of paddle
boolean isHit[ROWS][COLUMNS];   //Array of if bricks are hit or not
boolean bounced=false;  //Used to fix double bounce glitch
uint8_t lives = 3;       //Amount of lives
uint8_t level = 1;       //Current level
unsigned int score=0;   //Score for the game
short brickCount;  //Amount of bricks hit
char text[16];      //General string buffer
char initials[3];     //Initials used in high score
uint8_t hi, lo;
char tmpInitials[3];
unsigned int tmpScore = 0;


#include <Arduboy.h>
Arduboy arduboy;


void Paddle() {
  //Move right
  if(xPaddle < WIDTH - 47) if (arduboy.pressed(RIGHT_BUTTON)) xPaddle+=3;
  //Move left
  if(xPaddle > 0) if (arduboy.pressed(LEFT_BUTTON)) xPaddle-=3;
  
  //prevent it from going over to the sides
  if (xPaddle < 1) xPaddle = 0;
  if (xPaddle > WIDTH-47) xPaddle = WIDTH - 47;
  arduboy.drawRect(xPaddle, 63, 16, 1, 1);
}
void Ball() {
  if(Free) {
    //Move ball
    xb = xb + dx;
    yb = yb + dy;
    //Bounce off top edge
    if (yb <= 1) {
      yb = 1;
      dy = -dy;
      arduboy.tunes.tone(523, 200);
    }
    //Lose a life if bottom edge hit
    if (yb >= HEIGHT) {
      arduboy.tunes.tone(175, 200);
      delay(250);
      yb=60;
      Free = false;
      lives--;
    }
    //Bounce off left side
    if (xb <= 1) {
      xb = 1;
      dx = -dx;
      arduboy.tunes.tone(523, 200);
    }
    //Bounce off right side
    if (xb >= WIDTH - 31) {
      xb = WIDTH - 32;
      dx = -dx;
      arduboy.tunes.tone(523, 200);
    }
    //Bounce off paddle
    if (xb + 1 >= xPaddle && xb <= xPaddle+17 && yb+2 >= 63 && yb <= 64) {
      dy = -dy;
      dx = (xb-(xPaddle+6))/3; //Applies spin on the ball
      //Prevent straight bounce
      if (dx == 0) {
        dx = (random(-1,1));
      }
      //limit horizontal speed
      if (dx < -2) dx = -2;
      if (dx > 2)  dx =  2;
      arduboy.tunes.tone(200, 200);
    }
    //Reset Bounce
    bounced = false;
  } else {
    //Ball follows paddle
    xb = xPaddle + 7;
    yb = 60;
    //Release ball if FIRE pressed
    if (arduboy.pressed(B_BUTTON) && released) {
      released = false;
      Free = true;
      if (arduboy.pressed(LEFT_BUTTON) || arduboy.pressed(RIGHT_BUTTON)) {
        if (arduboy.pressed(LEFT_BUTTON)) dx = 1;
        else dx = -1;
      }else dx = random(-1, 1);
      //Ball heads upwards
      dy = -2;
    }
  }
  arduboy.drawRect(xb,yb,2,2,1);
}
void Brick() {
  //Bounce off Bricks
    for (byte row = 0; row < ROWS; row++) {
      for (byte column = 0; column < COLUMNS; column++) {
        if (!isHit[row][column]) {
          arduboy.drawRect(10*column, 2+6*row, 8, 4, 1);
          //If A collison has occured
          if (yb <= 6 * row + 7 && yb + 2 >= 6 * row + 1 &&
              xb <= 10 * column + 10 && xb + 2 >= 10 * column) {
            score = score + level * 5;
            brickCount++;
            isHit[row][column] = true;
            arduboy.drawRect(10*column, 2+6*row, 8, 4, 0);
            //Vertical collision
            if (yb + 2 > 6 * row + 7 || yb < 6 * row + 1) {
              //Only bounce once each ball move
              if(!bounced) {
                dy =- dy;
                bounced = true;
                arduboy.tunes.tone(261, 200);
              }
            }
            //Hoizontal collision
            if (xb < 10 * column || xb + 2 > 10 * column + 10) {
              //Only bounce once brick each ball move
              if(!bounced) {
                dx =- dx;
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
  lives = 2;
  xPaddle = 39;
  brickCount = 0;
  Free = false;
  yb = 60;
  for (byte row = 0; row < ROWS; row++)
    for (byte column = 0; column < COLUMNS; column++)
    isHit[row][column] = false;
}
void resetgame() {
  resetlevel();
  lives = 3;
  score = 0;
  state = 0;
}
void Statistics() {
  arduboy.setCursor(100,25);
  arduboy.print("LIVES");
  arduboy.setCursor(100,35);
  arduboy.print(lives);
  arduboy.setCursor(100,5);
  arduboy.print("LEVEL");
  arduboy.setCursor(100,15);
  arduboy.print(level);
  arduboy.setCursor(100, 45);
  arduboy.print("SCORE");
  arduboy.setCursor(100,55);
  arduboy.print(score);
  if (brickCount == ROWS * COLUMNS) {
    resetlevel();
    score = score + level * 100;
    level = level + 1;
    if (level <= 5) lives = lives + 1;
  }
}
void enterInitials() {
  short index = 0;
  initials[0] = ' ';
  initials[1] = ' ';
  initials[2] = ' ';
  while (true) {
    arduboy.clear();
    arduboy.setCursor(16,0);
    arduboy.print("HIGH SCORE");
    arduboy.setCursor(88, 0);
    arduboy.print(score);
    arduboy.setCursor(56, 20);
    arduboy.print(initials[0]);
    arduboy.setCursor(64, 20);
    arduboy.print(initials[1]);
    arduboy.setCursor(72, 20);
    arduboy.print(initials[2]);
    for(byte i = 0; i < 3; i++) {
      arduboy.drawLine(56 + (i*8), 27, 56 + (i*8) + 6, 27, 1);
    }
    arduboy.drawLine(56, 28, 88, 28, 0);
    arduboy.drawLine(56 + (index*8), 28, 56 + (index*8) + 6, 28, 1);
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
    if (arduboy.pressed(UP_BUTTON)){
      initials[index]--;
      arduboy.tunes.tone(523, 150);
      if (initials[index] == ' ') initials[index] = '?';
      if (initials[index] == '/') initials[index] = 'Z';
      if (initials[index] == 31)  initials[index] = '/';
      if (initials[index] == '@') initials[index] = ' ';
    }
    if (arduboy.pressed(A_BUTTON) && released) {
      released = false;
      if (index >= 2) {
        state = 0;
        arduboy.tunes.tone(1046, 200);
        return;
      }
    }
    if (arduboy.notPressed(LEFT_BUTTON) && arduboy.notPressed(RIGHT_BUTTON) && arduboy.notPressed(A_BUTTON))
    released = true;
    arduboy.display();
  }
}



void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);
  arduboy.display();
  if (arduboy.audio.enabled()) arduboy.audio.on();
  else arduboy.audio.off();
  arduboy.tunes.tone(987, 160);
  delay(160);
  arduboy.tunes.tone(1318, 400);
}
void loop() {
  if ((!arduboy.nextFrame())) return;
  arduboy.clear();
  switch(state) {
    case 0:{
      arduboy.setCursor(18,22);
      arduboy.setTextSize(2);
      arduboy.print("BREAKOUT");
      arduboy.setTextSize(1);
      arduboy.setCursor(31, 53);
        arduboy.print("PRESS FIRE!");
      if (arduboy.pressed(B_BUTTON) && released) {
        released = false;
        state = 1;
      }
      if (arduboy.pressed(A_BUTTON) && released) {
        released = false;
        state = 3;
      }
      if ((arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) && released) {
        released = false;
        if (arduboy.audio.enabled()) arduboy.audio.off();
        else arduboy.audio.on();
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
    
    case 2:
    // Each block of EEPROM has 7 high scores, and each high score entry
    // is 5 bytes long:  3 bytes for initials and two bytes for score.
    // High score processing
    for(byte i = 0; i < 7; i++) {
      hi = EEPROM.read(100 + (5*i));
      lo = EEPROM.read(100 + (5*i) + 1);
      if ((hi == 0xFF) && (lo == 0xFF)) {
        // The values are uninitialized, so treat this entry
        // as a score of 0.
        tmpScore = 0;
      } else tmpScore = (hi << 8) | lo;
      if (score > tmpScore) {
        enterInitials();
        for(byte j = i;j < 7;j++) {
          hi = EEPROM.read(100 + (5*j));
          lo = EEPROM.read(100 + (5*j) + 1);

          if ((hi == 0xFF) && (lo == 0xFF)) tmpScore = 0;
          else tmpScore = (hi << 8) | lo;

          tmpInitials[0] = (char)EEPROM.read(100 + (5*j) + 2);
          tmpInitials[1] = (char)EEPROM.read(100 + (5*j) + 3);
          tmpInitials[2] = (char)EEPROM.read(100 + (5*j) + 4);
  
          // write score and initials to current slot
          EEPROM.write(100 + (5*j), ((score >> 8) & 0xFF));
          EEPROM.write(100 + (5*j) + 1, (score & 0xFF));
          EEPROM.write(100 + (5*j) + 2, initials[0]);
          EEPROM.write(100 + (5*j) + 3, initials[1]);
          EEPROM.write(100 + (5*j) + 4, initials[2]);
  
          // tmpScore and tmpInitials now hold what we want to
          //write in the next slot.
          score = tmpScore;
          initials[0] = tmpInitials[0];
          initials[1] = tmpInitials[1];
          initials[2] = tmpInitials[2];
        }
        initials[0] = ' ';
        initials[1] = ' ';
        initials[2] = ' ';
      }
    }
    resetgame();
    
    
    break;
    case 3:
    // Each block of EEPROM has 7 high scores, and each high score entry
    // is 5 bytes long:  3 bytes for initials and two bytes for score
    arduboy.setCursor(32, 0);
    arduboy.print("HIGH SCORES");
    
    for(int i = 0; i < 7; i++) {
      arduboy.setCursor(34,9+(i*8));
      arduboy.print(i + 1);
      hi = EEPROM.read(100 + (5*i));
      lo = EEPROM.read(100 + (5*i) + 1);
    
      if ((hi == 0xFF) && (lo == 0xFF)) score = 0;
      else score = (hi << 8) | lo;
    
      initials[0] = (char)EEPROM.read(100 + (5*i) + 2);
      initials[1] = (char)EEPROM.read(100 + (5*i) + 3);
      initials[2] = (char)EEPROM.read(100 + (5*i) + 4);
    
      if (score > 0) {
        sprintf(text, "%c%c%c %u", initials[0], initials[1], initials[2], score);
        arduboy.setCursor(48,9 + (i*8));
        arduboy.print(text);
      }
      score = 0;
    }
    if (arduboy.pressed(A_BUTTON) && released) {
      released = false;
      state = 0;
    }
    if (arduboy.pressed(B_BUTTON) && released) {
      released = false;
      state = 1;
    }
    if (arduboy.pressed(LEFT_BUTTON) && released) {
      released = false;
      state = 5;
    }
    if ((arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) && released) {
      released = false;
      if (arduboy.audio.enabled()) arduboy.audio.off();
      else arduboy.audio.on();
    }
    break;
    
    case 4:
    arduboy.setCursor(WIDTH / 2 - 18, HEIGHT / 2);
    arduboy.print("PAUSED");
    if (arduboy.pressed(B_BUTTON) && released) {
      released = false;
      state = 1;
    }
    if (arduboy.pressed(A_BUTTON) && released) {
      released = false;
      resetgame();
    }
      if ((arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) && released) {
      released = false;
      if (arduboy.audio.enabled()) arduboy.audio.off();
      else arduboy.audio.on();
    }
    break;
    case 5:
    arduboy.setCursor(14, HEIGHT / 2);
    arduboy.print("EREASE HIGHSCORE?");
    if (arduboy.pressed(RIGHT_BUTTON + B_BUTTON)) {
      released = false;
      for (int i = 0; i < 70; i ++) {
        arduboy.clear();
        arduboy.setCursor(36,32);
        arduboy.print("WORKING...");
        arduboy.display();
        EEPROM.update(i + 100,0xff);
      }
      delay(500);
      arduboy.setCursor(33,32);
      arduboy.clear();
      arduboy.print("EREASE DONE");
      arduboy.display();
      delay(1000);
      state = 0;
      break;
    }
    if (arduboy.pressed(LEFT_BUTTON) && released) {
      released = false;
      state = 0;
    }
  }
  if (arduboy.audio.enabled()) {
    arduboy.drawRect(WIDTH - 3,HEIGHT - 2,2,2,1);
    arduboy.drawLine(WIDTH - 2, HEIGHT - 5, WIDTH - 2, HEIGHT - 1,1);
    arduboy.drawPixel(WIDTH - 1,HEIGHT - 4,1);
  }
  if (arduboy.notPressed(UP_BUTTON) && arduboy.notPressed(DOWN_BUTTON) && arduboy.notPressed(LEFT_BUTTON) &&
    arduboy.notPressed(RIGHT_BUTTON) && arduboy.notPressed(A_BUTTON) && arduboy.notPressed(B_BUTTON))
    released = true;
  arduboy.display();
}
