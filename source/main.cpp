/*---------------------------------------------------------------------------------

	$Id: main.cpp,v 1.13 2008-12-02 20:21:20 dovoto Exp $

	Simple console print demo
	-- dovoto


---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include "space.h"
#include "ship.h"
#include "barrel.h"
#include "fracas.h"
#include "laser.h"
#include <time.h>

volatile int frame = 0;
int shipSpeed = 3;
int ypos = 0;
int barrelYPos = 80;
int barrelPos = 267;
int barrelSpeed = 3;
int maxBarrels = 5;
int usedBarrels = 0;
int framesBetweenSpawns = 256; 
bool gameover = false;
bool fracasPresent = false;
int fracasPos = 256;
int fracasSpeed = 1;
bool laserActive = false;
int laserSpeed = 3;
int laserPos = 0;
int laserYPos = 0;
int fracasHealth = 10;
int fracasMaxHealth = 10;
bool victory = false;
int angle = 0;

//Struct to store the sprite information
typedef struct  
{
   u16* gfx;
   SpriteSize size;
   SpriteColorFormat format;
   int rotationIndex;
   int paletteAlpha;
   int x;
   int y;
}MySprite;


void Vblank() 
{
	frame++;
}


	
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	//Set the seed
	srand(time(NULL));
	//Pick a start position for the 1st barrel
	barrelYPos = rand()%128;
	
	//Create the objects for the game sprites
   MySprite sprites[] = {
	  {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 256, barrelYPos}, //Barrel
	  {0, SpriteSize_32x32, SpriteColorFormat_256Color, 1, 0, 0, 0},//Ship
	  {0, SpriteSize_8x8, SpriteColorFormat_256Color, 2, 0, 0, 0}, //Bullet
	  {0, SpriteSize_64x64, SpriteColorFormat_256Color, 3, 0, 256, 50} //Fracas
   };
   

	irqSet(IRQ_VBLANK, Vblank); //Interrupt for when finished drawing so can put on screen

	
	int allowScroll = 1; //Allow the background to scroll from the start
	
	
	//Set the video mode and set up the memory for the background
	videoSetMode(MODE_5_2D);
	vramSetBankB(VRAM_B_MAIN_BG_0x06000000);
	
	//Enable the console
	consoleDemoInit();
	
	//Initilize the top screen for the use of sprites
   oamInit(&oamMain, SpriteMapping_Bmp_1D_128, true); //TRUE FOR EXTENDED PALETTE
  
   
	//Set up sprites in memory bank
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	//allocate some space for the sprite graphics
	for(int i = 0; i < 4; i++)
	  sprites[i].gfx = oamAllocateGfx(&oamMain, sprites[i].size, sprites[i].format);
	 
	
	//Put the sprite art into the sprite objects
	//Barrel
   dmaCopy(barrelTiles, sprites[0].gfx, 32*32);
   
   //Fracus
   dmaCopy(fracasTiles, sprites[3].gfx, 64*64);
   
	//BULLET
	dmaCopy(laserTiles, sprites[2].gfx, 8*8);

   //Ship
   dmaCopy(shipTiles, sprites[1].gfx, 32*32);
   
	//Set up the memory for extended palettes
	vramSetBankF(VRAM_F_LCD);

	dmaCopy(barrelPal, VRAM_F_EXT_SPR_PALETTE[0], 256*2);
	dmaCopy(shipPal, VRAM_F_EXT_SPR_PALETTE[1], 256*2);
	dmaCopy(fracasPal, VRAM_F_EXT_SPR_PALETTE[3], 256*2);
	dmaCopy(laserPal, VRAM_F_EXT_SPR_PALETTE[2], 256*2);
	
	vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);
	
	
	//For background. Theres 4 layes. This specifies layer 3 (0-3)
	
	int bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_512x256, 0,0);
	
	dmaCopy(spaceBitmap, bgGetGfxPtr(bg3), 512*256);
	dmaCopy(spacePal, BG_PALETTE, 256*2);
	
 
	while(1) 
	{
		//Set the sprites onto the screen
		
		//Barrel
	   oamSet(
      
		 &oamMain,
         0,       //oam entry to set
         sprites[0].x, sprites[0].y, //position 
         0, //priority
		 0,
		
         sprites[0].size, 
		 sprites[0].format, 
		 sprites[0].gfx, 
		 sprites[0].rotationIndex, 
         true, //double the size of rotated sprites
         false, //don't hide the sprite
		 false, false, //vflip, hflip
		 false //apply mosaic
         );
		 
		 
		 //Ship
		  oamSet(
        
		 &oamMain,
         1,       //oam entry to set
         sprites[1].x, sprites[1].y, //position 
         0, //priority
		 1,
		
         sprites[1].size, 
		 sprites[1].format, 
		 sprites[1].gfx, 
		 sprites[1].rotationIndex, 
         true, //double the size of rotated sprites
         false, //don't hide the sprite
		 false, false, //vflip, hflip
		 false //apply mosaic
         );
		 
		
	  
		 
		 
		 
		 
		 //FRACAS
		 if(!fracasPresent)
	   oamSet(
        
		 &oamMain,
         3,       //oam entry to set
         sprites[3].x, sprites[3].y, //position 
         0, //priority
		 3,
		
         sprites[3].size, 
		 sprites[3].format, 
		 sprites[3].gfx, 
		 sprites[3].rotationIndex, 
         true, //double the size of rotated sprites
         true, //don't hide the sprite
		 false, false, //vflip, hflip
		 false //apply mosaic
         );
	  else
	   oamSet(
      
		 &oamMain,
         3,       //oam entry to set
         sprites[3].x, sprites[3].y, //position 
         0, //priority
		 3,
		
         sprites[3].size, 
		 sprites[3].format, 
		 sprites[3].gfx, 
		 sprites[3].rotationIndex, 
         true, //double the size of rotated sprites
         false, //don't hide the sprite
		 false, false, //vflip, hflip
		 false //apply mosaic
         );
		 
		 //Laser
		 if(laserActive)
		 {
		 oamSet(
		  &oamMain,
         2,       //oam entry to set
         sprites[2].x, sprites[2].y, //position 
         0, //priority
		 2,
	
         sprites[2].size, 
		 sprites[2].format, 
		 sprites[2].gfx, 
		 sprites[2].rotationIndex, 
         true, //double the size of rotated sprites
         false, //don't hide the sprite
		 false, false, //vflip, hflip
		 false //apply mosaic
         );
		 }
		 else
		 {
		 oamSet(
		  &oamMain,
         2,       //oam entry to set
         sprites[2].x, sprites[2].y, //position 
         0, //priority
		 2,
	
         sprites[2].size, 
		 sprites[2].format, 
		 sprites[2].gfx, 
		 sprites[2].rotationIndex, 
         true, //double the size of rotated sprites
         true, //don't hide the sprite
		 false, false, //vflip, hflip
		 false //apply mosaic
         );
		 }
		
	
		
		//Scale fracas to be 128x128 because biggest size ds supports is 64x64
		oamRotateScale(&oamMain, 3, 0, (1 << 7), (1<<7));
		
		//Spin ship if hit by barrel
		if(gameover)
		{
			oamRotateScale(&oamMain, 1, angle, (1 << 8), (1<<8));
			angle += 150;
		}
	
		
		//Move the barrel
		if(barrelPos > -35 && usedBarrels != 0)
		{
			barrelPos-=barrelSpeed;
			sprites[0].x-=barrelSpeed;
		}
		
		
		
		//Check if laser offscreen
		if(laserActive && laserPos > 256)
		{
			laserActive = false;
		}
		
		//Check if laser hit barrel
		if(laserActive && laserYPos > barrelYPos && laserYPos < (barrelYPos + 32))
		{
			if(laserPos > barrelPos && laserPos < (barrelPos + 32))
			{
				laserActive = false;
				//Move the barrel away from the game
				barrelPos = -46;
				sprites[0].x = -46;
			}
		}
		
		//Check if laser hit fracas
		if(allowScroll == 0 && fracasPresent && laserActive && laserYPos > 50 && laserYPos < (114))
		{
			if(laserPos > 128)
			{
				laserActive = false;
				fracasHealth--;
			}
		}
		
		//Update fracas's health on the bottom screen
		if(fracasPresent && !gameover)
		{
			consoleClear();
			iprintf("\x1b[33mFRACAS: [\x1b[32m");
			for(int healthInd = 0; healthInd < fracasHealth; healthInd++)
			{
				iprintf("|");
			}
			iprintf("\x1b[31m");
			for(int healthInd = 0; healthInd < fracasMaxHealth - fracasHealth; healthInd++)
			{
				iprintf("|");
			}
			iprintf("\x1b[33m]\x1b[37m");
		}
		
		//Show progress towards the boss
		if(!fracasPresent && !gameover && !victory)
		{
			consoleClear();
			iprintf("\x1b[32mPROGRESS: ");
			for(int healthInd = 0; healthInd < usedBarrels; healthInd++)
			{
				iprintf("_");
			}
			iprintf(">");
			for(int healthInd = 0; healthInd < maxBarrels - usedBarrels; healthInd++)
			{
				iprintf("_");
			}
			iprintf("\x1b[39m");
		}
		
		//Move laser
		if(laserActive)
		{
			laserPos+=laserSpeed;
			sprites[2].x+=laserSpeed;
		}
		
		//Move fracas
		if(fracasPresent && fracasPos > 128 && !gameover)
		{
			if(frame%2 == 0)
			{
				fracasPos-=fracasSpeed;
				sprites[3].x-=fracasSpeed;	
			}	
		}
		
		//Reset the barrel
		
		if(frame !=0 && frame%framesBetweenSpawns == 0 && !gameover && !victory)
		{
			if(usedBarrels != maxBarrels)
			{
				barrelPos = 267;
				sprites[0].x = 256;
				barrelYPos = rand()%128;
				sprites[0].y = barrelYPos;
				usedBarrels++;
				
				
				//Have fracas move in
				if(usedBarrels == maxBarrels)
				{
					fracasPresent = true;
				}
			}
			else //Boss Time. Adjust the spawn range of barrels
			{
				allowScroll = 0;
				barrelPos = 171;
				sprites[0].x = 160;
				barrelYPos = 50 + rand()%64;
				sprites[0].y = barrelYPos;
			}
			
		}
		
		//Check if barrel hits the ship
		if(barrelPos < 0 && barrelPos > -32)
		{
			if((ypos + 20) > barrelYPos && (ypos - 20) < barrelYPos)
			{
				if(!gameover)
				{
					consoleClear();
					iprintf("GAME OVER");
					gameover = true;
					allowScroll = 0;
				}
			}
		}
		
		//Check if victory
		if(fracasHealth <= 0 && !victory)
		{
			fracasPresent = false;
			victory = true;
			consoleClear();
			iprintf("YOU DID IT!");
		}
		
		//Print the controls and objective
		if(!victory && !gameover)
		{
			iprintf("\x1b[36m\n\nDefeat the evil Fracas \nat all costs!\nAvoid his arsonal of \nbarrel-based weaponry and \nsave the universe!\n\n\x1b[37m");
			iprintf("D-Pad - Move Ship\nA - Shoot\n");
		}

		

		//For button inputs
		scanKeys();
		
		if(keysHeld() & (KEY_UP))
		{
			if(ypos > -10 && !gameover)
			{
				ypos-=shipSpeed;
				sprites[1].y-=shipSpeed;
			}
		}
		if(keysHeld() & (KEY_DOWN))
		{
			if(ypos < 140 && !gameover)
			{
				ypos+=shipSpeed;
				sprites[1].y+=shipSpeed;
			}
		}
		if(keysHeld() & (KEY_A))
		{
			if(!laserActive && !gameover)
			{
				laserActive = true;
				laserYPos = ypos + 24;
				sprites[2].y = ypos + 24;
				laserPos = 32;
				sprites[2].x = laserPos;
			}
		}
	
		//Scroll the background
		if(allowScroll == 1)
		bgSetScroll(3, (1 + frame%256), 0);
		bgUpdate();
		
		
		swiWaitForVBlank(); //Wait for drawing to be done before putting on screen
		
		oamUpdate(&oamMain);//Update the sprites
		
		
	}

	return 0;
}
