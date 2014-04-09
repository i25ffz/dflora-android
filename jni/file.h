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

#ifndef _FILE_H_
#define _FILE_H_

#ifdef ANDROID
#include <android/asset_manager.h>
#else
#include <stdio.h>
#endif // ANDROID

/** A File warpper class support android && other os.
*/
class File
{
private:
#ifdef ANDROID
    AAsset* m_asset;
#else
    FILE* m_file;
#endif // ANDROID

public:
	File(const char * filename);
	size_t read(void *buf, size_t size, size_t count);
	int seek(long offset, int pos);
	long tell();
	int close();
};

#endif
