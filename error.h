/*
  error.h
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

#ifndef __ERRORS_H__
#define __ERRORS_H__

enum ErrorTypes {
	E_WARNING = 0,
	E_ERROR = 1,
	E_FATAL = 2
};

void error(int error, const char *s, ...);

#endif
