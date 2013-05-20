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

#include "font.h"
#include "assert.h"

Font g_Font;

Font::Font()
{
	m_nWidth = 16;
	m_nHeight = 16;
	m_nSpacing = 10; 
	m_nNumFontX = 16;
	m_nNumFontY = 16;
	m_pVertices = NULL;
}

Font::~Font()
{
	SafeDeleteArray(m_pVertices);
}

GLvoid Font::CreateASCIIFont()
{
	m_nWidth = 16;
	m_nHeight = 16;
	m_nSpacing = 10; 
	m_nNumFontX = 16;
	m_nNumFontY = 16;
	
	int total = (m_nNumFontX+1)*m_nNumFontY*2*2;
	m_pVertices = new GLshort[total];
	int vidx=0;		// vertex array index
	
	for(short j=0; j<m_nNumFontY; j++)
	{
		GLshort h0 = 256-m_nHeight*j;
		GLshort h1 = 256-m_nHeight*(j+1);

		for(short i=0; i<m_nNumFontX; i++)
		{
			GLshort w0 = m_nWidth*i;
			m_pVertices[vidx++] = w0;
			m_pVertices[vidx++] = h0;
			m_pVertices[vidx++] = w0;
			m_pVertices[vidx++] = h1;
		}

		m_pVertices[vidx++] = 256;
		m_pVertices[vidx++] = h0;
		m_pVertices[vidx++] = 256;
		m_pVertices[vidx++] = h1;
	}

	assert(vidx == total);
	m_Texture.SetImageFile("/sdcard/dflora/font.bmp");
	m_Texture.LoadTexture();
}


void Font::DrawASCIITextQuad(const char *const string, bool italic)
{
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glScaler(FTOR(1.f/256), FTOR(1.f/256), FTOR(1.f/256));

	m_Texture.Apply();
	char c;
	GLshort w=0;
	for(int i=0; c=string[i]; i++)
	{
		int offset = c-' ';
		int y = offset/16;
		if(italic)	y += 8;
		int x = offset%16;
		int first = (y*17+x)<<2;
		
		GLshort vert[8];
		vert[0] = w;
		vert[1] = m_nHeight;
		vert[2] = w;
		vert[3] = 0;
		w += m_nSpacing;
		vert[4] = w;
		vert[5] = m_nHeight;
		vert[6] = w;
		vert[7] = 0;

		GLshort tex[8];
		for(int j=0; j<8; j++)	
			tex[j] = m_pVertices[first+j];
		
		glVertexPointer(2, GL_SHORT, 0, vert);
		glTexCoordPointer(2, GL_SHORT, 0, tex);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

}

void Font::Draw2DStringLine(Real x, Real y, Real width, Real height, const char * const string, bool italic)
{
	GLint matmod = Begin2DDrawing();

	glTranslater(x, y, R_ZERO);
	glScaler(width, height, R_ONE);
	DrawASCIITextQuad(string, italic);

	End2DDrawing(matmod);
}
