typedef unsigned short u16;
typedef unsigned int u32;
#define SCREENMAXHEIGHT 160
#define GAMEWIDTH 20
#define OFFSET(r, c, numcols) ((r)*(numcols) + (c))
#define BOXWIDTH 16 
#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006
#define NUMOBJS 10

//proto types
void setPixel(int row, int col, u16 color);
void drawRect(int row, int col, int height, int width, u16 color);
void drawHollowRect(int row, int col, int height, int width, u16 color);
void drawVerticalLine(u16 color, u16 color2);
void waitForVblank();
void drawImage3(int row, int col, int height, int width, const u16* image);
void drawBackground(const unsigned short arr[]);

typedef struct {
	int row;
	int col;
	int rd;
	int cd;
	int up, right, left, down;
	u16 color;
	u16* imagetype; 
} MOVOBJ;

/* DMA */

typedef struct
{								// ***********************************************************
	 const volatile void *src;	// We mark this as const which means we can assign it const
	 volatile void *dst;		// things without the compiler yelling but we can also pass it
	 volatile u32 cnt;		// things that are not const!
								// ***********************************************************
} DMAREC;

#define DMA ((volatile DMAREC *)0x040000B0)

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

#define START_ON_FIFO_EMPTY 0x30000000

