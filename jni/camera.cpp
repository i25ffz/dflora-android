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
#include "camera.h"


Camera::Camera()
{
	m_PositionMin[0]= FTOR(-100);		m_PositionMax[0] = FTOR(100);
	m_PositionMin[1] = FTOR(-100);		m_PositionMax[1] = FTOR(100);
	m_PositionMin[2] = FTOR(-100);		m_PositionMax[2] = FTOR(100);
	m_PitchMax = Product(R_PI, FTOR(0.45));
	m_PitchMin = -Product(R_PI, FTOR(0.45));
	m_LenMax = FTOR(100);
	m_LenMin = FTOR(1);
	m_Len = FTOR(40);
}

Camera::~Camera()
{
//	glDeleteBuffers(1, &m_VerticesBuffer);
//	glDeleteBuffers(1, &m_TexCoordsBuffer);
}

void Camera::Create()
{
	m_PositionMin[0]= FTOR(-100);		m_PositionMax[0] = FTOR(100);
	m_PositionMin[1] = FTOR(-100);		m_PositionMax[1] = FTOR(100);
	m_PositionMin[2] = FTOR(-100);		m_PositionMax[2] = FTOR(100);
	m_PitchMax = Product(R_PI, FTOR(0.45));
	m_PitchMin = -Product(R_PI, FTOR(0.45));
	m_LenMax = FTOR(100);
	m_LenMin = FTOR(1);

	m_Position[0] = m_Position[1] = m_Position[2] = R_ZERO;
	m_Pitch = m_Yaw = R_ZERO;

	m_TextureStreak.SetImageFile("Streaks.bmp");
	m_TextureStreak.LoadTexture();
	m_TextureHalo.SetImageFile("Halo.bmp");
	m_TextureHalo.LoadTexture();
	m_TextureGlow.SetImageFile("HardGlow.bmp");
	m_TextureGlow.LoadTexture();
	m_TextureBigGlow.SetImageFile("BigGlow.bmp");
	m_TextureBigGlow.LoadTexture();

	m_Vertices[0][0] = FTOR(-0.5);
	m_Vertices[0][1] = FTOR(-0.5);

	m_Vertices[1][0] = FTOR(0.5);
	m_Vertices[1][1] = FTOR(-0.5);

	m_Vertices[2][0] = FTOR(0.5);
	m_Vertices[2][1] = FTOR(0.5);

	m_Vertices[3][0] = FTOR(-0.5);
	m_Vertices[3][1] = FTOR(0.5);

	m_TexCoords[0][0] = R_ZERO;
	m_TexCoords[0][1] = R_ZERO;

	m_TexCoords[1][0] = R_ONE;
	m_TexCoords[1][1] = R_ZERO;

	m_TexCoords[2][0] = R_ONE;
	m_TexCoords[2][1] = R_ONE;

	m_TexCoords[3][0] = R_ZERO;
	m_TexCoords[3][1] = R_ONE;

	// optional VBO rendering solution
//	glGenBuffers(1, &m_VerticesBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, m_VerticesBuffer);
//	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(RealVec2), m_Vertices, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);

//	glGenBuffers(1, &m_TexCoordsBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordsBuffer);
//	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(RealVec2), m_TexCoords, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Camera::Move(Real dx, Real dy, Real dz)
{
	m_Position[0] += dx;	
	m_Position[1] += dy;	
	m_Position[2] += dz;
	Clamp();
}

void Camera::MoveForward(Real delta)
{
	Real ca = Product(delta, Cos(m_Pitch));
	RealVec3 dir;

	dir[0] = Product(ca, Cos(m_Yaw));
	dir[1] = Product(ca, Sin(m_Yaw));
	dir[2] = Product(delta, Sin(m_Pitch));

	m_Position[0] += dir[0];
	m_Position[1] += dir[1];
	m_Position[2] += dir[2];

	Clamp();
}

void Camera::MoveSideward(Real delta)
{
	Real x, y;
	
	Real l=m_Yaw - R_HALFPI;
	x = Product(delta, Cos(l));
	y = Product(delta, Sin(l));
	m_Position[0] += x;
	m_Position[1] += y;

	Clamp();
}

void Camera::Turn(Real dlon, Real dlat)
{
	m_Yaw += dlon;
	m_Pitch += dlat;

	Clamp();
}

void Camera::Focus(Real x, Real y, Real z)
{
	Real ca = m_Len * Cos( m_Pitch );
	Real dir[3];

	dir[0] = ca * Cos(m_Yaw);
	dir[1] = ca * Sin(m_Yaw);
	dir[2] = m_Len * Sin(m_Pitch);

	m_Position[0] = x - dir[0];
	m_Position[1] = y - dir[1];
	m_Position[2] = z - dir[2];

	Clamp();
}

void Camera::SetPerspective()
{
	Real ca = Product(m_Len, Cos(m_Pitch));
	RealVec3 dir, foc;

	dir[0] = Product(ca, Cos(m_Yaw));
	dir[1] = Product(ca, Sin(m_Yaw));
	dir[2] = Product(m_Len, Sin(m_Pitch));

	foc[0] = m_Position[0] + dir[0];
	foc[1] = m_Position[1] + dir[1];
	foc[2] = m_Position[2] + dir[2];

	RealMat4	ViewMatrix; 
	LookAtMatrix(ViewMatrix,
				m_Position[0], m_Position[1], m_Position[2],    // position of viewer
                foc[0], foc[1], foc[2],							// Where to look
                R_ZERO, R_ZERO, R_ONE);							// Up axis

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
    // Apply the view matrix
	glMultMatrixr(ViewMatrix);		// ogl es 1.1

}

void Camera::Zoom(Real dlen)
{
	m_Len += dlen;
	Real ca = Product(dlen, Cos(m_Pitch));
	Real dir[3];

	dir[0] = Product(ca, Cos(m_Yaw));
	dir[1] = Product(ca, Sin(m_Yaw));
	dir[2] = Product(m_Len, Sin(m_Pitch));

	m_Position[0] += dir[0];
	m_Position[1] += dir[1];
	m_Position[2] += dir[2];

	Clamp();
}

void Camera::Hover(Real foc_x, Real foc_y, Real theta)
{
	RealVec3 dir = {R_ZERO, R_ZERO, R_ZERO};
	Real foc_z, disXY;
	dir[0] = foc_x - m_Position[0];
	dir[1] = foc_y - m_Position[1];
	
	disXY = LengthVec3(dir);

	if(disXY < FTOR(0.001)){
		Turn(theta, R_ZERO);
		return;
	}
	dir[2] = - Product(disXY, Quotient(Sin(m_Pitch), Cos(m_Pitch)));	// careful: tan(m_Pitch) --> unlimit 
	foc_z = m_Position[2] - dir[2];

	m_Len = LengthVec3(dir);
	
	m_Yaw += theta;
	if(m_Yaw > R_PI)	m_Yaw -= R_2PI;
	if(m_Yaw < -R_PI)	m_Yaw += R_2PI;

	Real ca = Product(m_Len, Cos(m_Pitch));

	dir[0] = Product(ca, Cos(m_Yaw));
	dir[1] = Product(ca, Sin(m_Yaw));
	dir[2] = Product(m_Len, Sin(m_Pitch));

	m_Position[0] = foc_x - dir[0];
	m_Position[1] = foc_y - dir[1];
	m_Position[2] = foc_z - dir[2];

	Clamp();
}

void Camera::Clamp()
{
	for(int i=0; i<3; i++)
	{
		if(m_Position[i]>m_PositionMax[i])
			m_Position[i] = m_PositionMax[i];
		else if(m_Position[i]<m_PositionMin[i])
			m_Position[i] =  m_PositionMin[i];
	}

	m_Pitch = (m_Pitch>m_PitchMax) ? m_PitchMax : m_Pitch;
	m_Pitch = (m_Pitch<m_PitchMin) ? m_PitchMin : m_Pitch;
	m_Len = (m_Len>m_LenMax) ? m_LenMax : m_Len;
	m_Len = (m_Len<m_LenMin) ? m_LenMin : m_Len;
}

void Camera::GetPosition(RealVec3 pos)
{
	pos[0] = m_Position[0];
	pos[1] = m_Position[1];
	pos[2] = m_Position[2];
}

void Camera::DrawLensFlare(RealVec3 lightPos, RealMat4 viewMat, RealMat4 projMat, GLint viewport[4])
{

	RealVec3 lightPosInViewport;
	Real	 cx, cy;
	RealVec4 color;
	
	if(ProjectVec3ToViewport(lightPos, viewMat, projMat, viewport, lightPosInViewport))
	{
		cx = Product(ITOR(viewport[0]+viewport[2]), FTOR(0.5));
		cy = Product(ITOR(viewport[1]+viewport[3]), FTOR(0.5));

		GLint matrixmode = Begin2DDrawing();
		glDisable(GL_LIGHTING);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glEnableClientState(GL_VERTEX_ARRAY);
//		glBindBuffer(GL_ARRAY_BUFFER, m_VerticesBuffer);	// optional vbo
		glVertexPointer(2, GL_REAL, 0, m_Vertices);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//		glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordsBuffer);	// optional vbo
		glTexCoordPointer(2, GL_REAL, 0, m_TexCoords);

		RealVec2 pos;
		Real scalebase = Product(ITOR(viewport[2]+viewport[3]), FTOR(0.03));
		Real dx = Product(FTOR(2.0f), cx - lightPosInViewport[0]);
		Real dy = Product(FTOR(2.0f), cy - lightPosInViewport[1]);

		pos[0] = lightPosInViewport[0];
		pos[1] = lightPosInViewport[1];

		// render the lens flare, borrowed from NeHe
		// ender the large hazy glow
		color[0] = FTOR(0.6f); color[1] = FTOR(0.6f); 
		color[2] = FTOR(0.8f); color[3] = FTOR(1.0f);
		DrawBigGlow(color, pos, FTOR(36.0f));
		DrawStreaks(color, pos, FTOR(40.0f));

		color[0] = FTOR(0.8f); color[1] = FTOR(0.8f); 
		color[2] = FTOR(1.0f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.1f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.1f));

		DrawGlow(color, pos, Product(FTOR(0.6), scalebase));

		color[0] = FTOR(0.9f); color[1] = FTOR(0.6f);
		color[2] = FTOR(0.4f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.15f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.15f));

		DrawHalo(color, pos, Product(FTOR(1.7), scalebase));

		color[0] = FTOR(0.8f); color[1] = FTOR(0.5f);
		color[2] = FTOR(0.6f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.175f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.175f));
		
		DrawHalo(color, pos, Product(FTOR(0.83), scalebase));
		
		color[0] = FTOR(0.7f); color[1] = FTOR(0.7f);
		color[2] = FTOR(0.4f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.285f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.285f));

		DrawHalo(color, pos, Product(FTOR(1.6), scalebase));
		
		color[0] = FTOR(0.9f); color[1] = FTOR(0.2f);
		color[2] = FTOR(0.1f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.2755f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.2755f));
		
		DrawGlow(color, pos, Product(FTOR(0.8), scalebase));

		color[0] = FTOR(0.9f); color[1] = FTOR(0.9f);
		color[2] = FTOR(0.2f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.4755f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.4755f));
		
		DrawGlow(color, pos, Product(FTOR(1.0), scalebase));

		color[0] = FTOR(0.93f); color[1] = FTOR(0.82f);
		color[2] = FTOR(0.72f); color[3] = FTOR(0.5f);
		
		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.49f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.49f));
		
		DrawHalo(color, pos, Product(FTOR(1.4), scalebase));

		color[0] = FTOR(0.7f); color[1] = FTOR(0.6f);
		color[2] = FTOR(0.5f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.65f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.65f));
		
		DrawGlow(color, pos, Product(FTOR(1.4), scalebase));

		color[0] = FTOR(0.7f); color[1] = FTOR(0.8f);
		color[2] = FTOR(0.3f); color[3] = FTOR(0.5f);
		
		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.63f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.63f));
		
		DrawGlow(color, pos, Product(FTOR(1.8), scalebase));

		color[0] = FTOR(0.4f); color[1] = FTOR(0.3f);
		color[2] = FTOR(0.2f); color[3] = FTOR(0.5f);
		
		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.8f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.8f));
		
		DrawHalo(color, pos, Product(FTOR(1.4), scalebase));
		
		color[0] = FTOR(0.7f); color[1] = FTOR(0.5f);
		color[2] = FTOR(0.5f); color[3] = FTOR(0.5f);
		
		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.7825f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.7825f));
		
		DrawGlow(color, pos, Product(FTOR(0.6), scalebase));

		color[0] = FTOR(0.5f); color[1] = FTOR(0.7f);
		color[2] = FTOR(0.7f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(1.0f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(1.0f));

		DrawHalo(color, pos, Product(FTOR(1.7), scalebase));
		
		color[0] = FTOR(0.4f); color[1] = FTOR(0.1f);
		color[2] = FTOR(0.9f); color[3] = FTOR(0.5f);

		pos[0] = lightPosInViewport[0] + Product(dx, FTOR(0.975f));
		pos[1] = lightPosInViewport[1] + Product(dy, FTOR(0.975f));

		DrawGlow(color, pos, Product(FTOR(2.0), scalebase));

//		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
		End2DDrawing(matrixmode);
	}
}

void Camera::DrawStreaks(RealVec4 color, RealVec2 pos, Real scale)
{
	m_TextureStreak.Apply();
	glColor4r(color[0], color[1], color[2], color[3]);
	glPushMatrix();
		glTranslater(pos[0], pos[1], R_ZERO);
		glScaler(scale, scale, R_ONE);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();	
}

void Camera::DrawBigGlow(RealVec4 color, RealVec2 pos, Real scale)
{
	m_TextureBigGlow.Apply();
	glColor4r(color[0], color[1], color[2], color[3]);
	glPushMatrix();
		glTranslater(pos[0], pos[1], R_ZERO);
		glScaler(scale, scale, R_ONE);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();
}

void Camera::DrawGlow(RealVec4 color, RealVec2 pos, Real scale)
{
	m_TextureGlow.Apply();
	glColor4r(color[0], color[1], color[2], color[3]);
	glPushMatrix();
		glTranslater(pos[0], pos[1], R_ZERO);
		glScaler(scale, scale, R_ONE);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();
}

void Camera::DrawHalo(RealVec4 color, RealVec2 pos, Real scale)
{
	m_TextureHalo.Apply();
	glColor4r(color[0], color[1], color[2], color[3]);
	glPushMatrix();
		glTranslater(pos[0], pos[1], 0);
		glScaler(scale, scale, R_ONE);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glPopMatrix();

}

void Camera::Fade(Real alpha)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_REAL, 0, m_Vertices);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLint matrixmode = Begin2DDrawing();
	GLsizei	viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	Real centerx = Product(ITOR(viewport[0]+viewport[2]), FTOR(0.5));
	Real centery = Product(ITOR(viewport[1]+viewport[3]), FTOR(0.5));

	glColor4r(0, 0, 0, alpha);
	glTranslater(centerx, centery, R_ZERO);
	glScaler(ITOR(viewport[2]), ITOR(viewport[3]), R_ONE);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	End2DDrawing(matrixmode);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	glDisableClientState(GL_VERTEX_ARRAY);
}


void Camera::UpdateAnimation(TimeValue dt)
{
	
	Real kdt = Quotient(ITOR(dt), ITOR(-7000));
	Hover(0, 0, kdt);

	Clamp();	
	SetPerspective();
}