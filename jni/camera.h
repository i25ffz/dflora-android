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

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "utility.h"
#include "texture.h"

/** A view point from which the scene will be observed.
	@remarks
	This class realizes camera function in the same way as the glu function gluLookAt.
	It defines the view point position, adopts the Eular angle roll, yaw and pitch (roll
	always to be zero) to determine the rotation/direction of camera directions. The Camera 
	class also uses the length of lens combined with yaw and pitch angle of camera to 
	determine the lookat point. It has SetPerspective function similiar with gluLookAt to 
	set the perspective view of the final rendering scene. To confine the view point, class 
	Camera has clamp function to restrain the position, direction and focusing point.
	@par
	In addition to standard camera functions: focus, move, turn and hover around something,
	Class Camera has implemented the gfx of fade in/out and lens flare. The implemetation of 
	lens flare is greatly modified from NeHe OpenGL lesson 44 3D Lens Flare With Occlusion, 
	but it is more simple and efficient, since the lens flare only is rendered in 2D viewport 
	coordinate and without occlusion detecting.
	@note
	We commented out vertex buffer object rendering solution, because there is no apparent 
	efficiency improvement. I don't konw how the inner implementation of gerbera of VBO for the 
	win32 software pipeline.
*/

class Camera  
{
public:
	Camera();
	~Camera();
	void Create();
	void SetPerspective();
	void Move(Real dx, Real dy, Real dz);
	void MoveForward(Real delta);
	void MoveSideward(Real delta);
	void Turn(Real dlon, Real dlat);
	void Focus(Real x, Real y, Real z);
	void Zoom(Real dlen);
	void Hover(Real foc_x, Real foc_y, Real theta);
	void GetPosition(RealVec3 pos);

	void DrawLensFlare(RealVec3 lightPos, RealMat4 viewMat, RealMat4 projMat, GLint viewport[4]);

protected:
	void DrawStreaks(RealVec4 color, RealVec2 pos, Real scale);
	void DrawBigGlow(RealVec4 color, RealVec2 pos, Real scale);
	void DrawGlow(RealVec4 color, RealVec2 pos, Real scale);
	void DrawHalo(RealVec4 color, RealVec2 pos, Real scale);

public:
	void Fade(Real alpha);
	void UpdateAnimation(TimeValue dt);


private:
	void Clamp();
	Real m_Len, m_LenMin, m_LenMax;		// the length of lens focus
	RealVec3 m_Position, m_PositionMin, m_PositionMax;	// camera location
	Real m_Yaw, m_Pitch, m_PitchMax, m_PitchMin;	// lens direction: Euler Angle "xyz" (pitch-roll-yaw) convention
	
	// lens flare textures
	Texture m_TextureStreak;
	Texture m_TextureHalo;
	Texture m_TextureGlow;
	Texture m_TextureBigGlow;
	RealVec2 m_Vertices[4]; 
	RealVec2 m_TexCoords[4];
	
	// optional VBO solution
//	GLuint	m_VerticesBuffer;
//	GLuint	m_TexCoordsBuffer;
	
};

#endif
