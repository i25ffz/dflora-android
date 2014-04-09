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

#include "file.h"

#ifdef ANDROID
extern AAssetManager* g_assetMgr;
#endif // ANDROID

File::File(const char * filename)
{
#ifdef ANDROID
    m_asset = AAssetManager_open(g_assetMgr, filename, AASSET_MODE_STREAMING);
#else
	m_file = fopen(filename, "rb");
#endif // ANDROID


/*

AAssetDir* assetDir = AAssetManager_openDir(mgr, "");
const char* filename = (const char*)NULL;
while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
    AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
    char buf[BUFSIZ];
    int nb_read = 0;
    FILE* out = fopen(filename, "w");
    while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
        fwrite(buf, nb_read, 1, out);
    fclose(out);
    AAsset_close(asset);
}
AAssetDir_close(assetDir);

*/

}

size_t File::read(void *buf, size_t size, size_t count)
{
#ifdef ANDROID
	if(m_asset == NULL)
        return -1;

    AAsset_read(m_asset, buf, size * count);
    return count;
#else
	if(m_file == NULL)
        return -1;

    return fread(buf, size, count, m_file);
#endif // ANDROID
}

int File::seek(long offset, int pos)
{
#ifdef ANDROID
	if(m_asset == NULL)
        return -1;

    return AAsset_seek(m_asset, offset, pos);
#else
	if(m_file == NULL)
        return -1;

    return fseek(m_file, offset, pos);
#endif // ANDROID
}

long File::tell()
{
#ifdef ANDROID
	if(m_asset == NULL)
        return -1;

    return AAsset_getLength(m_asset);
#else
	if(m_file == NULL)
        return -1;

    return ftell(m_file);
#endif // ANDROID
}

int File::close()
{
#ifdef ANDROID
    AAsset_close(m_asset);
    return 0;
#else
	if(m_file == NULL)
        return -1;

    return fclose(m_file);
#endif // ANDROID
}
