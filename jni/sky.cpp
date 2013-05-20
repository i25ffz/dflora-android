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

#include "assert.h"
#include "sky.h"

Sky::Sky()  
{
	m_nLongitude        = 0;
	m_nLatitude			= 0;
	m_nNumVertices      = 0;
	m_pVertices         = NULL;
	m_pNormals          = NULL;
	m_pBackgroundTexcoords = NULL;
	m_pForegroundTexcoords = NULL;
	m_nNumIndices       = 0;
	m_pIndices          = NULL;
}

Sky::~Sky()
{
	SafeDeleteArray(m_pVertices);
	SafeDeleteArray(m_pNormals);
	SafeDeleteArray(m_pBackgroundTexcoords);
	SafeDeleteArray(m_pForegroundTexcoords);
	SafeDeleteArray(m_pIndices);
}


void Sky::Create()
{
	m_nLatitude = 9;
	m_nLongitude = 12;
	CreateSkyDoom(m_nLongitude, m_nLatitude);
//	CreateSkyBox();
}

void Sky::CreateSkyBox()
{
	m_TextureFence.SetImageFile("/sdcard/dflora/skyfence.tga");

	m_nNumVertices = 10;
	m_pVertices = new RealVec3[m_nNumVertices];
	
	m_pVertices[0][0] = FTOR(-0.5);
	m_pVertices[0][1] = FTOR(-0.5);
	m_pVertices[0][2] = R_ZERO;

	m_pVertices[1][0] = FTOR(-0.5);
	m_pVertices[1][1] = FTOR(-0.5);
	m_pVertices[1][2] = FTOR(1.0);

	m_pVertices[2][0] = FTOR(-0.5);
	m_pVertices[2][1] = FTOR(0.5);
	m_pVertices[2][2] = R_ZERO;

	m_pVertices[3][0] = FTOR(-0.5);
	m_pVertices[3][1] = FTOR(0.5);
	m_pVertices[3][2] = FTOR(1.0);

	m_pVertices[4][0] = FTOR(0.5);
	m_pVertices[4][1] = FTOR(0.5);
	m_pVertices[4][2] = R_ZERO;

	m_pVertices[5][0] = FTOR(0.5);
	m_pVertices[5][1] = FTOR(0.5);
	m_pVertices[5][2] = FTOR(1.0);

	m_pVertices[6][0] = FTOR(0.5);
	m_pVertices[6][1] = FTOR(-0.5);
	m_pVertices[6][2] = R_ZERO;

	m_pVertices[7][0] = FTOR(0.5);
	m_pVertices[7][1] = FTOR(-0.5);
	m_pVertices[7][2] = FTOR(1.0);

	m_pVertices[8][0] = FTOR(-0.5);
	m_pVertices[8][1] = FTOR(-0.5);
	m_pVertices[8][2] = R_ZERO;

	m_pVertices[9][0] = FTOR(-0.5);
	m_pVertices[9][1] = FTOR(-0.5);
	m_pVertices[9][2] = FTOR(1.0);

	m_pBackgroundTexcoords = new RealVec2[m_nNumVertices];
	m_pBackgroundTexcoords[0][0] = FTOR(0);
	m_pBackgroundTexcoords[0][1] = FTOR(0);

	m_pBackgroundTexcoords[1][0] = FTOR(0);
	m_pBackgroundTexcoords[1][1] = FTOR(1);

	m_pBackgroundTexcoords[2][0] = FTOR(0.25);
	m_pBackgroundTexcoords[2][1] = FTOR(0);

	m_pBackgroundTexcoords[3][0] = FTOR(0.25);
	m_pBackgroundTexcoords[3][1] = FTOR(1);

	m_pBackgroundTexcoords[4][0] = FTOR(0.5);
	m_pBackgroundTexcoords[4][1] = FTOR(0);

	m_pBackgroundTexcoords[5][0] = FTOR(0.5);
	m_pBackgroundTexcoords[5][1] = FTOR(1);

	m_pBackgroundTexcoords[6][0] = FTOR(0.75);
	m_pBackgroundTexcoords[6][1] = FTOR(0);

	m_pBackgroundTexcoords[7][0] = FTOR(0.75);
	m_pBackgroundTexcoords[7][1] = FTOR(1);

	m_pBackgroundTexcoords[8][0] = FTOR(1);
	m_pBackgroundTexcoords[8][1] = FTOR(0);

	m_pBackgroundTexcoords[9][0] = FTOR(1);
	m_pBackgroundTexcoords[9][1] = FTOR(1);

}

void Sky::CreateSkyDoom(uint8 longitude, uint8 latitude)	// half sphere
{
	m_TextureFence.SetImageFile("/sdcard/dflora/skyfence.tga");
	m_TextureDoom.SetImageFile("/sdcard/dflora/skydoom.tga");


	m_nLatitude = latitude>2 ? latitude : 2;
	m_nLongitude = longitude>2 ? longitude : 2;

	Real R = FTOR(2.5);		// texture duplication = 2*R
	Real longitudeinc2PI = Quotient(R_2PI, ITOR(m_nLongitude));
	unsigned int vindex=0;
	uint8 i, j;

	m_TextureDoom.LoadTexture();
	m_TextureFence.LoadTexture();

	// vertices
	m_nNumVertices = m_nLatitude*(m_nLongitude+1);
	m_pVertices = new RealVec3[m_nNumVertices];
	m_pBackgroundTexcoords = new RealVec2[m_nNumVertices];
	m_pForegroundTexcoords = new RealVec2[m_nNumVertices];

	Real Rjinc = Quotient(R, ITOR(m_nLatitude-1));
	Real latinc = Quotient(R_HALFPI, ITOR(m_nLatitude-1));

	int fencetex1 = m_nLatitude/4;
	Real fencetex_lat = FTOR(1.0);
	Real fencetex_latinc = Quotient(R_ONE-fencetex_lat, ITOR(m_nLatitude-1-fencetex1));
	Real tex_latinc = Quotient(fencetex_lat, ITOR(m_nLatitude-1-fencetex1));
	Real tex_loninc = Quotient(R_ONE, ITOR(m_nLongitude));
		
	Real lat = R_ZERO;
	Real Rj = R_ZERO;

	for(j=0; j<m_nLatitude; j++)
	{	
		Real cos_a = Cos(lat);
		Real sin_a = Sin(lat);
		Real angle = R_ZERO;
		Real tex_x, tex_y;
		Real tex_lon = R_ZERO;

		for(i=0; i<=m_nLongitude; i++)
		{
			Real x = Cos(angle);
			Real y = Sin(angle);
			tex_x = Product(Rj, Cos(angle));
			tex_y = Product(Rj, Sin(angle));

			tex_x += R;
			tex_y += R;

			m_pBackgroundTexcoords[vindex][0] = tex_lon;
			if(j>fencetex1)
				m_pBackgroundTexcoords[vindex][1] = fencetex_lat-Product(ITOR(j-fencetex1), tex_latinc);
			else
				m_pBackgroundTexcoords[vindex][1] = R_ONE-Product(ITOR(j), fencetex_latinc);

			m_pForegroundTexcoords[vindex][0] = tex_x;
			m_pForegroundTexcoords[vindex][1] = tex_y;

			m_pVertices[vindex][0] = Product(x, sin_a);
			m_pVertices[vindex][1] = Product(y, sin_a);
			m_pVertices[vindex][2] = cos_a;

			vindex++;

			angle += longitudeinc2PI;
			tex_lon += tex_loninc;
		}
		Rj += Rjinc;
		lat+= latinc;
	}
	assert(vindex == m_nNumVertices);

	// indices
	uint32 slideri=0;
	m_nNumIndices = 2*(m_nLatitude-1)*(m_nLongitude+1);
	m_pIndices = new uint16[m_nNumIndices];

	// tiangle strip
	uint16 lup = m_nLongitude+1;
	uint16 ldown = 0;
	for(j=0; j<m_nLatitude-1; j++)
	{
		for(i=0; i<=m_nLongitude; i++)
		{
			m_pIndices[slideri++] = lup+i;
			m_pIndices[slideri++] = ldown+i;
		}
		lup += m_nLongitude+1;
		ldown += m_nLongitude+1;
	}
	assert(slideri == m_nNumIndices);
}

void Sky::Draw()
{
	DrawSkyDoom();
//	DrawSkyBox();
//	DrawMultiTexture();
}

void Sky::DrawSkyDoom()
{
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_REAL, 0, m_pVertices);

	// background
	m_TextureFence.Apply();
	glTexCoordPointer(2, GL_REAL, 0, m_pBackgroundTexcoords);

	{
		uint8 i=0;
		uint32 inc = m_nLongitude+1;
		uint32 count = inc<<1;
		uint32 idx = count*i;

		for(; i<m_nLatitude-1; i++)
		{
			glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, m_pIndices+idx);
			idx += count;
		}
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Sky::DrawSkyBox()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_REAL, 0, m_pVertices);

	// background
	m_TextureFence.Apply();
	glTexCoordPointer(2, GL_REAL, 0, m_pBackgroundTexcoords);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Sky::DrawMultiTexture()
{
	glDisable(GL_LIGHTING);

	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	m_TextureFence.Apply();
	glTexCoordPointer(2, GL_REAL, 0, &(m_pBackgroundTexcoords[0][0]));

	Real envcolor[4] = {R_ONE, R_ONE, R_ONE, R_ONE};
	glTexEnvrv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envcolor);

	glTexEnvr(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glActiveTexture(GL_TEXTURE1);
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	m_TextureDoom.Apply();
	glTexCoordPointer(2, GL_REAL, 0, &(m_pForegroundTexcoords[0][0]));

//	glTexEnvr(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//	glTexEnvr(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);	// !! seems wrong !!
	//glTexEnvr(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_CONSTANT);
	glTexEnvr(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	glTexEnvr(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
	
	//glTexEnvr(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_CONSTANT);
	glTexEnvr(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
//	glTexEnvr(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

	static Real offset = R_ZERO;
	offset += FTOR(0.002f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_REAL, 0, &(m_pVertices[0][0]));

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslater(FTOR(0.f), offset, FTOR(0.f));
	glMatrixMode(GL_MODELVIEW);

	DrawHalfSphere();
	glDisableClientState(GL_VERTEX_ARRAY);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);


	glActiveTexture(GL_TEXTURE1);
	glClientActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexEnvr(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexEnvr(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glEnable(GL_LIGHTING);
}

void Sky::DrawHalfSphere()
{
	uint32 idx = 0;
	uint32 inc = m_nLongitude+1;
	uint32 count = inc<<1;

	for(uint8 i=0; i<m_nLatitude-1; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, m_pIndices+idx);
		idx += count;
	}
}