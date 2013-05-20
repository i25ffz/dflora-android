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

#ifndef _SKY_H_
#define _SKY_H_

#include "utility.h"
#include "texture.h"


/** A class to help render sky.
	@remarks
	Support skydoom and skybox.
	@bugs 
	1. multitexture seems not work when the TexEnv set to COMBINE ...
	2. very slow when using multitexture
*/
class Sky
{
public:
	Sky();
	~Sky();
	void Create();
	void CreateSkyDoom(uint8 longitude, uint8 latitude);
	void CreateSkyBox();
	
	void Draw();
	void DrawSkyDoom();
	void DrawSkyBox();
	void DrawMultiTexture();	// multitexture option

protected:
	void DrawHalfSphere();

	uint8			m_nLatitude, m_nLongitude;
	Texture			m_TextureDoom, m_TextureFence;

	// one vertices array, two texture layer: background, foreground, multitexture
	uint32			m_nNumVertices;
	RealVec3 *		m_pVertices;
	RealVec3 *		m_pNormals;
	RealVec2 *		m_pBackgroundTexcoords;
	RealVec2 *		m_pForegroundTexcoords;
	uint32			m_nNumIndices;
	uint16 *		m_pIndices;
};

#endif