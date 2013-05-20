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

#ifndef _BUTTERFLYGROUP_H_
#define _BUTTERFLYGROUP_H_

#include "butterfly.h"

/** Wrapper class of a group of butterflies.
	@remarks
	On larger scale, each butterfly is regarded as a mass point. There are several 
	group behaviours as wander, hover, flee, boid, etc. can be applied to. The motion 
	control method of group of butterflies is modified from OpenSteer, for more information 
	see the website of C. W. Reynolds: http://www.red.com/cwr/
	@par
	In order to accelerate the rendering process, ButterflyGroup also acts
	as a resource manage to hold the texture resources for all the butterflies
	and render them by texture classification.
	@see
	Butterfly

*/

class ButterflyGroup
{
	friend class Butterfly;
	enum BEHAVIOUR
	{
		STOP,
		WANDER,
		HOVER,
		FLEE,
		BOID,
	};

public:
	ButterflyGroup();
	~ButterflyGroup();
	void Create(uint8 num);
	void Destroy();
	void Initial();
	void Update(TimeValue deltaTime);
	void Draw();

	void SetActiveBehaviour(BEHAVIOUR behav);
	void SetCenter(const RealVec3 c)
	{
		m_Center[0] = c[0];
		m_Center[1] = c[1];
		m_Center[2] = c[2];
	}
	
	void GetCenter(RealVec3 c)
	{
		c[0] = m_Center[0];
		c[1] = m_Center[1];
		c[2] = m_Center[2];
	}

	void MoveCenter(const RealVec3 delta)
	{
		m_Center[0] += delta[0];
		m_Center[1] += delta[1];
		m_Center[2] += delta[2];
	}

	void SetTarget(const RealVec3 t)
	{
		m_Target[0] = t[0];
		m_Target[1] = t[1];
		m_Target[2] = t[2];
	}

	void GetTarget(RealVec3 t)
	{
		t[0] = m_Target[0];
		t[1] = m_Target[0];
		t[2] = m_Target[0];
	}

	void MoveTarget(RealVec3 delta)
	{
		m_Target[0] += delta[0];
		m_Target[1] += delta[1];
		m_Target[2] += delta[2];
	}
	
protected:
	void UpdateWanderBehavour(TimeValue deltaTime);
	void UpdateHoverBehavour(TimeValue deltaTime);
	void UpdateFleeBehavour(TimeValue deltaTime);
	void UpdateBoidBehavour(TimeValue deltaTime);

	uint8	m_nNumButterflies;
	Butterfly * m_pButterflies;

	RealVec3	m_Center;
	RealVec3	m_Target;
	Real		m_Boundary;
	BEHAVIOUR	m_ActiveBehaviour;
	
	Texture		m_Texture1, m_Texture2;
};

#endif