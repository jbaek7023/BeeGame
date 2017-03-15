#include <stdlib.h>
#include <stdio.h>

#include "myLib.h"
#include "text.h"
#include "gtLogo3.h"
#include "bullDog5.h"
#include "bullDog10.h"
#include "bullDog15.h"
#include "bullDog20.h"
#include "bullDog30.h"
#include "background.h"
#include "gtWin.h"
#include "deadBee.h"

#define REG_DISPCTL *(u16 *)0x4000000

#define MODE3 3
#define BG2_ENABLE (1<<10)

//color stuff
#define RGB(r, g, b) ((b)<<10 | (g)<<5 | (r))
#define RED RGB(31,0,0)
#define BLUE RGB(0,0,31)
#define CYAN RGB(0, 31, 31)
#define YELLOW RGB(31, 31, 0)
#define BLACK 0

extern u16 *videoBuffer; 

// Buttons
#define BUTTON_A	(1<<0)
#define BUTTON_B	(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT	(1<<5)
#define BUTTON_UP	(1<<6)
#define BUTTON_DOWN	(1<<7)
#define BUTTON_R	(1<<8)
#define BUTTON_L	(1<<9)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

#define BUTTONS *(volatile unsigned int *)0x4000130

enum {SPLASH, GAME, WIN, LOSE, LOSELIFE};
void setGameGround();
void splash();
int game(int seed);
void win();
void lose();
int checkWin();
int checkCollision();
void drawScore();
MOVOBJ objs[NUMOBJS];
MOVOBJ oldobjs[NUMOBJS];
MOVOBJ *cur;
MOVOBJ me[1], oldMe[1];
void loselife();
void reset();
int calledResetOnGame=0;

int dels[] = {-3, -2, -1, 1, 2, 3};
int blockPosition[] = {30, 80, 29, 57, 10, 90, 40, 80, 20, 50 };
int blockHeight[]= {20, 5, 30, 10, 10, 30, 15, 20, 10, 30};

int score;

int main(void){
	int state=SPLASH;
	while(1) {
		switch(state) {
		case SPLASH:
			splash();
			state = GAME;
			break;
		case GAME:
			state = game(0);
			break;
		case WIN:
			win();
			state = SPLASH;
			break;
		case LOSE:
			lose();
			state = SPLASH;
			break;
		case LOSELIFE:
			loselife();	
			state = SPLASH;
		default:
			break;
		} 
	} 	
}

void splash()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	drawBackground(background);
	drawString(50, 5, "Bee Adventure", YELLOW);
	drawString(70, 5, "Press Start (Enter)", YELLOW);
	while(!KEY_DOWN_NOW(BUTTON_START));
	while(KEY_DOWN_NOW(BUTTON_START));
	setGameGround();
}		

void setGameGround(){
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	u16 *ptr = videoBuffer;
	int i;
	for(i=0; i<38400; i++){
		*ptr++ = BLACK;
	}
	drawVerticalLine(CYAN, RED);	
	
	drawRect(SCREENMAXHEIGHT/2-10, 220, 20, 20, CYAN);
	drawRect(SCREENMAXHEIGHT/2-5, 220, 10, 15, BLACK);
}

void win(int score){
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	
	drawBackground(gtWin);

	drawString(50, 5, "YOU WON!!!!!", CYAN);
	drawString(70, 5, "Main Page (Enter)", CYAN);
	while(!KEY_DOWN_NOW(BUTTON_START)&&!KEY_DOWN_NOW(BUTTON_SELECT));
	if(KEY_DOWN_NOW(BUTTON_SELECT)){
		reset();
	}
	while(KEY_DOWN_NOW(BUTTON_START));
}

void lose(){
	REG_DISPCTL = MODE3 | BG2_ENABLE;

 	drawBackground(deadBee);
	
	drawString(70, 5, "bumped into pipe.", CYAN);	
	drawString(90, 5, "Main Page (Enter)", CYAN);
	while(!KEY_DOWN_NOW(BUTTON_START)&&!KEY_DOWN_NOW(BUTTON_SELECT));
	if(KEY_DOWN_NOW(BUTTON_SELECT)){
		reset();
	}
	while(KEY_DOWN_NOW(BUTTON_START));
}

void loselife(){
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	
	drawBackground(deadBee);
	
	drawString(50, 5, "OUT OF LIFE", CYAN);
	drawString(70, 5, "Main Page (Enter)", CYAN);
	while(!KEY_DOWN_NOW(BUTTON_START)&&!KEY_DOWN_NOW(BUTTON_SELECT));
	if(KEY_DOWN_NOW(BUTTON_SELECT)){
		reset();
	}
	while(KEY_DOWN_NOW(BUTTON_START));
}

//Checking collision. I calculated every possibility.
int checkCollision(){
	int i;
	for(i=0; i<NUMOBJS; i++){
		//left and up
		if(objs[i].left>me[0].left&&objs[i].left<me[0].right){
			if(objs[i].up>me[0].up&&objs[i].up<me[0].down){
				return 1;
			}
		}

		//right and up
		if(objs[i].right>me[0].left&&objs[i].right<me[0].right){
			if(objs[i].up>me[0].up&&objs[i].up<me[0].down){
				return 1;
			}
		}

		//left and down
		if(objs[i].left>me[0].left&&objs[i].left<me[0].right){
			if(objs[i].down>me[0].up&&objs[i].down<me[0].down){
				return 1;
			}
		}

		//right and down
		if(objs[i].right>me[0].left&&objs[i].right<me[0].right){
			if(objs[i].down>me[0].up&&objs[i].down<me[0].down){
				return 1;
			}
		}
	}

	for(i=0; i<NUMOBJS; i++){
		//left and up
		if(me[0].left>objs[i].left&&me[0].left<objs[i].right){
			if(me[0].up>objs[i].up&&me[0].up<objs[i].down){
				return 1;
			}
		}

		//right and up
		if(me[0].right>objs[i].left&&me[0].right<objs[i].right){
			if(me[0].up>objs[i].up&&me[0].up<objs[i].down){
				return 1;
			}
		}

		//left and down
		if(me[0].left>objs[i].left&&me[0].left<objs[i].right){
			if(me[0].down>objs[i].up&&me[0].down<objs[i].down){
				return 1;
			}
		}

		//right and down
		if(me[0].right>objs[i].left&&me[0].right<objs[i].right){
			if(me[0].down>objs[i].up&&me[0].down<objs[i].down){
				return 1;
			}
		}
	}	
	return 0; 
}

int checkWin(){
	if(me[0].col>220) return 1;	
	else return 0;	//false
}

void reset(){
	score=100;
	
	//setting my block axies
	me[0].row=SCREENMAXHEIGHT/2-5;
	me[0].col=0;
	me[0].rd=0;
	me[0].cd=6;
	me[0].color = RED;
	oldMe[0]=me[0];

	int numdels = sizeof(dels)/sizeof(dels[0]);
	int i;
	//even if we are resetting, the directions of pipes are randomly generated.
	for(i=0; i<NUMOBJS; i++) {
		objs[i].row = blockPosition[i];
		objs[i].col = GAMEWIDTH+GAMEWIDTH*i+BOXWIDTH/8;
		objs[i].rd = dels[rand()%numdels];
		objs[i].cd = dels[rand()%numdels];
		objs[i].color = BLUE;
		oldobjs[i] = objs[i];
	}
	if(calledResetOnGame==1){
		splash();
		calledResetOnGame=0;
	}
}

int game(int seed){
	seed=0;
	char buffer[41]; 
	score = 100;
	int numdels = sizeof(dels)/sizeof(dels[0]);
	int i;
	
	//velocity is random.
	//setting enemy blocks
	for(i=0; i<NUMOBJS; i++) {
		objs[i].row = blockPosition[i];
		objs[i].col = GAMEWIDTH+GAMEWIDTH*i+BOXWIDTH/8;
		objs[i].rd = dels[rand()%numdels];
		objs[i].cd = dels[rand()%numdels];
		objs[i].color = BLUE;
		oldobjs[i] = objs[i];
	}
	//setting my block
	me[0].row=SCREENMAXHEIGHT/2-5;
	me[0].col=0;
	me[0].rd=0;
	me[0].cd=6;
	me[0].color = RED;
	oldMe[0]=me[0];
	
	int scoreCounter=9;
	while(1){
		//Controller
		if(KEY_DOWN_NOW(BUTTON_LEFT)) {
			cur=me;
			cur->col -= cur->cd;	
			//if user tries to go left from the screen,
			if(me[0].col < 0) {	
				//reset
				cur->col += cur->cd;
			}
		}
		if(KEY_DOWN_NOW(BUTTON_RIGHT)) {
			cur=me;
			cur->col += cur->cd;	
		}
		if(KEY_DOWN_NOW(BUTTON_SELECT)) {
			calledResetOnGame=1;
			reset();
		}

		//LOOP Blocks
		for(i=0; i<NUMOBJS;i++) {
			cur = objs + i;
			cur->row = cur->row + cur->rd;
			if(cur->row<0) {
				cur->row=0;
				cur->rd=-cur->rd;
			}
			if(cur->row>SCREENMAXHEIGHT-blockHeight[i]-10+1) {	
				//10 is score height
				cur->row=SCREENMAXHEIGHT-blockHeight[i]-10+1;
				cur->rd=-cur->rd;
			}
		}
		
		//life
		scoreCounter--;
		if(scoreCounter==0) {
			score--;
			scoreCounter=9;
		}
		sprintf(buffer, "%d HP", score);
		drawRect(150, 5, 10, 72, BLACK);	
		drawString(150, 5, buffer, YELLOW);	
		
		
		for(i=0; i<NUMOBJS; i++) {
			//erase old blocks
			drawImage3(oldobjs[i].row, oldobjs[i].col, blockHeight[i] , BOXWIDTH, BLACK);
			
			//draw new blocks
			cur = objs + i;	
			if((i==3)|(i==4)|(i==8)){
				drawImage3(cur->row, cur->col, blockHeight[i], BOXWIDTH, bullDog10);
			} else if (i==1){
				drawImage3(cur->row, cur->col, blockHeight[i], BOXWIDTH, bullDog5);
			} else if (i==6){
				drawImage3(cur->row, cur->col, blockHeight[i], BOXWIDTH, bullDog15);
			} else if ((i==0)|(i==7)){
				drawImage3(cur->row, cur->col, blockHeight[i], BOXWIDTH, bullDog20);
			} else if ((i==2)|(i==5)|(i==9)){
				drawImage3(cur->row, cur->col, blockHeight[i], BOXWIDTH, bullDog30);
			}
			oldobjs[i] = objs[i];
			//set the edges of me[o]... continue to checkCollision()			
			objs[i].up=cur->row;
			objs[i].down=cur->row+blockHeight[i];
			objs[i].left=cur->col;
			objs[i].right=cur->col+BOXWIDTH;
		}

		//draw old me
		drawImage3(oldMe[0].row, oldMe[0].col, 10, 10, BLACK);
		cur = me;	
		//drawRect(cur->row, cur->col, 10, 10, cur->color);
		drawImage3(cur->row, cur->col, 10, 10, gtLogo3);
							
		oldMe[0] = me[0];
		//set the edges of me[o]... continue to checkCollision()
		me[0].up=cur->row;
		me[0].down=cur->row+10;
		me[0].left=cur->col;
		me[0].right=cur->col+10;
			
		if(checkCollision()==1){
			return LOSE;
		}
		if(score==0){
			return LOSELIFE;
		}
		if(checkWin()==1){
			return WIN;
		}
		//slow here 
		waitForVblank();
	}
	return 0;
}



























