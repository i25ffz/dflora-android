/*
-----------------------------------------------------------------------------
This source file is part of Dflora (Dancing Flora) OpenGL|ES 1.1 version

Copyright (c) 2004-2005 Changzhi Li < richardzlee@163.com >

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free 
Software Foundation; either version 2 of the License, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
details.

You should have received a copy of the GNU Lesser General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 59 
Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#ifndef _FONT_H_
#define _FONT_H_

#include "utility.h"
#include "texture.h"

/** ASCII font wrapper class, support texture based font rendering.
	@remarks
	This font rendering method is ported from NeHe OpenGL lesson 17 - 2D Texture Font. 
	Because there is no display list in OpenGL|ES, each time when rendering a character,
	we have to create a quad for that character, setting its positions and texcoords.
	@note
	For the sake of saving memery, here the vertex array is the same with the texcoordx
	array (GLshort type) to hold the 256x256 font texture. To properly display the text, 
	glMatrixMode(GL_TEXTURE) and glScale( ) should be used to scale the texture matrix.

	For more infomation visit: http://nehe.gamedev.net
*/
class Font  
{
public:
	Font();
	~Font();

	void CreateASCIIFont();
	void DrawASCIITextQuad(const char * const string, bool italic = false);
	void Draw2DStringLine(Real x, Real y, Real width, Real height, const char * const string, bool italic = false);

protected:
	GLshort		m_nWidth;		// char width	(x coordinate)
	GLshort		m_nHeight;		// char height	(y coordinate)
	GLshort		m_nSpacing;		// char space	(x coordinate)
	GLshort		m_nNumFontX;	// num of font in the texture x coordinate
	GLshort		m_nNumFontY;	// num of font in the texture y coordinate
	GLshort		* m_pVertices;	// vertex array is the same with texcoords array
	Texture		m_Texture;

};

extern Font  g_Font;


#endif
