#include "myLib.h"

u16 *videoBuffer = (u16 *) 0x6000000;

//setting a pixel
void setPixel(int row, int col, u16 color) {
	videoBuffer[OFFSET(row, col, 240)] = color;
}

//draw a rectangle
void drawRect(int row, int col, int height, int width, u16 color){
	int r,c;
	for(r=0; r<height; r++){
		for(c=0; c<width; c++){
			setPixel(row+r, col+c, color);
		}
	}
}

void drawHollowRect(int row, int col, int height, int width, u16 color){
	int r,c;
	for(r=0; r<height; r++){
		for(c=0; c<width; c++) {
			if((c==0)|(c==(width-1))|(r==0)|(r==(height-1))){
				setPixel(row+r, col+c, color);
			}
		}
	}
}

void drawVerticalLine(u16 color, u16 color2){
	int r, i;
	for(r=0; r<SCREENMAXHEIGHT-10; r++){	
		for(i=0;i<=NUMOBJS;i++){
			setPixel(r, GAMEWIDTH+GAMEWIDTH*i, color);
		}
	}
	for(r=68; r<SCREENMAXHEIGHT/2+12;r++){
		for(i=0;i<=NUMOBJS;i++){
			setPixel(r, GAMEWIDTH+GAMEWIDTH*i, 0);
		}
	}
}

void waitForVblank() {
	int i;
	//This slows the game speed. You can set 10 to 20 or 30. 
	//I could win this game with speed ten (faster)  
	for(i=0;i<20;i++){
		while(SCANLINECOUNTER > 160);
		while(SCANLINECOUNTER < 160);
	}
}


void drawImage3(int row, int col, int height, int width, const u16* image){
	int r;
	for(r = 0; r < height; r ++) {
		DMA[3].src = &image[r * width];
		DMA[3].dst = &videoBuffer[OFFSET(row + r, col, 240)];
		DMA[3].cnt = DMA_ON | width;
	}
}

void drawBackground(const unsigned short arr[]) {
	DMA[3].src = arr;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = (240*160) | DMA_ON;
}



