/*
  error.h
  OpenTTD 32bpp Tile Tool
  Made by Luca Spiller 2005 (ok, was based upon the strgen code, that was based upon that in ScummVM)
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
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"

const char* ErrorString[3] = {
	"Warning",
	"Error",
	"Fatal",
};

void error(int error, const char *s, ...)
{
	char buf[1024];
	
	va_list va;
	va_start(va, s);
	vsprintf(buf, s, va);
	va_end(va);
	
	fprintf(stderr, "%s: %s\n", ErrorString[error], buf);
	
	if (error == E_FATAL)
		exit(1);
}
