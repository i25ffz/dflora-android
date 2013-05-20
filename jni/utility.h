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

/** This file provides arithmetic utility functions for real number, vector, 
	quaternion and matrix operations.
	@remarks
	This file defines Real type by typedef method, which is corresponding with
	mathmatical real number. A Real may be represented either as float point or
	fixed point data type in the computer. Thanks for the Real type, the code is 
	highly portable on platforms no matter if they have FPU or not.
	@par
	This file wrap multiplication, division, sqrt, and sin/cos operations on real
	number (fixed or float point). All the algebraic functioins are based on real
	number, so they can accept both fixed and float numbers, vectors and matrices
	as the parameters simply via switching a macro definition FIXED_REAL. This idea
	is useful even when the mathmatical real number is represented in computer as
	double on some platform that support double precision arithmetic operations.


*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GLES/gl.h>
#include "glwrapfix.h"
#include "timer.h"

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#ifdef ANDROID
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

#ifdef FIXED_REAL
typedef GLfixed		Real;
#define GL_REAL		GL_FIXED
#define ITOR(x)		ITOQ16(x)
#define FTOR(x)		FTOQ16(x)
#define R_ONE		Q_ONE
#define R_HALF		Q_HALF
#define R_ZERO		Q_ZERO
#define R_QUATPI	Q_QUATPI
#define R_HALFPI	Q_HALFPI
#define R_PI		Q_PI
#define R_2PI		Q_2PI
#define R_R2PI		Q_R2PI
#else	// FLOAT_REAL
typedef GLfloat		Real;
#define GL_REAL		GL_FLOAT
#define ITOR(x)		((GLfloat)x)
#define FTOR(x)		((GLfloat)x)
#define R_ONE		((GLfloat)1.f)
#define R_HALF		((GLfloat).5f)
#define R_ZERO		((GLfloat)0.f)
#define R_QUATPI	(3.1415926535f*0.25f)
#define R_HALFPI	(3.1415926535f*0.5f)
#define R_PI		3.1415926535f
#define R_2PI		6.2831853072f
#define R_R2PI		(1.0f/6.28318530717958647692f)
#endif

#define S15Q16_FACTOR	16
#define S15Q16_ONE		(1 << S15Q16_FACTOR)	// representation of 1
#define S15Q16_ZERO		0						// representation of 0
#define S15Q16_HALF		0x08000					// S15.16 0.5 
#define S15Q16_PINF		0x7fffffff				// +inf 
#define S15Q16_MINF		0x80000000				// -inf 

#define ITOQ16(x) ((x)<<S15Q16_FACTOR)
#define Q16TOI(x) ((x)>>S15Q16_FACTOR)
#define FTOQ16(f) ((int)((float)(f) * (float)S15Q16_ONE + ((f) > 0 ? 0.5 : -0.5)))

#define S15Q16_QUATPI	FTOQ16(3.1415926535f*0.25f)
#define S15Q16_HALFPI	FTOQ16(3.1415926535f*0.5f)
#define S15Q16_PI		FTOQ16(3.1415926535f)
#define S15Q16_2PI		FTOQ16(6.28318530717958647692f)
#define S15Q16_R2PI		FTOQ16(1.0f/6.28318530717958647692f)

#define ONLY_SUPPORT_S15Q16

#ifdef ONLY_SUPPORT_S15Q16
#define Q_FACTOR		S15Q16_FACTOR
#define Q_ZERO			S15Q16_ZERO
#define Q_ONE			S15Q16_ONE
#define Q_HALF			S15Q16_HALF
#define Q_QUATPI		S15Q16_QUATPI
#define Q_HALFPI		S15Q16_HALFPI
#define Q_PI			S15Q16_PI
#define Q_2PI			S15Q16_2PI
#define Q_R2PI			S15Q16_R2PI
#endif

// type definition
#ifdef WIN32
typedef char int8;
typedef unsigned char uint8;
typedef short int int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
#endif

typedef GLfixed FixedVec2[2];
typedef GLfixed FixedVec3[3];
typedef GLfixed FixedVec4[4];
typedef GLfixed FixedQuat4[4];
typedef GLfixed FixedMat4[16];

typedef Real RealVec2[2];
typedef Real RealVec3[3];
typedef Real RealVec4[4];
typedef Real RealQuat4[4];	// in the order of [0-3] <-> [w, z, y, z]
typedef Real RealMat4[16];

// maths utility functions
Real		Rand01();
Real		Sqrt( Real val );
Real		Sin( Real a );
Real		Cos( Real a );
Real		Product( Real x, Real y );
Real		Inverse( Real x );
Real		Quotient( Real numerator, Real denominator);

Real		Interpolate( Real x0, Real x1, Real alpha );
void		ScaleVec3( RealVec3 vec, Real s );
void		ScaleVec3ByVec3( RealVec3 vec, RealVec3 s );
void		TranslateVec3ByVec3( RealVec3 vec, RealVec3 translate );
void		ClipVec3( RealVec3 vec, Real minimun, Real maximun );
Real		LengthVec3( const RealVec3 a );
Real		NormalizeVec3( RealVec3 a );
Real		UnifyVec3( const RealVec3 in, RealVec3 out );
Real		DotProductVec3( const RealVec3 v1, const RealVec3 v2 );
void		CrossProductVec3( RealVec3 n, const RealVec3 a, const RealVec3 b );

// all the quaternion operation are assumed to be unit
void		LoadIdentityQuat4( RealQuat4 q );
void		InverseQuat4( RealQuat4 q );	
void		PostMultiplyQuat4( const RealQuat4 qin1, const RealQuat4 qin2, RealQuat4 qout );
void		SetQuat4FromRotationMat4( RealQuat4 q, const RealMat4 m );
void		SetQuat4FromAngleAxis( RealQuat4 q, Real angle, const RealVec3 axis );
void		SetQuat4FromAngleDisp( RealQuat4 q, const RealVec3 disp );
void		RotateVec3ByQuat4( const RealVec3 vin, const RealQuat4 q, RealVec3 vout );
void		SetRotationMat4FromQuat4( RealMat4 m, const RealQuat4 q );

void		LoadIdentityMat4( RealMat4 m );
void		CopyMat4( RealMat4 from, RealMat4 to );

// m = m*f
void		PostMultiplyMat4( RealMat4 m, const RealMat4 f );

void		ScaleMat4ByVec3( RealMat4 m, const RealVec3 s );
void		SetRotationMat4FromDispVec3( RealMat4 m, const RealVec3 disp);
void		SetRotationMat4FromAngleAxisVec3( RealMat4 m, Real angle, const RealVec3 axis );
void		LookAtMatrix( RealMat4 m, Real ex, Real ey, Real ez, Real atx, Real aty, Real atz, Real ux, Real uy, Real uz );

// out = m*in
void		TransformVec3ByMat4( const RealVec3 vin, const RealMat4 m, RealVec3 vout);
void		RotateVec3ByMat4( const RealVec3 vin, const RealMat4 m, RealVec3 vout);

bool		ProjectVec3ToViewport( const RealVec3 vin, const RealMat4 viewMat, 
						   const RealMat4 projMat, const GLint viewport[4], 
						   RealVec3 vout );

#define  SafeDelete(p) {if(p) {delete p; p=NULL;}}			// for shorter code
#define  SafeDeleteArray(p) {if(p) {delete[] p; p=NULL;}}	// for shorter code

// 2D drawing auxiliary functions

GLint Begin2DDrawing (void);
void End2DDrawing (GLint originalMatrixMode);

#endif // _UTILITY_H_
