/*
  main.cpp
  OpenTTD 32bpp Tile Tool
  Made by Luca Spiller 2005
  For more info on OpenTTD see http://www.openttd.org/
  
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the
 Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#ifndef WIN32
#  include <dirent.h>
#else
#  include <windows.h>
#endif
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
using namespace std;

#include "tiles.h"
#include "font.h"
#include "error.h"

SDLFont *font;

int _loaded_tile_images = 0;
TileIMG _tiles_images[MAX_TILE_IMAGES][ZOOM_LEVELS];

bool _display_fl = 0;


TileIMG *tile_to_place = NULL;
TileBase *first_ptr = NULL;

int zoom = 1;

int HalfTileSizes[ZOOM_LEVELS][2] = {
	{ 128, 64 },
	{ 64, 32 },
	{ 32, 16 },
};

int TileOffset[ZOOM_LEVELS] = {
	32,
	16,
	8,
};

int SelectedTileX, SelectedTileY;
unsigned long SelectedTileExpiry = 0;
TileBase *SelectedTile;

int xoff = 270, yoff = 130;
int tiles_x = 5, tiles_y = 5;

SDL_Surface *screen, *tile_select, *filled_tile_select;

int colour;

#define BG_COLOURS 4

int BackgroundColours[BG_COLOURS] = {
	0x000000, // black
	0xFF00FF, // pink
	0xFFFF00, // yellow
	0x00FFFF, // cyan 
};

bool show_grid = true;

int main(int argc, char *argv[])
{
	int done = 0;
	Uint8* keys;
	unsigned long cTime;
	bool kSpaceDown = false, kQDown = false, kADown = false, kCDown = false, kHDown = false;
	
	// setup the SDL stuff
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
		error(E_FATAL, "Unable to init SDL: %s\n", SDL_GetError());
		
	atexit(SDL_Quit);
	
	// window icon and caption
	SDL_WM_SetIcon(IMG_Load("data/openttd.128.png"), NULL);
	SDL_WM_SetCaption("OpenTTD 32bpp Tile Tool", NULL);

	// and show the window
	screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if ( screen == NULL )
		error(E_FATAL, "Unable to set 800x600 video: %s\n", SDL_GetError());

	{ // attempt to load the required data files
		tile_select = IMG_Load("data/ets.png");
		if (!tile_select)	 error(E_FATAL, "Failed to load `data/ets.png` -- have you actually got the data files?");
		
		filled_tile_select = IMG_Load("data/fts.png");
		if (!tile_select)	 error(E_FATAL, "Failed to load `data/fts.png`");	
	
		if (!LoadTileSet("bnk"))
			error(E_FATAL, "Could not load required `bnk` tileset!");
	
		_loaded_tile_images++;
	
		if (!LoadTileSet("bts"))
			error(E_FATAL, "Could not load required `bts` tileset!");	
	
		// and the font
		font = initFont("data/font");
	}
	
	{ // read the data/tiles/ directory and load tilesets
#ifndef WIN32
		DIR *dir = opendir("data/tiles");
		struct dirent *dirent;
	
		if (dir != NULL) {
			while ((dirent = readdir(dir)) != NULL) {
				if (dirent->d_name[0] != '.' && strcmp(dirent->d_name, "bnk") && strcmp(dirent->d_name, "bts")) {
					if (_loaded_tile_images++ >= MAX_TILE_IMAGES)
						error(E_FATAL, "Too many tilesets loaded :(");
				
					if (!LoadTileSet(dirent->d_name))
						_loaded_tile_images--; // no need to completely give up
				}
			}
		}
		closedir(dir);
#else
		WIN32_FIND_DATA fd;
		HANDLE h = FindFirstFile("data/tiles/*.*", &fd);
		
		if (h != INVALID_HANDLE_VALUE) {
			do {
				if (fd.cFileName[0] != '.' && strcmp(fd.cFileName, "bnk") && strcmp(fd.cFileName, "bts")) {
					if (_loaded_tile_images++ >= MAX_TILE_IMAGES)
						error(E_FATAL, "Too many tilesets loaded :(");
					
					if (!LoadTileSet(fd.cFileName))
						_loaded_tile_images--; // no need to completely give up
				}
			} while (FindNextFile(h, &fd));
			FindClose(h);
		}
#endif
	}
	
	{ // set the tiles to blank initially
		int i;
		for (i = 0; i < 25; i++)
			AddTileToEnd(0, NULL);
		// hopefully we now have 25 blank tiles :D
	}
	
	while(done == 0)
	{
		SDL_Event event;
		int x, y, i;
		TileBase *tmp;

		cTime = SDL_GetTicks();

		// check for any events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				done = 1;	

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				done = 1;
				
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				int x, y, x2, y2;
				SDL_GetMouseState(&x, &y);
				if (_display_fl) {
					y2 = (int) floor((double)(y - 10) / 15) + 1;
					if (y2 > 1 && y2 <= _loaded_tile_images) {
						tile_to_place = _tiles_images[y2];
						_display_fl = !_display_fl;
					}
				} else {
					if (y > 10 && y < (40 * tiles_y) + 10 && x > screen->w - (40 * tiles_x) - 10 && x < screen->w - 10) {	
						x2 = (tiles_x - 1) - (screen->w - x - 10) / 40;
						y2 = (y - 10) / 40;
						if (tile_to_place == NULL) {
							SelectedTileX = x2;
							SelectedTileY = y2;
							SelectedTile = TileFromXY(x2, y2);
							SelectedTileExpiry = cTime + 10000;
						} else {
							TileLevel *tmp2;
							tmp = TileFromXY(x2, y2);
							tmp2 = NewTileLevelOnTop(tmp);
							tmp2->image = tile_to_place;
							tmp->height = 0;
							tile_to_place = NULL;
						}
					}
				}
			}
		}

		// check key states
		keys = SDL_GetKeyState(NULL);
		if (keys[SDLK_UP]) { yoff -= 5; }
		if (keys[SDLK_DOWN]) { yoff += 5; }
		if (keys[SDLK_LEFT]) { xoff -= 5; }
		if (keys[SDLK_RIGHT]) { xoff += 5; }

		if (keys[SDLK_w]) { 
			for (i = 0; i < tiles_y; i++)
				AddTileToEnd(0, _tiles_images[0]);
			tiles_x++;
		}
		if (keys[SDLK_s] && tiles_x > 1) {
			DeleteTilesFromEnd(tiles_y);
			tiles_x--;
		}
		if (keys[SDLK_e]) {
			for (i = 0; i < tiles_x; i++)
				AddTileAfter(tiles_y * (i + 1) + (i - 1), 0, _tiles_images[0]);
			tiles_y++;
		}
		if (keys[SDLK_d] && tiles_y > 1) {
			for (i = tiles_x; i > 0; i--)
				DeleteTileByID((tiles_y * i) - 1);
			tiles_y--;
		}

		if (keys[SDLK_p]) {
			if (SDL_SaveBMP(SDL_GetVideoSurface(), "screenshot.bmp") == -1)
				error(E_WARNING, "Unable to save screenshot `screenshot.bmp`");
			else
				error(E_WARNING, "Screenshot saved to `screenshot.bmp`");
		}

		if (!kCDown && keys[SDLK_c]) {
			colour++;
			if (colour == BG_COLOURS)
				colour = 0;
			kCDown = true;
		} else if (kCDown && !keys[SDLK_c])
			kCDown = false;

		if (!kHDown && keys[SDLK_h]) {
			show_grid = !show_grid;
			kHDown = true;
		} else if (kHDown && !keys[SDLK_h])
			kHDown = false;

		if (keys[SDLK_PAGEUP]) { if (zoom < ZOOM_LEVELS - 1) zoom++;}
		if (keys[SDLK_PAGEDOWN]) { if (zoom > 0) zoom--; }
		
		// change tile height
		if (!kQDown && SelectedTileExpiry != 0 && keys[SDLK_q]) {
			(TileFromXY(SelectedTileX, SelectedTileY))->height++;
			SelectedTileExpiry = cTime + 15000;
			kQDown = true;
		} else if (kQDown && !keys[SDLK_q])
			kQDown = false;
			
		if (!kADown && SelectedTileExpiry != 0 && keys[SDLK_a]) {
			(TileFromXY(SelectedTileX, SelectedTileY))->height--;
			SelectedTileExpiry = cTime + 15000;
			kADown = true;
		} else if (kADown && !keys[SDLK_a])
			kADown = false;
		
		// show tile select menu
		if (!kSpaceDown && keys[SDLK_SPACE]) {
			_display_fl = !_display_fl;
			kSpaceDown = true;
		} else if (kSpaceDown && !keys[SDLK_SPACE])
			kSpaceDown = false;
		
		
		// Drawing stuff
		SDL_FillRect(screen, NULL, BackgroundColours[colour]);

		tmp = first_ptr;
		for (x = 0; x < tiles_x; x++) {
			for (y = 0; y < tiles_y; y++) {
				SDL_Rect dest;
				TileLevel *level;
				
				if (tmp == NULL)
					error(E_FATAL, "Attempted to draw non-existant tile (%d, %d)!", x, y);

				// Isometric view		
				// base tiles - these stay at height level 0
				if (show_grid) {
					dest.x = xoff + x * HalfTileSizes[zoom][0] - (HalfTileSizes[zoom][0] * y); // tile_x
					dest.y = yoff + y * HalfTileSizes[zoom][1] + (HalfTileSizes[zoom][1] * x); // tile_y
					SDL_BlitSurface(_tiles_images[0][zoom].image, NULL,screen, &dest);
				}
				
				// actual tiles
				level = tmp->base;
				while (level != NULL) {
					if (level->image != NULL) {
						dest.x = xoff + x * HalfTileSizes[zoom][0] - (HalfTileSizes[zoom][0] * y) + level->image[zoom].offset_x; // tile_x
						dest.y = yoff + y * HalfTileSizes[zoom][1] + (HalfTileSizes[zoom][1] * x) - (tmp->height * TileOffset[zoom]) + level->image[zoom].offset_y; // tile_y
						SDL_BlitSurface(level->image[zoom].image, NULL,screen, &dest);
					}
					level = level->next;
				}
				
				// Top down view
				dest.x = screen->w - 50 - x * 40;
				dest.y = 10 + y * 40;
				SDL_BlitSurface((tile_to_place == NULL) ? tile_select : filled_tile_select, NULL,screen, &dest);

				// Increment to the next tile
				tmp = tmp->next;
			}
		}

		// helper text
		if (tile_to_place != NULL)
			drawString(screen, font, screen->w - 210, 220, "Select a position");
		else if (SelectedTileExpiry != 0) {
			drawString(screen, font, screen->w - 210, 220, "Q    Increase Height");
			drawString(screen, font, screen->w - 210, 235, "A    Decrease Height");
		} else if (!_display_fl) {
			drawString(screen, font, screen->w - 300, 450, "Spacebar");
			drawString(screen, font, screen->w - 150, 450, "Draw a tile");
			drawString(screen, font, screen->w - 300, 465, "Arrow Keys");
			drawString(screen, font, screen->w - 150, 465, "Move");
			drawString(screen, font, screen->w - 300, 480, "Page Up / Down");
			drawString(screen, font, screen->w - 150, 480, "Zoom");
			drawString(screen, font, screen->w - 300, 495, "W / S");
			drawString(screen, font, screen->w - 150, 495, "Change x axis size");
			drawString(screen, font, screen->w - 300, 510, "E / D");
			drawString(screen, font, screen->w - 150, 510, "Change y axis size");
			drawString(screen, font, screen->w - 300, 525, "P");
			drawString(screen, font, screen->w - 150, 525, "Screenshot");
			drawString(screen, font, screen->w - 300, 540, "C");
			drawString(screen, font, screen->w - 150, 540, "Change colour");
			drawString(screen, font, screen->w - 300, 555, "H");
			drawString(screen, font, screen->w - 150, 555, "Hide / Show Grid");
		}
						
		// list of loaded tiles
		if (_display_fl) {
			y = 10;
			drawString(screen, font, 10, y, "Select a tile");
			for (i = 2; i <= _loaded_tile_images; i++) {
				y += 15;
				drawString(screen, font, 10, y, _tiles_images[i][0].name);
			}
		}
		
		// selected tile
		if (SelectedTileExpiry != 0 && cTime < SelectedTileExpiry)
		{
			SDL_Rect dest;
			dest.x = xoff + SelectedTileX * HalfTileSizes[zoom][0] - (HalfTileSizes[zoom][0] * SelectedTileY);
			dest.y = yoff + SelectedTileY * HalfTileSizes[zoom][1] + (HalfTileSizes[zoom][1] * SelectedTileX) - (SelectedTile->height * TileOffset[zoom]);
			SDL_BlitSurface(_tiles_images[1][zoom].image, NULL, screen, &dest);
		} else if (SelectedTileExpiry != 0 && cTime > SelectedTileExpiry) {
			SelectedTileExpiry = 0;
		}
		
		// show it!
		SDL_Flip(screen);
		SDL_Delay(50);
	}

	return 0;
}
