/*
  tiles.h
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

#ifndef __TILES_H__
#define __TILES_H__

#include <SDL/SDL.h>
#include <string>

struct TileIMG
{
	SDL_Surface *image;
	char *name;
	int offset_x, offset_y;
};

class TileLevel
{
	public:
		TileLevel *next;
		TileIMG *image;
};


class TileBase
{
	public:
		TileBase *next;
		TileLevel *base;

		int	height;
		int num;
};

// generic time stuff
int LoadTileSet(char *name);

// linked list stuff
void AddTileToEnd(int height, TileIMG *image);
TileBase *TileFromXY(int x, int y);
void DeleteTilesFromEnd(int num);
void AddTileAfter(int num, int height, TileIMG *image);
void DeleteTileByID(int num);
int CountTiles(void);
void DeleteTile(TileBase *tile);
TileLevel *NewTileLevel(TileIMG *image);
TileLevel *NewTileLevelOnTop(TileBase *tile);

#define MAX_TILE_IMAGES 100
#define ZOOM_LEVELS 3

extern int _loaded_tile_images;
extern TileIMG _tiles_images[MAX_TILE_IMAGES][ZOOM_LEVELS];
extern TileBase *first_ptr;
extern int tiles_x, tiles_y;

#endif
