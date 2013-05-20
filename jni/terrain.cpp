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

#include "terrain.h"

void Terrain::Create()
{
	Real length = ITOR(780);
	Real z = R_ZERO;

	m_Vertices[0][0] = -length;
	m_Vertices[0][1] = -length;
	m_Vertices[0][2] = z;

	m_Vertices[1][0] = length;
	m_Vertices[1][1] = -length;
	m_Vertices[1][2] = z;

	m_Vertices[2][0] = length;
	m_Vertices[2][1] = length;
	m_Vertices[2][2] = z;

	m_Vertices[3][0] = -length;
	m_Vertices[3][1] = length;
	m_Vertices[3][2] = z;

	Real tr = FTOR(10.0);

	m_TexCoords[0][0] = R_ZERO;
	m_TexCoords[0][1] = R_ZERO;

	m_TexCoords[1][0] = tr;
	m_TexCoords[1][1] = R_ZERO;

	m_TexCoords[2][0] = tr;
	m_TexCoords[2][1] = tr;

	m_TexCoords[3][0] = R_ZERO;
	m_TexCoords[3][1] = tr;

	m_Texture.SetImageFile("/sdcard/dflora/SAND.BMP");
}


void Terrain::Draw()
{
	glDisable(GL_LIGHTING);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	m_Texture.Apply();

	glColor4r(FTOR(0.9), FTOR(0.9), FTOR(0.9), FTOR(0.7));
	glTexCoordPointer(2, GL_REAL, 0, m_TexCoords);
	glVertexPointer(3, GL_REAL, 0, m_Vertices);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
