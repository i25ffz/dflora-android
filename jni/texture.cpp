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

#include "texture.h"
#include "stdio.h"
#include "image.h"

GLuint Texture::m_CurrentTexName = 0;
GLfloat Texture::m_MappingMode = GL_MODULATE;

Texture::Texture()
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_TextureName);
}

Texture::Texture(const char * imgfn, int mpmpLevel)
{
	m_TextureName = 0;
	strcpy(m_Filename, imgfn);
	m_BadFile = FALSE;
}

void Texture::SetImageFile(const char * imgfn)
{
	m_TextureName = 0;
	strcpy( m_Filename, imgfn);
	m_BadFile = FALSE;
}

void Texture::Apply()
{
	if(m_BadFile)
		return;

	if(m_TextureName)
	{
		if(m_CurrentTexName != m_TextureName)
		{
		    glBindTexture(GL_TEXTURE_2D, m_TextureName);
			m_CurrentTexName = m_TextureName;
		}
	}
	else
	{
		LoadTexture();
		m_CurrentTexName = m_TextureName;
	}
}

bool Texture::LoadTexture()
{
	bool Status=false;

	Image	image;
	if(image.Load(m_Filename))
	{
		Status=true;

		glGenTextures(1, &m_TextureName);
		glBindTexture(GL_TEXTURE_2D, m_TextureName);		
		glTexImage2D(GL_TEXTURE_2D, 
			0, 
			image.GetDataFormat(), 
			image.GetWidth(), 
			image.GetHeight(), 	
			0, 
			image.GetDataFormat(), 
			GL_UNSIGNED_BYTE, 
			image.GetData());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	image.FreeData();
	return Status;
}
