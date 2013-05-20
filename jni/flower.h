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

#ifndef _FLOWER_H_
#define _FLOWER_H_

#include "model.h"

/** The dancing animation records for the Flower model.
	@remarks
	Script records a set of static locally deformed postures and global motion command.
	Flower Model will read Script and animate accroding to the script. As flower model
	is driven dynamically, the posture is actually some kinematic (such as velocity, 
	acceleration) or dynamic (such as torques and forces exerted on the bones) parameters, 
	in stead of geometrical parameters such as translation and rotation something often used
	in traditional key-frame animation.
	@see
	Flower, Flower::Posture
*/
class Script
{
public:
	/** The smallest unit of script.
	*/
	typedef struct _BeatUnit
	{
	public:
		int16		m_PoseIndex[6];		// posture ID, maxmium 6
		Real		m_PoseExtent[6];	// posture extent
		
		RealVec3	m_Velocity;
		Real		m_Acceleration;		// point velocity and acceleration
		
		Real		m_WhirlVel;			// whirl angular velocity			
		Real		m_WhirlAcc;			// whirl angular acceleration
		
		RealVec3	m_FlipAxis;
		Real		m_FlipVel;			// flip angular velocity
		uint8		m_FlipRound;		// number of flip round 360*n
		
		uint8		m_Action;			// Action mask: b7-------b0: b0 - jump, b1 - whirl, b2 - flip
		
		bool Create()
		{
			m_PoseIndex[0] = m_PoseIndex[1] = m_PoseIndex[2] = -1;
			m_PoseIndex[3] = m_PoseIndex[4] = m_PoseIndex[5] = -1;
			
			m_PoseExtent[0] = m_PoseExtent[1] = m_PoseExtent[2] = R_ONE;
			m_PoseExtent[3] = m_PoseExtent[4] = m_PoseExtent[5] = R_ONE;
			
			m_Velocity[0] = m_Velocity[1] = m_Velocity[2] = 0;
			m_Acceleration = 0;
			
			m_WhirlVel = m_WhirlAcc = 0;
			
			m_FlipAxis[0] = R_ONE;
			m_FlipAxis[1] = 0;
			m_FlipAxis[2] = 0;					
			m_FlipVel	= 0;
			m_FlipRound	= 0;
			
			m_Action = 0x0;
			return TRUE;
		}
	} BeatUnit;

public:	
	uint16		m_nNumBeatUnit;
	BeatUnit	* m_pBeatUnits;


	Script();
	bool Create()	{ m_nNumBeatUnit = 0 ; m_pBeatUnits = NULL; return TRUE; }
	void Destory()	{ SafeDeleteArray(m_pBeatUnits);	m_nNumBeatUnit = 0; }
	bool Load(const char * filename);
};

/** A specific skin model of flower derived from Model.
	@remarks
	Flower add some dancing features to Model. Flower can grow and dance.
	When it grows, each bone of its skeleton can be scaled seperately, and could deform the model
	in various way. When it dances, it reads Script and dancing accroding to it. All the animation
	are controlled dynamically, and need not any key-frame infomation at all.
	
	@see
	Model, Script
*/

class Flower : public Model
{
public:
	/** An instance of skeleton status.
		Here a set of postures are predefined in the Script for the Flower to interpolate 
		between.
		@see
		Script
	*/
	typedef struct _Posture
	{
	public:
		void Create(uint8 num)
		{ 
			m_nNumBones	= num;
			m_pBoneID = new uint8[num];
			m_pTorques = new RealVec3[num];
		}
		
		void Destory()
		{
			SafeDeleteArray(m_pBoneID);
			SafeDeleteArray(m_pTorques);
		}
		
		uint8		m_nNumBones;	// number of affected bones
		uint8		* m_pBoneID;
		RealVec3	* m_pTorques;		
	} Posture;

	Flower();
	~Flower();
	bool Load(const char* filename);

	void UpdateJump(TimeValue deltatime);
	void UpdateWhirl(TimeValue deltatime);

	void SelectPosture(int index, Real extent = R_ONE);

	int  ParseScript(Script * script, int pos);
	void Jump(RealVec3 v, Real a);
	void Whirl(Real w, Real a);

	void StartGrowth();
	void StopGrowth();
	void UpdateGrowth(TimeValue deltatime);

	void SetAge(Real age);
	Real GetAge() { return m_Age; }

	void StartDance(Script * script, TimeValue beatinterval);
	void StopDance();
	void UpdateDance(TimeValue deltatime);

protected:
	
	// typical prereserved postures
	int		m_nNumPostures;
	Posture		* m_pPostures;

	// current dance script
	Script		* m_pScript;
	int		m_nScriptPos;

	// global motions
	// jump
	RealVec3 m_Velocity;		// global velocity of root joint
	Real	m_Acceleration;		// global acceleration of root joint in Z-direction

	// whirl == rotate round z-axis
	Real		m_WhirlDis;		// global whirl displacement
	Real		m_WhirlVel;		// global whirl angular velocity			
	Real		m_WhirlAcc;		// global whirl angular acceleration

	//  time related to dancing
	TimeValue	m_nTimeRemain;		// remaining time of one beat unit 
	TimeValue	m_nBeatInterval;	// interval between two beat unit

	Real		m_Age;				// [0,1] to control grow process
};

#endif