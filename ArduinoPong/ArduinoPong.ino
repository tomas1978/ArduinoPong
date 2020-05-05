#include"U8glib.h"

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);

class Ball
{
  private:
    float xpos;
    float ypos;
    float xspeed;
    float yspeed;
    float radius;
  public:
    Ball(int xp, int yp, int xs, int ys, int r):xpos(xp),ypos(yp),xspeed(xs),yspeed(ys),radius(r){}
    float getXpos(){return xpos;}
    void setXpos(int xp){xpos=xp;}
    float getYpos(){return ypos;}
    void setYpos(int yp){ypos=yp;}
    float getXspeed(){return xspeed;}
    void setXspeed(int xs){xspeed=xs;}
    float getYspeed(){return yspeed;}
    void setYspeed(int ys){yspeed=ys;}
    float getRadius(){return radius;}
    void setRadius(int r){radius=r;}
    void draw(){
      oled.drawCircle(xpos,ypos,radius);
    }
};

class Paddle
{
  private:
    float xpos;
    float ypos;
    float paddleSpeed;
    float paddleSize;
  public:
    Paddle(int xp, int yp, int sp, int s):xpos(xp),ypos(yp),paddleSpeed(sp),paddleSize(s){}
    float getXpos(){return xpos;}
    void setXpos(int xp){xpos=xp;}
    float getYpos(){return ypos;}
    void setYpos(int yp){ypos=yp;}
    float getPaddleSpeed(){return paddleSpeed;}
    void setPaddleSpeed(int sp){paddleSpeed=sp;}
    float getSize(){return paddleSize;}
    void setRadius(int s){paddleSize=s;}
    void draw(){
      oled.drawVLine(xpos,ypos,paddleSize);
    }
};

//Port numbers for input buttons
int player1Up=2;
int player1Down=3;
int player2Up=4;
int player2Down=5;
int player1Points=0;
int player2Points=0;

enum gameState{startMenu, playing, gameOver};
gameState state=startMenu;


Ball ball(50,60,2,3,2);
Paddle player1(10,25,1,20);
Paddle player2(120,25,1,20);

void setup() {
  Serial.begin(9600);
  pinMode(player1Up,INPUT);
  pinMode(player1Down,INPUT);
  pinMode(player2Up,INPUT);
  pinMode(player2Down,INPUT);
  oled.setFont(u8g_font_helvB10);
}

//Check if ball collides with paddles 
bool collision(Paddle p)
{
  bool collisionX = false;
  bool collisionY = false;
  Serial.println(abs(ball.getXpos()-p.getXpos()));
  if(abs(ball.getXpos()-p.getXpos())<2) {
    collisionX=true;
    Serial.println("Kollision i x-led");
  }

  if(ball.getYpos()<(p.getYpos()+p.getSize()) && ball.getYpos()>p.getYpos()) {
    collisionY=true;
    Serial.println("Kollision i y-led");
  }

  if(collisionX && collisionY) {
    return true;
  }
  return false;
}

bool reset() {
  ball.setXpos(50);
  ball.setYpos(60);
  delay(500);
}

void runStartMenu() { 
  player1Points=0;
  player2Points=0;
  
  oled.firstPage();
  do {
    oled.setFont(u8g_font_helvB18);
    oled.setPrintPos(25,24);
    oled.print("PONG");
    oled.setFont(u8g_font_helvB10);
    oled.setPrintPos(10,36);
    oled.print("Push a button to"); 
    oled.setPrintPos(10,48);
    oled.print("start game");
    if(digitalRead(player1Down) || digitalRead(player1Up) || digitalRead(player2Down) || digitalRead(player2Up)) {
      state=playing;
    }
  }while(oled.nextPage());
}

void playingGame() {
  if(digitalRead(player1Down)) {
    player1.setYpos(player1.getYpos()+player1.getPaddleSpeed());
  }
  if(digitalRead(player1Up)) {
    player1.setYpos(player1.getYpos()-player1.getPaddleSpeed());
  }
  if(digitalRead(player2Down)) {
    player2.setYpos(player2.getYpos()+player2.getPaddleSpeed());
  }
  if(digitalRead(player2Up)) {
    player2.setYpos(player2.getYpos()-player2.getPaddleSpeed());
  }

  ball.setXpos(ball.getXpos()+ball.getXspeed());
  ball.setYpos(ball.getYpos()+ball.getYspeed());

  if(ball.getYpos()-2*ball.getRadius()<0 || ball.getYpos()+ball.getRadius()>oled.getHeight()){
    ball.setYspeed(-ball.getYspeed());
  }

  if(collision(player1) || collision(player2)) {
    ball.setXspeed(-ball.getXspeed());
  }

  if(ball.getXpos()>oled.getWidth()) {
    player1Points++;
    reset();
  }

  if(ball.getXpos()<0) {
    player2Points++;
    reset();
  }

  if(player1Points==3 || player2Points==3) {
    state = gameOver;
  }
  
  oled.firstPage();
  do {
    oled.setPrintPos(0,12);
    oled.print(player1Points);

    oled.setPrintPos(oled.getWidth()-15,12);
    oled.print(player2Points);
    
    ball.draw();
    player1.draw();
    player2.draw();
  }while(oled.nextPage());
}

void printWinner() {
    oled.firstPage();
  do {
    oled.setFont(u8g_font_helvB14);
    oled.setPrintPos(10,24);
    oled.print("GAME OVER");
    oled.setFont(u8g_font_helvB10);
    oled.setPrintPos(10,36);
    if(player1Points>2) {
      oled.print("Winner: Player 1"); 
    }
    else {
      oled.print("Winner: Player 2");
    }
    oled.setPrintPos(10,48);
    oled.print("Push a button to");
    oled.setPrintPos(10,60);
    oled.print("restart game");
    
    if(digitalRead(player1Down) || digitalRead(player1Up) || digitalRead(player2Down) || digitalRead(player2Up)) {
      state=startMenu;
    }
  }while(oled.nextPage());
}

void loop() {
  if(state==startMenu) {
    runStartMenu();
  }
  if(state==playing) {
    playingGame();
  } 
  if(state==gameOver) {
    printWinner();
  }
}
