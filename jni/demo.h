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

#ifndef _DEMO_H_
#define _DEMO_H_

//#include <windows.h>
#include <math.h>
#include <GLES/gl.h>
#include <EGL/egl.h>

#include "flower.h"
#include "terrain.h"
#include "butterfly.h"
#include "butterflygroup.h"
#include "camera.h"
#include "sky.h"
#include "timer.h"


/** The main application class of this demo.
*/

class Demo
{
	enum Status
	{
		FREEZING,
		IDLING,
		GROWING,
		DANCING,
	};

public:
	Demo();
	~Demo();

	bool Create();
	void Destroy();

	void Tick(TimeValue dt);	
	void Render();

	bool IsFinished() { return m_bFinished; }
	void Finish() { m_bFinished = true; }

protected:
	void SetupSchedule();

	void AnimateFlower1(const Interval & itv = FOREVER);
	void AnimateFlower2(const Interval & itv = FOREVER);
	void AnimateFlower3(const Interval & itv = FOREVER);
	void AnimateFlower4(const Interval & itv = FOREVER);
	void AnimateButterflyGroup(const Interval & itv = FOREVER);
	void AnimateCamera(const Interval & itv = FOREVER);

	void RenderSky(TimeValue t);
	void RenderTerrain(TimeValue t);
	void RenderFlowers(TimeValue t);
	void RenderButterflyGroup(TimeValue t);
	void RenderCamera(TimeValue t);
	void RenderText(TimeValue t);

public:
	Camera		m_Camera;

protected:
	Flower * m_pFlower1;
	Flower * m_pFlower2;
	Flower * m_pFlower3;
	Flower * m_pFlower4;

	Script * m_pScript;
	Terrain * m_pTerrain;
	ButterflyGroup * m_pButterflyGroup;

	Sky		* m_pSky;
	Status	m_eStatus;
	TimeValue	m_AnimateTime;
	bool		m_bFinished;
private:

	RealVec3 m_LightPos;
	RealMat4 m_ViewMat, m_ProjMat;

	// animation interval
	Interval m_itvFlower1Grow, m_itvFlower2Grow, m_itvFlower3Grow, m_itvFlower4Grow;
	Interval m_itvFlower1Dance, m_itvFlower2Dance, m_itvFlower3Dance, m_itvFlower4Dance;
	Interval m_itvCameraAnimation1, m_itvCameraAnimation2;
	Interval m_itvButterflyGroupAnimation;
	
	// rendering interval
	Interval m_itvFlowersRender;
	Interval m_itvTitleFontRender, m_itvAuthorFontRender;

	Interval m_itvLensFlareRender;
	
	// butterfly Render
	Interval m_itvButterflyGroupRender1;
	Interval m_itvButterflyGroupRender2;
	
	// Camera fade
	Interval m_itvCameraFadeIn, m_itvCameraFadeOut;
	Interval m_itvCameraFadeInMid, m_itvCameraFadeOutMid;
	Interval m_itvPlayMusic1, m_itvPlayMusic2;
	bool	m_bPlayMusic1, m_bPlayMusic2;
};

#endif
