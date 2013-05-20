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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

//#include "windows.h"
#include "utility.h"

/** Wrapper class for the OpenGL|ES texture object.
*/
class Texture
{
public :
	Texture();
	Texture(const char * imgfilename, int mipmaplevel = 0); //eg: A 256X256 image MipmapLevel should be 9
	~Texture();

	bool LoadTexture();
	void Apply();
	void SetImageFile(const char * filename);

protected:
	char m_Filename[MAX_PATH];
	GLuint m_TextureName;
	static GLuint m_CurrentTexName;
	static GLfloat m_MappingMode;
	bool	m_BadFile;
};

#endif