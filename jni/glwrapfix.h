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

/**	This file unifies two versions (fixed/float) of glFunctions in a single wrapper.
	@remarks
	To wrap the "glFunctionx" and "glFunctionf" with "glFunctionr" gives the possiblity
	of writing portable OpenGL|ES code platforms supporting either float point or fixed
	point real number operations.
	@par
	This idea is also useful to OpenGL two versions (float/double) of glFunctions (glFunctionf
	and glFunctiond).

*/

#ifndef __gl_wrap_fix_h_
#define __gl_wrap_fix_h_

#include <GLES/gl.h>

// gl functions wrapper
#ifdef FIXED_REAL
#define glAlphaFuncr		glAlphaFuncx
#define glTranslater		glTranslatex
#define glClearColorr		glClearColorx
#define glClearDepthr		glClearDepthx
#define glColor4r			glColor4x
#define glDepthRanger		glDepthRangex
#define glFogr				glFogx
#define glFogrv				glFogxv
#define glFrustumr			glFrustumx
#define glLightr			glLightx
#define glLightrv			glLightxv
#define glLightModelr		glLightModelx
#define glLightModelrv		glLightModelxv
#define glLineWidthr		glLineWidthx
#define glLoadMatrixr		glLoadMatrixx
#define glMaterialr			glMaterialx
#define glMaterialrv		glMaterialxv
#define glMultiTexCoord4r	glMultiTexCoord4x
#define glMultMatrixr		glMultMatrixx
#define glNormal3r			glNormal3x
#define glOrthor			glOrthox
#define glPointSizer		glPointSizex
#define glPolygonOffsetr	glPolygonOffsetx
#define glRotater			glRotatex
#define glSampleCoverager	glSampleCoveragex
#define glScaler			glScalex
#define glTexEnvr			glTexEnvx
#define glTexEnvrv			glTexEnvxv
#define glTexParameterr		glTexParameterx
#define glTranslater		glTranslatex

// gles 1.1
#define glGetClipPlaner		glGetClipPlanex
#define glGetRealv			glGetFixedv
#define glGetLightrv		glGetLightxv
#define glGetMaterialrv		glGetMaterialxv
#define glGetTexEnvrv		glGetTexEnvxv
#define glGetTexParameterrv	glGetTexParameterxv
#define glClipPlaner		glClipPlanex
#define glPointParameterrv	glPointParameterxv
#define glPointParameterr	glPointParameterx
#define glDrawTexrOES		glDrawTexxOES
#define glDrawTexrvOES		glDrawTexxvOES
#define glTexParameterrv	glTexParameterxv

#else	// FLOAT_REAL

#define glAlphaFuncr		glAlphaFunc
#define glTranslater		glTranslatef
#define glClearColorr		glClearColor
#define glClearDepthr		glClearDepthf
#define glColor4r			glColor4f
#define glDepthRanger		glDepthRangex
#define glFogr				glFogf
#define glFogrv				glFogfv
#define glFrustumr			glFrustumf
#define glLightr			glLightf
#define glLightrv			glLightfv
#define glLightModelr		glLightModelf
#define glLightModelrv		glLightModelfv
#define glLineWidthr		glLineWidth
#define glLoadMatrixr		glLoadMatrixf
#define glMaterialr			glMaterialf
#define glMaterialrv		glMaterialfv
#define glMultiTexCoord4r	glMultiTexCoord4f
#define glMultMatrixr		glMultMatrixf
#define glNormal3r			glNormal3f
#define glOrthor			glOrthof
#define glPointSizer		glPointSize
#define glPolygonOffsetr	glPolygonOffset
#define glRotater			glRotatef
#define glSampleCoverager	glSampleCoverage
#define glScaler			glScalef
#define glTexEnvr			glTexEnvf
#define glTexEnvrv			glTexEnvfv
#define glTexParameterr		glTexParameterf
#define glTranslater		glTranslatef

// gles 1.1
#define glGetClipPlaner		glGetClipPlanef
#define glGetRealv			glGetFloatv
#define glGetLightrv		glGetLightfv
#define glGetMaterialrv		glGetMaterialfv
#define glGetTexEnvrv		glGetTexEnvfv
#define glGetTexParameterrv	glGetTexParameterfv
#define glClipPlaner		glClipPlanef
#define glPointParameterrv	glPointParameterfv
#define glPointParameterr	glPointParameterf
#define glDrawTexrOES		glDrawTexfOES
#define glDrawTexrvOES		glDrawTexfvOES
#define glTexParameterrv	glTexParameterfv

#endif


#endif // __gl_wrap_fix_h_