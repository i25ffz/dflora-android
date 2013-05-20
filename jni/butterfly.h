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

#ifndef _BUTTERFLY_H_
#define _BUTTERFLY_H_

#include "model.h"
#include "texture.h"

/** A specific skin model of butterfly derived from Model.
	@remarks
	The animation of swaying wings of Butterfly is not predefined key-frame interpolation.
	As the bone of the model could act something like a mass-spring system. The angle 
	displacement wings of butterfly is dynamically calculated each frame using 2nd order 
	dynamic equation. Giving an initial spring offset and setting the damping to zero, the
	wings will flutter forever.
	@par
	On larger scale, the motion of butterfly is an autonomous behaviour. The motion control 
	of Butterfly is adapted from OpenSteer, for more info see the website of C. W. Reynolds:
	http://www.red.com/cwr/
	@see
	Model, ButterflyGroup
	
*/

class Butterfly : public Model
{
	friend class ButterflyGroup;
public:
	Butterfly();
	virtual ~Butterfly();
	void	Create();
	void	CreateBones();
protected:
	void	TransformVerticesFromBoneSpaceToModelSpace();

public:
	void	Initial();

	void	GetPosition(RealVec3 pos) const;
	void	SetPosition(const RealVec3 pos);
	void	PredictPosition(TimeValue deltaTime, RealVec3 pos) const;

	Real	GetSpeed() const { return LengthVec3(m_Velocity); }
	void	SetSpeed(Real s);
	void	SetVelocity(const RealVec3 v);

	Real	GetMinSpeed() { return m_SpeedMin; }
	void	SetMinSpeed(Real sm) { m_SpeedMin = sm; }

	Real	GetMaxSpeed() const { return m_SpeedMax; }
	void	SetMaxSpeed(Real sm) { m_SpeedMax = sm; }

	void	SetAcceleration(const RealVec3 a);
	void	SetMaxAcceleration(Real am) { m_AccelerationMax = am; }
	
	// local space orientation
	void	SetUp(const RealVec3 v);
	void	GetUp(RealVec3 v) const;
	void	SetSide(const RealVec3 v);
	void	GetSide(RealVec3 v) const;
	void	SetForward(const RealVec3 v);
	void	GetForward(RealVec3 v) const;

	Real	GetRadius() const;
public:
	void	RespondToBoundary(const RealVec3 center, Real radius, RealVec3 steer);
	void	ClampAboveTerrain(const RealVec3 pos);
	void	ClampPosition(TimeValue deltaTime);
	void	Update(TimeValue deltaTime);
	
	void	Draw();

protected:
	void	RegenerateLocalSpace(const RealVec3 vNewForward);
	void	SteerForWander(TimeValue deltaTime, RealVec3 steer);
	void	SteerForSeek(const RealVec3 target, RealVec3 steer);
	void	SteerForFlee(const RealVec3 target, RealVec3 steer);

	bool	IsInBoidNeighborhood(const Butterfly* other, Real minDistance, Real maxDistance, Real cosMaxAngle);
	void	SteerForSeparation(Real maxDistance, Real cosMaxAngle, const Butterfly* pGroup, int nNumbers, RealVec3 steer);
	void	SteerForAlignment(Real maxDistance, Real cosMaxAngle, const Butterfly* pGroup, int nNumbers, RealVec3 steer);
	void	SteerForCohesion(Real maxDistance, Real cosMaxAngle, const Butterfly* pGroup, int nNumbers, RealVec3 steer);

	void	ApplySteeringForce(TimeValue deltaTime);

	RealVec3	* m_pVerticesBS;	// vertex coordination in bone space, will not change during animatoin
	RealVec3	* m_pNormalsBS;		// vertex normal in bone space, will not change during animation

	RealVec3	m_Velocity;			// maybe velocity direction is not forward
	RealVec3	m_Acceleration;		// acceleratoin = force / mass;

	Real		m_Radius;			// size of bounding sphere, for obstacle avoidance, etc.
	Real		m_AccelerationMax;	// the maximum steering force this vehicle can apply
									// (steering force is clipped to this magnitude)
	
	// velocity's magnitude (speed) is clipped to [m_fSpeedMin, m_fSpeedMax]
	Real		m_SpeedMax;			// the maximum speed this agent is allowed to move
	Real		m_SpeedMin;			// the minimum speed this agent is allowed to move

	RealVec3	m_AccelerationWander;
};
#endif