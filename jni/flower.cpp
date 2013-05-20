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

#include <stdio.h>
#include "flower.h"

Script::Script()
{
	m_pBeatUnits = NULL;
	m_nNumBeatUnit = 0;
}

bool Script::Load(const char * filename)
{
	uint16 i;
	FILE * file = fopen(filename, "rb");
	if(file == NULL)		return FALSE;

	if(fread(&m_nNumBeatUnit, sizeof(uint16), 1, file) != 1)
		return FALSE;
	
	m_pBeatUnits = new BeatUnit[m_nNumBeatUnit];

	for (i=0; i<m_nNumBeatUnit; i++)
	{
		BeatUnit & bu = m_pBeatUnits[i];
		if(fread(bu.m_PoseIndex, sizeof(int16), 6, file) != 6)
			return FALSE;

		if(fread(bu.m_PoseExtent, sizeof(Real), 6, file) != 6)
			return FALSE;

		if(fread(bu.m_Velocity, sizeof(RealVec3), 1, file) != 1)
			return FALSE;

		if(fread(&(bu.m_Acceleration), sizeof(Real), 1, file) != 1)
			return FALSE;

		if(fread(&(bu.m_WhirlVel), sizeof(Real), 1, file) != 1)
			return FALSE;

		if(fread(&(bu.m_WhirlAcc), sizeof(Real), 1, file) != 1)
			return FALSE;

		if(fread(bu.m_FlipAxis, sizeof(RealVec3), 1, file) != 1)
			return FALSE;

		if(fread(&(bu.m_FlipVel), sizeof(Real), 1, file) != 1)
			return FALSE;

		if(fread(&(bu.m_FlipRound), sizeof(uint8), 1, file) != 1)
			return FALSE;

		if(fread(&(bu.m_Action), sizeof(uint8), 1, file) != 1)
			return FALSE;
	}	

	fclose(file);
	return TRUE;
}

Flower::Flower()
{
	m_nNumPostures	= 0;
	m_pPostures		= NULL;

	m_pScript		= NULL;
	m_nScriptPos	= 0;

	m_Velocity[0] = m_Velocity[1] = m_Velocity[2] = R_ZERO;
	m_Acceleration = R_ZERO;

	m_WhirlDis = 0;
	m_WhirlVel = 0;
	m_WhirlAcc = 0;

	m_nNumPostures	= 0;
	m_pPostures		= NULL;
	
	m_Age			= FTOR(1);
}

Flower::~Flower()
{
	if(m_pPostures)
	{
		for(uint16 i=0; i<m_nNumPostures; i++)
			m_pPostures[i].Destory();
		SafeDeleteArray(m_pPostures);
	}
	m_pScript = 0;
}

bool Flower::Load(const char * filename)
{
	uint16 i;
	FILE * file = fopen(filename, "rb");
	if(file == NULL)	return FALSE;
	
	// Vertices
	if(fread(&m_nNumVertices, sizeof(uint16), 1, file) != 1)
		return FALSE;

	m_pVertices = new RealVec3[m_nNumVertices];
	m_pVerticesTM = new RealVec3[m_nNumVertices];

	m_pBoneID = new uint8[m_nNumVertices];
	m_pBoneWeight = new Real[m_nNumVertices];

	if(fread(m_pVertices, sizeof(RealVec3), m_nNumVertices, file) != m_nNumVertices)
		return FALSE;

	if(fread(m_pBoneID, sizeof(uint8), m_nNumVertices, file) != m_nNumVertices)
		return FALSE;

	// initialize vertices weight
	for(i=0; i<m_nNumVertices; i++)
	{
		m_pBoneWeight[i] = R_ONE;
	}


	// Normals
	m_pNormals = new RealVec3[m_nNumVertices];
	m_pNormalsTM = new RealVec3[m_nNumVertices];

	if(fread(m_pNormals, sizeof(RealVec3), m_nNumVertices, file) != m_nNumVertices)
		return FALSE;

	// Indices
	if(fread(&m_nNumIndices, sizeof(uint16), 1, file) != 1)
		return FALSE;

	m_pIndices = new GLushort[m_nNumIndices];

	if(fread(m_pIndices, sizeof(GLushort), m_nNumIndices, file) != m_nNumIndices)
		return FALSE;

	// Materials
	if(fread(&m_nNumMaterials, sizeof(uint16), 1, file) != 1)
		return FALSE;

	m_pMaterials = new Model::Material[m_nNumMaterials];

	for(i=0; i<m_nNumMaterials; i++)
	{
		if(fread(m_pMaterials+i, sizeof(Model::Material), 1, file) != 1)
			return FALSE;
	}

	// Bones
	if(fread(&m_nNumBones, sizeof(uint16), 1, file) != 1)
		return FALSE;

	m_pBones = new Bone[m_nNumBones];
	
	for(i=0; i<m_nNumBones; i++)
	{
		Bone& bone = m_pBones[i];
		bone.Create();
		int8 parent;
		if(fread(&parent, sizeof(int8), 1, file) != 1)
			return FALSE;
		bone.SetParent(parent);

		if(fread(bone.m_TranslateOrg, sizeof(RealVec3), 1, file) != 1)
			return FALSE;
		if(fread(bone.m_RotateOrg, sizeof(RealQuat4), 1, file) != 1)
			return FALSE;
		
		Real temp;
		if(fread(&temp, sizeof(Real), 1, file) != 1)
			return FALSE;
		bone.SetFlex(temp);

		if(fread(&temp, sizeof(Real), 1, file) != 1)
			return FALSE;
		bone.SetInverseInertia(temp);

		if(fread(&temp, sizeof(Real), 1, file) != 1)
			return FALSE;
		bone.SetDamp(temp);
	}

	uint16 numIndices = (uint16)(m_nNumBones<<1);		// m_nNumBones*2
	m_pSkeletonVertices = new RealVec3[m_nNumBones];
	m_pSkeletonIndices = new GLushort[numIndices];
	GLushort * slider = m_pSkeletonIndices;
	for(i=0; i<m_nNumBones; i++)
	{
		int16 pr = m_pBones[i].GetParent();
		*slider++ = pr<0 ? (GLushort)0 : (GLushort) pr;
		*slider++ = (GLushort) i;
	}	

	// Postures
	if(fread(&m_nNumPostures, sizeof(uint16), 1, file) != 1)
		return FALSE;

	m_pPostures = new Posture[m_nNumPostures];

	for(i=0; i<m_nNumPostures; i++)
	{
		Posture& pos = m_pPostures[i];	// alias for short
		uint8 num;
		if(fread(&num, sizeof(uint8), 1, file) != 1)
			return FALSE;		
		pos.Create(num);
		for(uint8 j=0; j<num; j++)
		{
			if(fread(&(pos.m_pBoneID[j]), sizeof(uint8), 1, file) != 1)
				return FALSE;			 
			if(fread(&(pos.m_pTorques[j]), sizeof(RealVec3), 1, file) != 1)
				return FALSE;
		}
	}

	fclose(file);
	SetupModelSpaceToBoneSpaceTM();
	return TRUE;
}

void Flower::SelectPosture(int index, Real extent)
{
	if(index > m_nNumPostures || index < 0)
		return;
	if(extent < R_ZERO)		extent = R_ZERO;
	if(extent > ITOR(2))	extent = ITOR(2);

	uint16 numBone = m_pPostures[index].m_nNumBones;	// number of affected bones
	Posture& post= m_pPostures[index];
	
	for(uint16 i=0; i<numBone; i++)
	{
		m_pBones[post.m_pBoneID[i]].SetExternalTorque(post.m_pTorques[i], extent);
	}
}

void Flower::UpdateJump(TimeValue dt)	// dt: msec
{
	RealVec3 pos;
	GetRootPositionAbs(pos);
	
	Real dt_s = Quotient(ITOR(dt), ITOR(1000));
	
	pos[0] += Product(m_Velocity[0], dt_s);
	pos[1] += Product(m_Velocity[1], dt_s);
	pos[2] += Product(m_Velocity[2], dt_s);

	if(pos[2] > R_ZERO)
		m_Velocity[2] += Product(m_Acceleration, dt_s);
	
	if( pos[2] < R_ZERO)	//  fall down onto the ground
	{
		m_Velocity[0] = R_ZERO;
		m_Velocity[1] = R_ZERO;
		m_Velocity[2] = R_ZERO;
		m_Acceleration = R_ZERO;
		pos[2] = R_ZERO;
	}

	SetRootPositionAbs(pos);
}

void Flower::UpdateWhirl(TimeValue dt)
{
	if(m_WhirlVel == R_ZERO)	return;

	Real VelOld = m_WhirlVel;

	Real dt_s = Quotient(ITOR(dt), ITOR(1000));

	m_WhirlDis += Product(m_WhirlVel, dt_s);
 	m_WhirlVel += Product(m_WhirlAcc, dt_s);

	if( (m_WhirlVel<R_ZERO && VelOld>R_ZERO) || (m_WhirlVel>R_ZERO && VelOld<R_ZERO) )
		m_WhirlVel = R_ZERO;
	
	RealMat4 rot;
	LoadIdentityMat4(rot);
	RealVec3 zaxix = {R_ZERO, R_ZERO, R_ONE};
	SetRotationMat4FromAngleAxisVec3(rot, m_WhirlDis, zaxix);
	PostMultiplyMat4(m_TMModel2World, rot);
}


void Flower::StartDance(Script * script, TimeValue beatinterval)
{
	m_nTimeRemain = 0;
	m_nBeatInterval = beatinterval;

	m_pScript = script;
	m_nScriptPos = 0;
}

void Flower::StopDance()
{
	m_pScript = NULL;
	RealVec3	pos = { R_ZERO, R_ZERO, R_ZERO };
	SetRootPositionAbs(pos);
	SelectPosture(0);
	TimeValue deltatime = 80;
	UpdateSkeleton(deltatime);

	UpdateJump(deltatime);
	UpdateWhirl(deltatime);

	m_pScript = NULL;
	m_nTimeRemain = 0;
}

void Flower::UpdateDance(TimeValue deltatime)
{
	if(m_pScript == NULL)		return;

	if( m_nTimeRemain <= 0 )
		m_nTimeRemain += m_nBeatInterval;

	// usually deltatime can not greater than m_nBeatInterval
	if(deltatime > m_nBeatInterval)
	{
		while(deltatime > m_nBeatInterval)
		{
			deltatime -= m_nBeatInterval;
			m_nScriptPos++;
		}
	}

	//	dance a while
	if(m_nTimeRemain <= deltatime)
		m_nScriptPos = ParseScript(m_pScript, m_nScriptPos);

	if(m_nScriptPos<0)
		return;

	UpdateSkeleton(deltatime);
	UpdateJump(deltatime);
	UpdateWhirl(deltatime);

	m_nTimeRemain -= deltatime;
}

int Flower::ParseScript(Script * script, int pos )
{

	if(script == NULL)		return -1;
	if(script->m_nNumBeatUnit == 0)	return -1;

	if( pos >= script->m_nNumBeatUnit)	// recycle dancing from the begining position
		return -1;

	Script::BeatUnit& bu = script->m_pBeatUnits[pos];

	{
		for(uint8 i=0; i<6; i++)
		{
			int16 pos_index = bu.m_PoseIndex[i];
			if( pos_index > 0 )
				SelectPosture( pos_index, bu.m_PoseExtent[i]);
		}

		if( bu.m_Action & 0x1)
			Jump(bu.m_Velocity, bu.m_Acceleration);

		if( bu.m_Action & 0x2)
			Whirl(bu.m_WhirlVel, bu.m_WhirlAcc);
	}
	
	return pos+1;
}

void Flower::Jump(RealVec3 v, Real a)
{
	RealVec3 pos;
	GetRootPositionAbs(pos);
	if( pos[2] <= FTOR(0.01) ){
		m_Velocity[0] = Product(v[0], ITOR(100));	// 100 is a speed coefficient
		m_Velocity[1] = Product(v[1], ITOR(100));
		m_Velocity[2] = Product(v[2], ITOR(100));
	}
	m_Acceleration = Product(a, ITOR(200));
}

void Flower::Whirl(Real w, Real a)
{
	m_WhirlVel = Product(w, ITOR(50));
	m_WhirlAcc = Product(a, ITOR(25));
}

void Flower::SetAge(Real age)
{
	if(age<FTOR(0.01))	m_Age = FTOR(0.01);
	else if(age>R_ONE)	m_Age = R_ONE;
	else				m_Age = age;

	RealVec3 s = {m_Age, m_Age, m_Age};
	SetScaleFactor(s);
}

void Flower::StartGrowth()
{
	SetAge(FTOR(0.01));
	UpdateSkeleton();
}

void Flower::UpdateGrowth(TimeValue deltatime)
{
	Real age = GetAge();
	Real growthrate = FTOR(0.00009);	// rate in msec
	age += Product(growthrate, ITOR(deltatime));
	SetAge(age);

	uint16 pidx = (uint16) (rand()%m_nNumPostures);
	Real ext = Product(Rand01(), R_HALF)+R_HALF;
	SelectPosture(pidx, ext);

	UpdateSkeleton(deltatime);
}

void Flower::StopGrowth()
{
	UpdateSkeleton();
}
