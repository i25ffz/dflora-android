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

#include "demo.h"
#include "image.h"
#include "font.h"
#include "timer.h"
#include "music.h"

Demo::Demo()
{
	m_AnimateTime = TimeValue(0);
	m_pFlower1 = NULL;
	m_pFlower2 = NULL;
	m_pFlower3 = NULL;
	m_pFlower4 = NULL;
	m_pScript  = NULL;
	m_eStatus  = FREEZING;
	m_bPlayMusic1 = false;
	m_bPlayMusic2 = false;
	m_bFinished = false;
	//srand(GetTickCount());
	srand(1024);
}

Demo::~Demo()
{
	SafeDelete(m_pFlower1);
	SafeDelete(m_pFlower2);
	SafeDelete(m_pFlower3);
	SafeDelete(m_pFlower4);
	SafeDelete(m_pScript);
	SafeDelete(m_pTerrain);
	SafeDelete(m_pButterflyGroup);
	SafeDelete(m_pSky);
}

bool Demo::Create()
{
	g_Font.CreateASCIIFont();
	m_Camera.Create();

	m_pSky = new Sky;
	m_pSky->Create();

	m_LightPos[0] = FTOR(9);
	m_LightPos[1] = FTOR(-3);
	m_LightPos[2] = FTOR(11);
	NormalizeVec3(m_LightPos);

	m_pTerrain = new Terrain;
	m_pTerrain->Create();

	m_pButterflyGroup = new ButterflyGroup;
	m_pButterflyGroup->Create(8);
	m_pButterflyGroup->Initial();	

	Real lightPosition0[4]   = {FTOR(0), FTOR(-20), FTOR(80), FTOR(1)};
	Real diffuseProperties0[4]  = {FTOR(0.7), FTOR(0.7), FTOR(0.7), FTOR(1)};
	Real specularProperties0[4] = {FTOR(0.2), FTOR(0.2), FTOR(0.2), FTOR(1)};
	Real ambientProperties0[4]  = {FTOR(0.5), FTOR(0.5), FTOR(0.5), FTOR(1)};

	glLightrv(GL_LIGHT0, GL_AMBIENT, ambientProperties0);
	glLightrv(GL_LIGHT0, GL_DIFFUSE, diffuseProperties0);
	glLightrv(GL_LIGHT0, GL_SPECULAR, specularProperties0);
	glLightModelr(GL_LIGHT_MODEL_TWO_SIDE, FTOR(1.0));
	glLightrv(GL_LIGHT0, GL_POSITION, lightPosition0);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColorr(FTOR(0.5), FTOR(0.6), FTOR(0.8), FTOR(0));

	// set camera
	m_Camera.Turn(R_HALFPI, FTOR(0));
	m_Camera.Move(FTOR(0), FTOR(-50), FTOR(20));

	m_pFlower1 = new Flower;
	m_pFlower2 = new Flower;
	m_pFlower3 = new Flower;
	m_pFlower4 = new Flower;

#ifdef FIXED_REAL
	if(!m_pFlower1->Load("/sdcard/dflora/model.q16"))	return FALSE;
	if(!m_pFlower2->Load("/sdcard/dflora/model.q16"))	return FALSE;
	if(!m_pFlower3->Load("/sdcard/dflora/model.q16"))	return FALSE;
	if(!m_pFlower4->Load("/sdcard/dflora/model.q16"))	return FALSE;
#else	// FLOAT_REAL
	if(!m_pFlower1->Load("/sdcard/dflora/model.flt"))	return FALSE;
	if(!m_pFlower2->Load("/sdcard/dflora/model.flt"))	return FALSE;
	if(!m_pFlower3->Load("/sdcard/dflora/model.flt"))	return FALSE;
	if(!m_pFlower4->Load("/sdcard/dflora/model.flt"))	return FALSE;
#endif
	m_pFlower1->StartGrowth();
	m_pFlower2->StartGrowth();
	m_pFlower3->StartGrowth();
	m_pFlower4->StartGrowth();

	m_pScript = new Script;
#ifdef FIXED_REAL
	if(!m_pScript->Load("/sdcard/dflora/littleswan.sqt"))	return FALSE;
#else	// FLOAT_REAL
	if(!m_pScript->Load("/sdcard/dflora/littleswan.sft"))	return FALSE;
#endif

	g_Timer.Start();
	m_AnimateTime = TimeValue(0);

	SetupSchedule();
	PlayMusic("/sdcard/dflora/bamboo1.wav");

	return TRUE;
}

void Demo::Destroy()
{
	m_pScript->Destory();
}

void Demo::Tick(TimeValue dt)
{
	Interval itv(m_AnimateTime, m_AnimateTime+dt);
	AnimateFlower1(itv);
	AnimateFlower2(itv);
	AnimateFlower3(itv);
	AnimateFlower4(itv);
	AnimateButterflyGroup(itv);
	AnimateCamera(itv);

	if(m_itvPlayMusic1.Contain(m_AnimateTime) && !m_bPlayMusic1)
	{
		StopMusic();
		m_bPlayMusic1 = true;
		PlayMusic("/sdcard/dflora/bamboo.wav");
	}

	if(m_itvPlayMusic2.Contain(m_AnimateTime) && !m_bPlayMusic2)
	{
		StopMusic();
		m_bPlayMusic2 = true;
		PlayMusic("/sdcard/dflora/littleswan.wav");
	}
	m_AnimateTime += dt;
	
	if(m_AnimateTime > TimeValue(163500))
	{
		StopMusic();
		Finish();
	}
}

void Demo::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	m_Camera.SetPerspective();

	RenderSky(m_AnimateTime);
	RenderTerrain(m_AnimateTime);
	RenderButterflyGroup(m_AnimateTime);
	RenderFlowers(m_AnimateTime);
	RenderCamera(m_AnimateTime);
	RenderText(m_AnimateTime);	
}

void Demo::SetupSchedule()
{
	TimeValue start = TimeValue(0), end = TimeValue(0);
	TimeValue itv = TimeValue(5*1000);
	m_itvPlayMusic1.Set(start, TimeValue(1000));

	end += itv;
	m_itvCameraFadeIn.Set(start, end);
	m_itvTitleFontRender.Set(start, end);
	
	itv = TimeValue(10*1000);
	start = end; end += itv;
	m_itvLensFlareRender.Set(start, start+TimeValue(6200));
	m_itvCameraAnimation1.Set(TimeValue(100), end);
	
	start = end;
	itv = TimeValue(10*1000);
	end = TimeValue(62*1000);
	m_itvFlower1Grow.Set(start, end);
	start += itv;
	m_itvFlower2Grow.Set(start, end);
	start += itv;
	m_itvFlower3Grow.Set(start, end);
	start += itv;
	m_itvFlower4Grow.Set(start, end);

	start = end-TimeValue(4000);
	m_itvCameraFadeOutMid.Set(start, end-TimeValue(2000));
	start = end-TimeValue(2000);
	m_itvCameraFadeInMid.Set(start, end);
	m_itvButterflyGroupRender1.Set(TimeValue(4500), end-TimeValue(4000));

	itv = TimeValue(98*1000);
	start = end; end = start + itv;
	m_itvFlower1Dance.Set(start, end);
	m_itvFlower2Dance.Set(start, end);
	m_itvFlower3Dance.Set(start, end);
	m_itvFlower4Dance.Set(start, end);
	m_itvButterflyGroupRender2.Set(start, end-TimeValue(4000));
	m_itvCameraAnimation2.Set(start, end);
	m_itvButterflyGroupAnimation.Set(TimeValue(0), end);
	m_itvPlayMusic2.Set(start, start+TimeValue(1000));
	
	// rendering interval
	m_itvFlowersRender.Set(TimeValue(0), end);
	start = end-TimeValue(3000);
	end += TimeValue(4000);
	m_itvCameraFadeOut.Set(start, end);
	m_itvAuthorFontRender.Set(start, end);
}

void Demo::AnimateFlower1(const Interval & itv)
{
	if(m_itvFlower1Grow.StartIn(itv))
		m_pFlower1->StartGrowth();

	if(m_itvFlower1Grow.Contain(itv))
		m_pFlower1->UpdateGrowth(itv.Duration());

	if(m_itvFlower1Grow.EndIn(itv))
		m_pFlower1->StopGrowth();

	if(m_itvFlower1Dance.StartIn(itv))
		m_pFlower1->StartDance(m_pScript, 310);

	if(m_itvFlower1Dance.Contain(itv))
		m_pFlower1->UpdateDance(itv.Duration());

	if(m_itvFlower1Dance.EndIn(itv))
		m_pFlower1->StopDance();
}

void Demo::AnimateFlower2(const Interval & itv)
{
	if(m_itvFlower2Grow.StartIn(itv))
		m_pFlower2->StartGrowth();

	if(m_itvFlower2Grow.Contain(itv))
		m_pFlower2->UpdateGrowth(itv.Duration());

	if(m_itvFlower2Grow.EndIn(itv))
		m_pFlower2->StopGrowth();


	if(m_itvFlower2Dance.StartIn(itv))
		m_pFlower2->StartDance(m_pScript, 310);

	if(m_itvFlower2Dance.Contain(itv))
		m_pFlower2->UpdateDance(itv.Duration());

	if(m_itvFlower2Dance.EndIn(itv))
		m_pFlower2->StopDance();
}

void Demo::AnimateFlower3(const Interval & itv)
{
	if(m_itvFlower3Grow.StartIn(itv))
		m_pFlower3->StartGrowth();

	if(m_itvFlower3Grow.Contain(itv))
		m_pFlower3->UpdateGrowth(itv.Duration());

	if(m_itvFlower3Grow.EndIn(itv))
		m_pFlower3->StopGrowth();


	if(m_itvFlower3Dance.StartIn(itv))
		m_pFlower3->StartDance(m_pScript, 310);

	if(m_itvFlower3Dance.Contain(itv))
		m_pFlower3->UpdateDance(itv.Duration());

	if(m_itvFlower3Dance.EndIn(itv))
		m_pFlower3->StopDance();
}

void Demo::AnimateFlower4(const Interval & itv)
{
	if(m_itvFlower4Grow.StartIn(itv))
		m_pFlower4->StartGrowth();

	if(m_itvFlower4Grow.Contain(itv))
		m_pFlower4->UpdateGrowth(itv.Duration());

	if(m_itvFlower4Grow.EndIn(itv))
		m_pFlower4->StopGrowth();

	if(m_itvFlower4Dance.StartIn(itv))
		m_pFlower4->StartDance(m_pScript, 310);

	if(m_itvFlower4Dance.Contain(itv))
		m_pFlower4->UpdateDance(itv.Duration());

	if(m_itvFlower4Dance.EndIn(itv))
		m_pFlower4->StopDance();
}

void Demo::AnimateButterflyGroup(const Interval & itv)
{
	if(m_itvButterflyGroupAnimation.Contain(itv))
	{
		m_pButterflyGroup->Update(itv.Duration());
	}
}

void Demo::AnimateCamera(const Interval & itv)
{
	if(m_itvCameraAnimation1.Contain(itv) || m_itvCameraAnimation2.Contain(itv))
	{
		m_Camera.UpdateAnimation(itv.Duration());
	}
}

void Demo::RenderSky(TimeValue t)
{
	RealVec3 campos;
	m_Camera.GetPosition(campos);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	Real scale = FTOR(50);
	glTranslater(campos[0], campos[1], campos[2]-FTOR(13));
	glScaler(scale, scale, scale);
	glColor4r(FTOR(1), FTOR(1), FTOR(1), FTOR(1));
	m_pSky->Draw();
	glGetRealv(GL_MODELVIEW_MATRIX, m_ViewMat);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void Demo::RenderTerrain(TimeValue t)
{
	m_pTerrain->Draw();
}

void Demo::RenderFlowers(TimeValue t)
{
	if(m_itvFlowersRender.Contain(t))
	{
		Real dis2 = FTOR(15);
		Real dis1 = FTOR(5);
		glPushMatrix();
		glTranslater(-dis2, R_ZERO, R_ZERO);
		m_pFlower1->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslater(-dis1, R_ZERO, R_ZERO);
		m_pFlower2->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslater(dis1, R_ZERO, R_ZERO);
		m_pFlower3->Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslater(dis2, R_ZERO, R_ZERO);
		m_pFlower4->Draw();
		glPopMatrix();
	}
}

void Demo::RenderButterflyGroup(TimeValue t)
{
	if(m_itvButterflyGroupRender1.Contain(t) || m_itvButterflyGroupRender2.Contain(t) )
		m_pButterflyGroup->Draw();
}

void Demo::RenderCamera(TimeValue t)
{
	if(m_itvCameraFadeIn.Contain(t))
	{
		Real alpha = Quotient(ITOR(t-m_itvCameraFadeIn.Start()),
			ITOR(m_itvCameraFadeIn.Duration()));
		m_Camera.Fade(R_ONE-alpha);
	}
	
	if(m_itvCameraFadeOut.Contain(t))
	{
		Real alpha = Quotient(ITOR(t-m_itvCameraFadeOut.Start()), 
			ITOR(m_itvCameraFadeOut.Duration()));
		m_Camera.Fade(alpha);
	}

	if(m_itvCameraFadeInMid.Contain(t))
	{
		Real alpha = Quotient(ITOR(t-m_itvCameraFadeInMid.Start()),
			ITOR(m_itvCameraFadeInMid.Duration()));
		m_Camera.Fade(R_ONE-alpha);
	}
	
	if(m_itvCameraFadeOutMid.Contain(t))
	{
		Real alpha = Quotient(ITOR(t-m_itvCameraFadeOutMid.Start()), 
			ITOR(m_itvCameraFadeOutMid.Duration()));
		m_Camera.Fade(alpha);
	}

	if(m_itvLensFlareRender.Contain(t))
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetRealv(GL_PROJECTION_MATRIX, m_ProjMat);
		m_Camera.DrawLensFlare(m_LightPos, m_ViewMat, m_ProjMat, viewport);
	}
}

void Demo::RenderText(TimeValue t)
{
	if(m_itvTitleFontRender.Contain(t))
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glColor4r(FTOR(1.0), FTOR(0), FTOR(0), FTOR(0.8));	
		g_Font.Draw2DStringLine(FTOR(40), FTOR(180), FTOR(3.0), FTOR(3.0), "Dancing", true);
		g_Font.Draw2DStringLine(FTOR(100), FTOR(140), FTOR(3.0), FTOR(3.0), "Flora", true);
		glDisable(GL_BLEND);
	}
	if(m_itvAuthorFontRender.Contain(t))
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4r(FTOR(1.0), FTOR(1.0), FTOR(1.0), FTOR(0.8));
		g_Font.Draw2DStringLine(FTOR(20), FTOR(180), FTOR(1.2), FTOR(1.2), "Producer", true);
		g_Font.Draw2DStringLine(FTOR(20), FTOR(150), FTOR(1.2), FTOR(1.2), "risk3D", true);
		g_Font.Draw2DStringLine(FTOR(20), FTOR(120), FTOR(1.2), FTOR(1.2), "richardzlee@163.com");
		glDisable(GL_BLEND);
	}
}


