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

#include "stdio.h"
//#include "windows.h"
#include "image.h"
#include "utility.h"
#include "file.h"

struct rgb_t
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
};


// 32-bit color type
struct rgba_t
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};


// packed pixel formats
struct rgb565_t
{
	GLushort
		b : 5,
		g : 6,
		r : 5;
};

// 16-bit color with alpha type
struct rgba5551_t
{
	GLushort 
		a : 1,
		b : 5,
		g : 5,
		r : 5;
};

// 16-bit color with alpha type
struct rgba4444_t
{
	GLushort
		a : 4,
		b : 4,
		g : 4,
		r : 4;
};

struct la_t
{
	GLubyte l;
	GLubyte a;
};


// magic number identifies a bmp file; actually chars 'B''M'
// allowed type magics are 'BM'  for Windows;  
// OS2 allows:
//'BA' - Bitmap Array
//'CI' - Color Icon
//'CP' - Color Pointer (mouse cursor)
//'IC' - Icon
//'PT' - Pointer (mouse cursor)

enum BMPMagic 
{
	MB = 19778
}; 


enum BMPTypes
{
	BMP_NODATA		= 0,
	BMP_BLACKWHIET	= 1,
	BMP_GRAYSCALE	= 2,
	BMP_IA			= 3, 
	BMP_RGB			= 4, 
	BMP_RGBA		= 5,
};

// BMP format bits - at start of file is 512 bytes of pure garbage

struct bmpheader_t		// 12 byte
{
    short fileType;				// always MB
    unsigned short size[2];		// a dword for whole file size - make unsigned Feb 2002
    short reserved1;			// reserved for future purposes
	short reserved2;			// reserved for future purposes
    unsigned short offset[2];	// offset to image in bytes
//	long  offset;
};

struct bmpinfo_t 
{
    long width;			// width of the image in pixels
    long height;		// height of the image in pixels
    short planes;       // word: number of planes (always 1)
    short colorbits;    // word: number of bits used to describe color in each pixel
    long compression;	// compression used
    long imageSize;		// image size in bytes
    long XpixPerMeter;	// pixels per meter in X
    long YpixPerMeter;	// pixels per meter in Y
    long colorUsed;		// number of colors used
    long important;		// number of "important" colors
};	// 36 byte


struct tgaheader_t
{
	GLubyte   idLength;
	GLubyte   colorMapType;
	GLubyte   imageType;
	GLubyte   colorMapSpec[5];
	GLushort  xOrigin;
	GLushort  yOrigin;
	GLushort  width;
	GLushort  height;
	GLubyte   bpp;
	GLubyte   imageDesc;
};


enum TGATypes
{
	TGA_NODATA = 0,
	TGA_INDEXED = 1,
	TGA_RGB = 2,
	TGA_GRAYSCALE = 3,
	TGA_INDEXED_RLE = 9,
	TGA_RGB_RLE = 10,
	TGA_GRAYSCALE_RLE = 11

};

static int GetPadding(int width, int bpp)
{
	return (((width * bpp + 31) & 0xFFFFFFE0) - width * bpp) / 8;
}

Image::Image() : m_pData(NULL)
{
}


Image::~Image()
{
	FreeData();

}

void Image::FreeData()
{
	SafeDeleteArray(m_pData);
}

bool Image::Flip()
{
	
	if (!m_pData)
		return false;

	rgba_t* tmpBits = new rgba_t[m_width];
	if (!tmpBits)
		return false;

	int lineWidth = m_width * 4;

	rgba_t* top = (rgba_t*)m_pData;
	rgba_t* bottom = (rgba_t*)(m_pData + lineWidth*(m_height-1));

	for (int i = 0; i < (m_height / 2); ++i)
	{
		memcpy(tmpBits, top, lineWidth); 
		memcpy(top, bottom, lineWidth);
		memcpy(bottom, tmpBits, lineWidth);

		top = (rgba_t*)((GLubyte*)top + lineWidth);
		bottom = (rgba_t* )((GLubyte*)bottom - lineWidth);
	}

	SafeDeleteArray(tmpBits);
	tmpBits = 0;

	return true; 

}


void Image::SwapBlueAndRed()
{
	switch (m_colorDepth)
	{
		case 32:
		{
			GLubyte temp;
			int total = m_width * m_height;
			rgba_t* source = (rgba_t*)m_pData;

			for (int pixel = 0; pixel < total; ++pixel)
			{
				temp = source[pixel].b;
				source[pixel].b = source[pixel].r;
				source[pixel].r = temp;
			}
		} break;
		case 24:
		{
			GLubyte temp;
			int total = m_width * m_height;
			rgb_t* source = (rgb_t*)m_pData;

			for (int pixel = 0; pixel < total; ++pixel)
			{
				temp = source[pixel].b;
				source[pixel].b = source[pixel].r;
				source[pixel].r = temp;
			}
		} break;
		default:
		// ignore other color depths
		break;
	}

}

bool Image::Load(const char * filename)
{
	size_t l = strlen(filename);
	if(!strnicmp(filename+(l-3), "bmp", 3))
		return LoadBMP(filename);
	if(!strnicmp(filename+(l-3), "tga", 3))
		return LoadTGA(filename);
	return false;
}

bool Image::LoadBMP(const char * filename)	// not work properly for 16bit and 8bit/256 color
{
	File file(filename);

    file.seek(0, SEEK_END);
    size_t filelen = file.tell();			// determine file size so we can fill it in later if FileSize == 0
    file.seek(0, SEEK_SET);

    int ncolours=0;
    int ncomp=0;

    bmpheader_t hd;
    bmpinfo_t	inf;
    file.read((char *)&hd, sizeof(bmpheader_t), 1);
    if (hd.fileType == MB)
	{
        long infsize;									// size of BMPinfo in bytes
        unsigned char *imbuff;							// image buffer
        file.read((char *)&infsize, sizeof(long), 1);	// insert inside 'the file is bmp' clause
        unsigned char * hdr=new unsigned char[infsize];	// to hold the new header

        file.read((char *)hdr, 1, infsize-sizeof(long));
        long hsiz = sizeof(inf);							// minimum of structure size &
        if(infsize <= hsiz) hsiz=infsize;
        memcpy(&inf, hdr, hsiz);							// copy only the bytes I can cope with
		ncolours=inf.colorbits/8;

        SafeDeleteArray(hdr);
        long size = hd.size[1]*65536+hd.size[0];

        // handle size==0 in uncompressed 24-bit BMPs
        if (size == 0) size = filelen;

        if (inf.imageSize == 0)
			inf.imageSize=inf.width*inf.height*ncolours;

        imbuff = new unsigned char [inf.imageSize];		// read from disk
		file.seek(hd.offset[1]*65536+hd.offset[0], SEEK_SET);
        file.read((char *)imbuff, sizeof(unsigned char), inf.imageSize);

        switch (ncolours) 
		{
		case 0:
			ncomp = BMP_BLACKWHIET;
			inf.colorbits = 1;

        case 1:
            ncomp = BMP_GRAYSCALE;					// actually this is a 256 colour, paletted image
            inf.colorbits = 8;						// so this is how many bits there are per index
													// inf.ColorUsed=256; // and number of colours used
            break;
        case 2:
            ncomp = BMP_IA;
            break;
        case 3:
            ncomp = BMP_RGB;
            break;
        case 4:
            ncomp = BMP_RGBA;
            break;
        default:
			break;
        }

		m_pData = new unsigned char [ncolours * inf.width * inf.height]; // to be returned
        
        unsigned long off=0;
        unsigned long rowbytes=ncolours * sizeof(unsigned char) * inf.width;
        unsigned long doff=rowbytes/4;
        if (rowbytes%4)		doff++;						// round up if needed
        doff*=4;										// to find dword alignment
        for(int j=0; j<inf.height; j++) 
		{
            if (ncomp>=1) 
				memcpy(m_pData+j*rowbytes, imbuff+off, rowbytes); // pack bytes closely
            off+=doff;
        }
        SafeDeleteArray(imbuff); // free the on-disk storage

        file.close();
    }
    else // else error in header
    {
        file.close();
        return false;
    }
    m_width = inf.width;
    m_height = inf.height;
    switch (ncomp)
	{
    case BMP_GRAYSCALE:
        m_dataFormat = GL_LUMINANCE;
		m_dataType = GL_UNSIGNED_BYTE;
		m_colorDepth = 8;
        break;	
    case BMP_IA:
        m_dataFormat = GL_LUMINANCE_ALPHA;
		m_dataType = GL_UNSIGNED_BYTE;
		m_colorDepth = 16;	// ??
        break;
    case BMP_RGB:
        m_dataFormat = GL_RGB;
		m_dataType = GL_UNSIGNED_BYTE;
		m_colorDepth = 24;
        break;
    case BMP_RGBA:
        m_dataFormat = GL_RGBA;
		m_dataType = GL_UNSIGNED_BYTE;
		m_colorDepth = 32;
        break;
    default:
        m_dataFormat = GL_RGB;
		m_dataType = GL_UNSIGNED_BYTE;
		m_colorDepth = 24;
        break;
    }
	SwapBlueAndRed();
    return true;
}

bool Image::LoadTGA(const char* filename)
{
	File file(filename);

	// read in the image type
	tgaheader_t tga;		// TGA header

	file.read(&tga, sizeof(tgaheader_t), 1);

	// see if the type is one that we support
	if ((  (tga.imageType != TGA_RGB)
		&& (tga.imageType != TGA_GRAYSCALE)
		&& (tga.imageType != TGA_RGB_RLE)
		&& (tga.imageType != TGA_GRAYSCALE_RLE) )
		|| (tga.colorMapType != 0) )
	{
		file.close();
		return NULL;
	}

	// store texture information
	m_width = tga.width;
	m_height = tga.height;

	// colormode -> 3 = BGR, 4 = BGRA
	int colorMode = tga.bpp / 8;

	// won't handle < 24 bpp for now
	if (colorMode < 3)
	{
		file.close();
		return NULL;
	}

	m_imageSize = m_width * m_height * colorMode;

	// allocate memory for TGA image data
	m_pData = new GLubyte[m_imageSize];

	// read image data
	if (tga.imageType == TGA_RGB || tga.imageType == TGA_GRAYSCALE)
	{
		file.read(m_pData, sizeof(GLubyte), m_imageSize);
	}
	else // must be RLE compressed
	{
		GLubyte id;
		GLubyte length;
		rgba_t color = { 0, 0, 0, 0 };
		GLuint i = 0;

		while(i < m_imageSize)
		{

			file.read(&id, sizeof(char), 1);

			// see if this is run length data
			if(id & 0x80)
			{
				// find the run length
				length = (GLubyte)(id - 127);

				// next 3 (or 4) bytes are the repeated values

				file.read(&color.b, sizeof(char), 1);
				file.read(&color.g, sizeof(char), 1);
				file.read(&color.r, sizeof(char), 1);

				if(colorMode == 4)
				{
					file.read(&color.a, sizeof(char), 1);
				}

				// save everything in this run
				while(length > 0)
				{
					m_pData[i++] = color.b;
					m_pData[i++] = color.g;
					m_pData[i++] = color.r;
					if(colorMode == 4)
					{
						m_pData[i++] = color.a;
					}

					--length;
				}
			}
			else
			{
				// the number of non RLE pixels
				length = GLubyte(id + 1);

				while (length > 0)
				{

					file.read(&color.b, sizeof(char), 1);
					file.read(&color.g, sizeof(char), 1);
					file.read(&color.r, sizeof(char), 1);

					if(colorMode == 4)
					{
						file.read(&color.a, sizeof(char), 1);
					}
					m_pData[i++] = color.b;
					m_pData[i++] = color.g;
					m_pData[i++] = color.r;
					if(colorMode == 4)
					{
						m_pData[i++] = color.a;
					}

					--length;
				}
			}
		}
	}

	file.close();

	switch(tga.imageType)
	{
		case TGA_RGB:
		case TGA_RGB_RLE:
			if (3 == colorMode)
			{
				m_dataFormat = GL_RGB;
				m_dataType = GL_UNSIGNED_BYTE;
				m_colorDepth = 24;
			}
			else
			{
				m_dataFormat = GL_RGBA;
				m_dataType = GL_UNSIGNED_BYTE;
				m_colorDepth = 32;
			}
			break;
		case TGA_GRAYSCALE:
		case TGA_GRAYSCALE_RLE:
			m_dataFormat = GL_LUMINANCE;
			m_dataType = GL_UNSIGNED_BYTE;
			m_colorDepth = 8;
			break;
	}
	SwapBlueAndRed();

	return (m_pData != NULL);
}

/*
bool Image::SaveTGA(const char* filename)
{
	FILE* pFile;

	pFile = fopen(filename, "wb");
	if (!pFile)	return false;

	// read in the image type
	tgaheader_t tga;		// TGA header
	tga.bpp = m_colorDepth;
	tga.colorMapSpec[0] = tga.colorMapSpec[1] = tga.colorMapSpec[2] = tga.colorMapSpec[3] = tga.colorMapSpec[4] = 0;
	tga.colorMapType = 0;
	tga.height = m_height;
	tga.width = m_width;
	tga.idLength = 0;
	tga.imageDesc = 0;
	tga.imageType = TGA_RGB;
	tga.xOrigin = 0;
	tga.yOrigin = 0;

	SwapBlueAndRed();
	fwrite(&tga, sizeof(tgaheader_t), 1);
	fwrite(m_pData, sizeof(GLubyte), m_imageSize);
	if (pFile) fclose(pFile);
	return true;
}
*/
