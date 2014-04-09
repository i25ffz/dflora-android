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

#include "butterflygroup.h"

ButterflyGroup::ButterflyGroup()
{
	m_nNumButterflies = 0;
	m_pButterflies = NULL;
	m_Center[0] = R_ZERO;
	m_Center[1] = R_ZERO;
	m_Center[2] = R_ZERO;
	m_Boundary = R_ONE;
	m_ActiveBehaviour = STOP;
}

ButterflyGroup::~ButterflyGroup()
{
	Destroy();
}

void ButterflyGroup::Create(uint8 num)
{
	m_nNumButterflies = num;
	m_pButterflies = new Butterfly[num];
	for(uint8 i=0; i<num; i++)
	{
		m_pButterflies[i].Create();
	}
	m_Texture1.SetImageFile("butterfly1.tga");
	m_Texture2.SetImageFile("butterfly2.tga");

}

void ButterflyGroup::Destroy()
{
	SafeDeleteArray(m_pButterflies);
}

void ButterflyGroup::Initial()
{
	RealVec3 pos;
	for(uint8 i=0; i<m_nNumButterflies; i++)
	{
		Butterfly * but = m_pButterflies+i;
		but->Initial();		
		m_Center[0] = FTOR(0);
		m_Center[1] = FTOR(0);
		m_Center[2] = FTOR(30);
		m_Boundary = FTOR(4.f);
		pos[0] = Product((R_HALF-Rand01()), m_Boundary) + m_Center[0];
		pos[1] = Product((R_HALF-Rand01()), m_Boundary) + m_Center[1];
		pos[2] = Product((R_HALF-Rand01()), m_Boundary) + m_Center[2];
		but->SetPosition(pos);
		RealVec3 zero = {R_ZERO, R_ZERO, R_ZERO};
		but->SetVelocity(zero);
		but->SetMaxAcceleration(FTOR(1.5));
		but->SetAcceleration(zero);
		but->SetMaxSpeed(FTOR(4));
		but->SetMinSpeed(R_ZERO);
	}
	SetActiveBehaviour(WANDER);
}

void ButterflyGroup::Update(TimeValue deltaTime)
{
	switch(m_ActiveBehaviour)
	{
	case WANDER:
		UpdateWanderBehavour(deltaTime);
		break;
	case HOVER:
		UpdateHoverBehavour(deltaTime);
		break;
	case BOID:
		UpdateBoidBehavour(deltaTime);
		break;
	default:
		break;
	}
}

void ButterflyGroup::SetActiveBehaviour(BEHAVIOUR behav)
{
	int i=0;
	m_ActiveBehaviour = behav;
	switch(m_ActiveBehaviour)
	{
	case WANDER:
		for(i=0; i<m_nNumButterflies; i++)
		{
			Butterfly * but =  m_pButterflies+i;
			but->SetMinSpeed(FTOR(0.f));
		}		
		break;
	case HOVER:
		for(i=0; i<m_nNumButterflies; i++)
		{
			Butterfly * but =  m_pButterflies+i;
			but->SetMinSpeed(Product(but->GetMaxSpeed(), FTOR(0.5)));
		}		
		break;
	case BOID:
		for(i=0; i<m_nNumButterflies; i++)
		{
			Butterfly * but =  m_pButterflies+i;
			but->SetMinSpeed(FTOR(0.f));
		}		
		break;
	default:		
		break;
	}
}
	
void ButterflyGroup::Draw()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_ALPHA_TEST);
	glAlphaFuncr(GL_GREATER, FTOR(0.5));

	m_Texture1.Apply();
	uint8 half = (uint8)(m_nNumButterflies>>1);
	uint8 i;
	for(i=0; i<half; i++)
	{
		m_pButterflies[i].Draw();
	}
	m_Texture2.Apply();
	for(i=half; i<m_nNumButterflies; i++)
	{
		m_pButterflies[i].Draw();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_LIGHTING);
}

void ButterflyGroup::UpdateWanderBehavour(TimeValue deltaTime)
{
	for(uint8 i=0; i<m_nNumButterflies; i++)
	{
		Butterfly * but = m_pButterflies+i;
		RealVec3 steerTotal, steerThis;
		but->SteerForWander(deltaTime, steerThis);
		steerTotal[0] = steerThis[0];
		steerTotal[1] = steerThis[1];
		steerTotal[2] = steerThis[2];

		but->RespondToBoundary(m_Center, m_Boundary, steerThis);
		steerTotal[0] += steerThis[0];
		steerTotal[1] += steerThis[1];
		steerTotal[2] += steerThis[2];

		but->SetAcceleration(steerTotal);
		but->Update(deltaTime);
		but->ClampPosition(deltaTime);
	}
}

void ButterflyGroup::UpdateHoverBehavour(TimeValue deltaTime)
{
	for(uint8 i=0; i<m_nNumButterflies; i++)
	{
		Butterfly * but = m_pButterflies+i;
		RealVec3 steerTotal, steerThis;
		but->SteerForSeek(m_Target, steerThis);
		steerTotal[0] = steerThis[0];
		steerTotal[1] = steerThis[1];
		steerTotal[2] = steerThis[2];
		
		but->SteerForWander(deltaTime, steerThis);
		steerTotal[0] += steerThis[0];
		steerTotal[1] += steerThis[1];
		steerTotal[2] += steerThis[2];

		Real separationRadius = Product(but->GetRadius(), 4);
		Real separationAngle  = FTOR(-0.707f);
		Real separationWeight =  FTOR(1.0f);

		but->SteerForSeparation(separationRadius, separationAngle, m_pButterflies, m_nNumButterflies, steerThis);
		steerTotal[0] += Product(steerThis[0], separationWeight);
		steerTotal[1] += Product(steerThis[1], separationWeight);
		steerTotal[2] += Product(steerThis[2], separationWeight);

		but->RespondToBoundary(m_Center, m_Boundary, steerThis);
		steerTotal[0] += steerThis[0];
		steerTotal[1] += steerThis[1];
		steerTotal[2] += steerThis[2];

		but->SetAcceleration(steerTotal);
		but->Update(deltaTime);
		but->ClampPosition(deltaTime);
	}
}

void ButterflyGroup::UpdateFleeBehavour(TimeValue deltaTime)
{
	for(uint8 i=0; i<m_nNumButterflies; i++)
	{
		Butterfly * but = m_pButterflies+i;
		RealVec3 steerTotal, steerThis;
		but->SteerForFlee(m_Target, steerThis);
		steerTotal[0] = steerThis[0];
		steerTotal[1] = steerThis[1];
		steerTotal[2] = steerThis[2];
		
		but->RespondToBoundary(m_Center, m_Boundary, steerThis);
		steerTotal[0] += steerThis[0];
		steerTotal[1] += steerThis[1];
		steerTotal[2] += steerThis[2];

		but->SetAcceleration(steerTotal);
		but->Update(deltaTime);
		but->ClampPosition(deltaTime);
	}
}

void ButterflyGroup::UpdateBoidBehavour(TimeValue deltaTime)
{
	for(uint8 i=0; i<m_nNumButterflies; i++)
	{
		Butterfly * but = m_pButterflies+i;

		Real separationRadius = Product(but->GetRadius(), FTOR(3.f));
		Real separationAngle  = FTOR(-0.707f);
		Real separationWeight =  FTOR(1.2f);

		Real alignmentRadius = Product(but->GetRadius(), FTOR(5.f));
		Real alignmentAngle  = FTOR(0.7f);
		Real alignmentWeight = FTOR(0.8f);

		Real cohesionRadius = Product(but->GetRadius(), FTOR(10.f));
		Real cohesionAngle  = FTOR(-0.15f);
		Real cohesionWeight = FTOR(0.7f);//0.8;

		Real maxRadius = alignmentRadius;
		if(maxRadius < cohesionRadius)	maxRadius = cohesionRadius;
		if(maxRadius < separationRadius) maxRadius = separationRadius;

		RealVec3 steerTotal = {R_ZERO, R_ZERO, R_ZERO};
		RealVec3 steerThis  = {R_ZERO, R_ZERO, R_ZERO};
		
		but->SteerForSeparation(separationRadius, separationAngle, m_pButterflies, m_nNumButterflies, steerThis);
		steerTotal[0] += Product(steerThis[0], separationWeight);
		steerTotal[1] += Product(steerThis[1], separationWeight);
		steerTotal[2] += Product(steerThis[2], separationWeight);

		but->SteerForAlignment(alignmentRadius, alignmentAngle, m_pButterflies, m_nNumButterflies, steerThis);
		steerTotal[0] += Product(steerThis[0], alignmentWeight);
		steerTotal[1] += Product(steerThis[1], alignmentWeight);
		steerTotal[2] += Product(steerThis[2], alignmentWeight);

		but->SteerForCohesion(cohesionRadius, cohesionAngle, m_pButterflies, m_nNumButterflies, steerThis);
		steerTotal[0] += Product(steerThis[0], cohesionWeight);
		steerTotal[1] += Product(steerThis[1], cohesionWeight);
		steerTotal[2] += Product(steerThis[2], cohesionWeight);

		but->RespondToBoundary(m_Center, m_Boundary, steerThis);
		steerTotal[0] += steerThis[0];
		steerTotal[1] += steerThis[1];
		steerTotal[2] += steerThis[2];

		but->SetAcceleration(steerTotal);
		but->Update(deltaTime);
		but->ClampPosition(deltaTime);
	}
}

