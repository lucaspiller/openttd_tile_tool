/*
  tiles.cpp
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

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "tiles.h"
#include "error.h"

int LoadTileSet(char *dir)
{
	char filename[255];
	char name[255];
	char buffer[255];
	FILE *fp;
	int zoom, x, y;
	
	sprintf(filename, "data/tiles/%s/info", dir);
	
	fp = fopen(filename, "r");
	
	if(fp == NULL)
	{
		error(E_ERROR, "Failed to open header `%s`", filename);
		fclose(fp);		
		return 0;
	}
	
	while(!feof(fp))
	{
		char *data_name = new char[255];
		memcpy(data_name, dir, 255);
		
		fgets(buffer, 255, fp);
		if(buffer[0] != '#' && buffer[0] != '\r' && buffer[0] != '\0' && buffer[0] != '\n' && strlen(buffer) != 0)
		{
			SDL_Surface *temp;
			
			sscanf(buffer, "%s Z%d X%d Y%d", name, &zoom, &x, &y);
			sprintf(filename, "data/tiles/%s/%s", dir, name);
			
			temp = IMG_Load(filename);
			if (!temp)
			{
				error(E_ERROR, "Failed to open image `%s`", filename);
				delete data_name;
				fclose(fp);
				return 0;
			}
			
			_tiles_images[_loaded_tile_images][zoom].image = temp;
			_tiles_images[_loaded_tile_images][zoom].offset_x = x;
			_tiles_images[_loaded_tile_images][zoom].offset_y = y;
			_tiles_images[_loaded_tile_images][zoom].name = data_name;
		}
	}
	
	fclose(fp);
	
	return 1;
}

void AddTileToEnd(int height, TileIMG *image)
{
	TileBase *tmp1 = new TileBase, *tmp2;

	tmp1->height = height;
	tmp1->next = NULL;
	tmp1->base = NewTileLevel(image);

	if (first_ptr == NULL) {
		tmp1->num = 0;
		first_ptr = tmp1;
	} else {
		tmp2 = first_ptr;
		while (tmp2->next != NULL)
			tmp2 = tmp2->next;
		tmp2->next = tmp1;
		tmp1->num = tmp2->num + 1;
	}
}

TileBase *TileFromXY(int x, int y)
{
	TileBase *tmp = first_ptr;
	int j, i = (x * tiles_y) + y;

	if (i == 0)
		return tmp;

	for (j = 0; j < i; j++)
	{
		if (tmp->next != NULL)
			tmp = tmp->next;
		else {
			error(E_WARNING, "Tile at (%d, %d) does not exist!", x, y);
			return NULL;
		}
	}

	return tmp;
}

// 0 is first tile
TileBase *TileFromID(int num)
{
	TileBase *tmp = first_ptr;
	int i = 0;

	while(i != num) {
		if (tmp->next != NULL) {
			tmp = tmp->next;
			i++;
		} else {
			error(E_WARNING, "Tile #%d does not exist!", i);
			return NULL;
		}
	}

	return tmp;
}

// num is number from end to delete
void DeleteTilesFromEnd(int num)
{
	TileBase *tmp, *tmp2;

	if (num < 1 || num > tiles_x * tiles_y)
		return;

	// we want the one before the one we are going to delete
	tmp = TileFromID((tiles_x * tiles_y) - num - 1);

	tmp2 = tmp->next;
	tmp->next = NULL;

	while(tmp2 != NULL) {
		tmp = tmp2;
		tmp2 = tmp2->next;
		error(E_WARNING, "Deleting #%d", tmp->num);
		delete tmp;
	}
}

void AddTileAfter(int num, int height, TileIMG *image)
{
	TileBase *tmp1, *tmp2 = new TileBase;
	tmp1 = TileFromID(num);
	
	tmp2->height = height;
	tmp2->next = tmp1->next;
	tmp2->base = NewTileLevel(image);

	tmp1->next = tmp2;
}

TileLevel *NewTileLevelOnTop(TileBase *tile)
{
	TileLevel *tmp1, *tmp2;
	
	tmp1 = tile->base;
	tmp2 = tmp1->next;

	while (tmp2 != NULL) {
		tmp1 = tmp2;
		tmp2 = tmp1->next;
	}

	tmp2 = NewTileLevel(_tiles_images[0]);
	tmp1->next = tmp2;

	return tmp2;
}

TileLevel *NewTileLevel(TileIMG *image)
{
	TileLevel *base = new TileLevel;

	base->image = image;
	base->next = NULL;

	return base;
}

void DeleteTileByID(int num)
{
	TileBase *tmp = TileFromID(num);
	DeleteTile(tmp);
}

void DeleteTile(TileBase *tile)
{
	TileBase *tmp2;

	// first clear it from the linked list
	(TileFromID(tile->num - 1))->next = TileFromID(tile->num + 1);

	// then clear any layered tiles
	tmp2 = tile->next;
	while (tmp2 != NULL)
	{
		tile = tmp2;
		tmp2 = tile->next;
		delete tile;
	}

	delete tmp2;
}

int CountTiles(void)
{
	TileBase *tmp = first_ptr;
	int i = 1;

	while(tmp->next != NULL)
	{
		tmp = tmp->next;
		i++;
	}

	return i;
}
		
