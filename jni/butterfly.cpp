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
#include "butterfly.h"

Butterfly::Butterfly() 
{
	m_pVerticesBS	= NULL;
	m_pNormalsBS	= NULL;
}

Butterfly::~Butterfly()
{
	SafeDeleteArray(m_pVerticesBS);
	SafeDeleteArray(m_pNormalsBS);
}

void Butterfly::Create()
{
	uint16 i;
	Real unit = FTOR(1.5f);
	m_nNumVertices = 8;
	m_pVerticesBS = new RealVec3[m_nNumVertices];
	m_pVertices = new RealVec3[m_nNumVertices];
	m_pVerticesTM = new RealVec3[m_nNumVertices];
	
	m_pVerticesBS[0][0] = m_pVerticesBS[4][0] = R_ZERO;
	m_pVerticesBS[0][1] = m_pVerticesBS[4][1] = unit;
	m_pVerticesBS[0][2] = m_pVerticesBS[4][2] = R_ZERO;

	m_pVerticesBS[1][0] = m_pVerticesBS[5][0] = R_ZERO;
	m_pVerticesBS[1][1] = m_pVerticesBS[5][1] = -unit;
	m_pVerticesBS[1][2] = m_pVerticesBS[5][2] = R_ZERO;

	m_pVerticesBS[2][0] = m_pVerticesBS[6][0] = R_ZERO;
	m_pVerticesBS[2][1] = m_pVerticesBS[6][1] = -unit;
	m_pVerticesBS[2][2] = m_pVerticesBS[6][2] = unit;

	m_pVerticesBS[3][0] = m_pVerticesBS[7][0] = R_ZERO;
	m_pVerticesBS[3][1] = m_pVerticesBS[7][1] = unit;
	m_pVerticesBS[3][2] = m_pVerticesBS[7][2] = unit;

	m_pTexCoords = new RealVec2[m_nNumVertices];
	m_pTexCoords[0][0] = m_pTexCoords[4][0] = R_ONE;
	m_pTexCoords[0][1] = m_pTexCoords[4][1] = R_ONE;

	m_pTexCoords[1][0] = m_pTexCoords[5][0] = R_ONE;
	m_pTexCoords[1][1] = m_pTexCoords[5][1] = R_ZERO;

	m_pTexCoords[2][0] = m_pTexCoords[6][0] = R_ZERO;
	m_pTexCoords[2][1] = m_pTexCoords[6][1] = R_ZERO;

	m_pTexCoords[3][0] = m_pTexCoords[7][0] = R_ZERO;
	m_pTexCoords[3][1] = m_pTexCoords[7][1] = R_ONE;

	m_pNormalsBS = new RealVec3[m_nNumVertices];
	m_pNormals = new RealVec3[m_nNumVertices];
	m_pNormalsTM = new RealVec3[m_nNumVertices];

	for(i=0; i<m_nNumVertices; i++)
	{
		m_pNormals[i][0] = R_ZERO;
		m_pNormals[i][1] = R_ZERO;
		m_pNormals[i][2] = R_ONE;
	}

	m_pBoneID = new uint8[m_nNumVertices];
	int num = m_nNumVertices/2;
	for(i=0; i<m_nNumVertices; i++)
	{
		if(i<num)
			m_pBoneID[i] = 1;
		else
			m_pBoneID[i] = 2;		
	}
	m_pBoneWeight = new Real[m_nNumVertices];
	for(i=0; i<m_nNumVertices; i++)
	{
		m_pBoneWeight[i] = R_ONE;
	}


	m_nNumIndices = 12;
	m_pIndices = new GLushort[m_nNumIndices];
	
	m_pIndices[0] = 0;
	m_pIndices[1] = 1;
	m_pIndices[2] = 2;

	m_pIndices[3] = 0;
	m_pIndices[4] = 2;
	m_pIndices[5] = 3;

	m_pIndices[6] = 4;
	m_pIndices[7] = 5;
	m_pIndices[8] = 6;

	m_pIndices[9] = 4;
	m_pIndices[10] = 6;
	m_pIndices[11] = 7;

	m_nNumMaterials = 1;
	m_pMaterials = new Model::Material[m_nNumMaterials];
	m_pMaterials[0].indices = 12;

	m_pMaterials[0].ambient[0] = R_ZERO;
	m_pMaterials[0].ambient[1] = R_ZERO;
	m_pMaterials[0].ambient[2] = R_ZERO;
	m_pMaterials[0].ambient[3] = R_ZERO;

	m_pMaterials[0].diffuse[0] = R_ZERO;
	m_pMaterials[0].diffuse[1] = R_ZERO;
	m_pMaterials[0].diffuse[2] = R_ZERO;
	m_pMaterials[0].diffuse[3] = R_ZERO;

	m_pMaterials[0].specular[0] = R_ZERO;
	m_pMaterials[0].specular[1] = R_ZERO;
	m_pMaterials[0].specular[2] = R_ZERO;
	m_pMaterials[0].specular[3] = R_ZERO;

	m_pMaterials[0].shininess = R_ZERO;
	m_pMaterials[0].transparency = R_ZERO;

	CreateBones();
	TransformVerticesFromBoneSpaceToModelSpace();
	SetupModelSpaceToBoneSpaceTM();
}

void Butterfly::CreateBones()
{
	RealVec3 py = {R_ZERO, R_ONE, R_ZERO};		// positive y
	RealVec3 my = {R_ZERO, -R_ONE, R_ZERO};		// minus y
	m_nNumBones = 3;
	m_pBones = new Bone[m_nNumBones];

	Bone* bone = m_pBones;
	bone->Create();
	bone->SetParent(-1);
	bone++;

	bone->Create();
	bone->SetParent(0);
	bone->SetFlex(FTOR(2.0));
	bone->SetInverseInertia(FTOR(80));

	SetQuat4FromAngleAxis(bone->m_RotateOrg, FTOR(0.314), py);

	bone->SetExternalTorque(py, FTOR(2));
	bone++;

	bone->Create();
	bone->SetParent(0);
	bone->SetFlex(FTOR(2.0));
	bone->SetInverseInertia(FTOR(80));

	SetQuat4FromAngleAxis(bone->m_RotateOrg, FTOR(0.314), my);

	bone->SetExternalTorque(my, FTOR(2));
}

void Butterfly::TransformVerticesFromBoneSpaceToModelSpace()
{
	int16 i = 0;
	RealVec3 * lvert = m_pVerticesBS;
	RealVec3 * gvert = m_pVertices;
	RealVec3 * lnorm = m_pNormalsBS;
	RealVec3 * gnorm = m_pNormals;

	for(i=0; i<m_nNumVertices; i++, lvert++, gvert++, lnorm++, gnorm++)
	{
		uint8 boneID = m_pBoneID[i];
		if (boneID<m_nNumBones)
		{
			Bone & bone = m_pBones[boneID];
			RealMat4 mat;
			LoadIdentityMat4(mat);
			SetRotationMat4FromQuat4(mat, bone.m_RotateAbs);
			ScaleMat4ByVec3(mat, bone.m_ScaleFactor);

			// tranlate is the sum of root bone and model
			mat[12] = bone.m_TranslateAbs[0];
			mat[13] = bone.m_TranslateAbs[1];
			mat[14] = bone.m_TranslateAbs[2];
			
			TransformVec3ByMat4(*lvert, mat, *gvert);
			RotateVec3ByMat4(*lnorm, mat, *gnorm);
		}
	}
}

void Butterfly::Initial()
{
	m_Velocity[0] = R_ZERO;
	m_Velocity[1] = R_ZERO;
	m_Velocity[2] = R_ZERO;

	m_Acceleration[0] = R_ZERO;
	m_Acceleration[1] = R_ZERO;
	m_Acceleration[2] = R_ZERO;

	m_AccelerationMax = FTOR(10);

	m_SpeedMax = FTOR(10);			// the maximum speed this agent is allowed to move
	m_SpeedMin = R_ZERO;			// the minimum speed this agent is allowed to move
	m_AccelerationWander[0] = R_ZERO;
	m_AccelerationWander[1] = R_ZERO;
	m_AccelerationWander[2] = R_ZERO;

	RealVec3 pos = {R_ZERO, R_ZERO, FTOR(15.0)};		
	SetPosition(pos);
}

void Butterfly::SetVelocity(const RealVec3 v)
{
	m_Velocity[0] = v[0];
	m_Velocity[1] = v[1];
	m_Velocity[2] = v[2];
}

void Butterfly::SetAcceleration(const RealVec3 a)
{
	m_Acceleration[0] = a[0];
	m_Acceleration[1] = a[1];
	m_Acceleration[2] = a[2];
}

void Butterfly::ApplySteeringForce(TimeValue deltaTime)
{
	Real rate = FTOR(0.5);
	RealVec3 newVelocity;
	Real dt_s = Quotient(ITOR(deltaTime), ITOR(1000));

	newVelocity[0] = m_Velocity[0] + Product(m_Acceleration[0], dt_s);
	newVelocity[1] = m_Velocity[1] + Product(m_Acceleration[1], dt_s);
	newVelocity[2] = m_Velocity[2] + Product(m_Acceleration[2], dt_s);

	// smooth velocity, damping factor
	m_Velocity[0] = Interpolate(m_Velocity[0], newVelocity[0], rate);
	m_Velocity[1] = Interpolate(m_Velocity[1], newVelocity[1], rate);
	m_Velocity[2] = Interpolate(m_Velocity[2], newVelocity[2], rate);

	// add turbulence
	m_Velocity[0] += Product(Product((Rand01()-R_HALF), FTOR(0.05)), m_SpeedMax);
	m_Velocity[1] += Product(Product((Rand01()-R_HALF), FTOR(0.05)), m_SpeedMax);
	m_Velocity[2] += Product(Product((Rand01()-R_HALF), FTOR(0.013)), m_SpeedMax);
	
	ClipVec3(m_Velocity, m_SpeedMin, m_SpeedMax);
	
	RealVec3 Pos;
	GetPosition(Pos);
	Pos[0] += Product(m_Velocity[0], dt_s);
	Pos[1] += Product(m_Velocity[1], dt_s);
	Pos[2] += Product(m_Velocity[2], dt_s);
	SetPosition(Pos);
}

void Butterfly::Update(TimeValue deltaTime)		// time in msec
{
	ApplySteeringForce(deltaTime);
	RealVec3 forward;
	forward[0] = m_Velocity[0];
	forward[1] = m_Velocity[1];
	if(m_Velocity[2] > FTOR(0.2) || m_Velocity[2] < FTOR(-0.2))
		forward[2] =  m_Velocity[2] > R_ZERO ? FTOR(0.2) : FTOR(-0.2);
	else
		forward[2] = m_Velocity[2];

	RegenerateLocalSpace(forward);
	UpdateSkeleton(deltaTime);
}

void Butterfly::Draw()
{
	// optional tranform/blend vertices using matrix palette, here I blend by myself
	TransformSkinMesh();

	glTexCoordPointer(2, GL_REAL, 0, m_pTexCoords);
	glVertexPointer(3, GL_REAL, 0, m_pVerticesTM);
	glNormalPointer(GL_REAL, 0, m_pNormalsTM);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, m_pIndices);
}

void Butterfly::RegenerateLocalSpace(const RealVec3 newForward)
{
	RealVec3 forward;
	forward[0] = newForward[0];
	forward[1] = newForward[1];
	forward[2] = newForward[2];
	
	if(NormalizeVec3(forward) <= FTOR(0.001))
		return;

	NormalizeVec3(forward);
	RealVec3 side = {forward[1], -forward[0], R_ZERO};
	NormalizeVec3(side);
	RealVec3 up;
	CrossProductVec3(up, side, forward);

	SetForward(forward);
	SetSide(side);
	SetUp(up);
}

void Butterfly::GetPosition(RealVec3 pos) const
{
	GetRootPositionAbs(pos);
}

void Butterfly::SetPosition(const RealVec3 pos)
{
	SetRootPositionAbs(pos);
}

void Butterfly::PredictPosition (TimeValue deltaTime, RealVec3 pos) const
{
	Real dt_s = Quotient(ITOR(deltaTime), ITOR(1000));	// more general
	Real rate = FTOR(0.21);
	RealVec3 newVelocity;
	newVelocity[0] = m_Velocity[0] + Product(m_Acceleration[0], dt_s);
	newVelocity[1] = m_Velocity[1] + Product(m_Acceleration[1], dt_s);
	newVelocity[2] = m_Velocity[2] + Product(m_Acceleration[2], dt_s);

	// smooth velocity, damping factor
	RealVec3 vVelocity;
	vVelocity[0] = Interpolate(m_Velocity[0], newVelocity[0], rate);
	vVelocity[1] = Interpolate(m_Velocity[1], newVelocity[1], rate);
	vVelocity[2] = Interpolate(m_Velocity[2], newVelocity[2], rate);

	ClipVec3(vVelocity, m_SpeedMin, m_SpeedMax);
	RealVec3 dis;
	dis[0] = Product(vVelocity[0], dt_s);
	dis[1] = Product(vVelocity[1], dt_s);
	dis[2] = Product(vVelocity[2], dt_s);
	
	GetPosition(pos);
	pos[0] += dis[0];
	pos[1] += dis[1];
	pos[2] += dis[2];
}

void Butterfly::SetSide(const RealVec3 v)
{
	m_TMModel2World[0] = v[0];
	m_TMModel2World[1] = v[1];
	m_TMModel2World[2] = v[2];
}

void Butterfly::GetSide(RealVec3 v) const
{
	v[0] = m_TMModel2World[0];
	v[1] = m_TMModel2World[1];
	v[2] = m_TMModel2World[2];
}

void Butterfly::SetForward(const RealVec3 v)
{
	m_TMModel2World[4] = v[0];
	m_TMModel2World[5] = v[1];
	m_TMModel2World[6] = v[2];
}

void Butterfly::GetForward(RealVec3 v) const
{
	v[0] = m_TMModel2World[4];
	v[1] = m_TMModel2World[5];
	v[2] = m_TMModel2World[6];
}

void Butterfly::SetUp(const RealVec3 v)
{
	m_TMModel2World[8] = v[0];
	m_TMModel2World[9] = v[1];
	m_TMModel2World[10] = v[2];
}

void Butterfly::GetUp(RealVec3 v) const
{
	v[0] = m_TMModel2World[8];
	v[1] = m_TMModel2World[9];
	v[2] = m_TMModel2World[10];
}

Real Butterfly::GetRadius() const
{
	return FTOR(0.2f);
}

void Butterfly::SteerForWander(TimeValue deltaTime, RealVec3 steer)
{	
    Real speed = Product(ITOR(deltaTime), FTOR(0.02));
	
	m_AccelerationWander[0] += Product((R_HALF-Rand01()), speed);
	if(m_AccelerationWander[0] < -m_AccelerationMax)	
		m_AccelerationWander[0] = -m_AccelerationMax;
	if(m_AccelerationWander[0] > m_AccelerationMax)	
		m_AccelerationWander[0] = m_AccelerationMax;	

	m_AccelerationWander[1] += Product((R_HALF-Rand01()), speed);
	if(m_AccelerationWander[1] < -m_AccelerationMax)	
		m_AccelerationWander[1] = -m_AccelerationMax;
	if(m_AccelerationWander[1] > m_AccelerationMax)	
		m_AccelerationWander[1] = m_AccelerationMax;

	m_AccelerationWander[2] += Product((R_HALF-Rand01()), speed);
	if(m_AccelerationWander[2] < -m_AccelerationMax)	
		m_AccelerationWander[2] = -m_AccelerationMax;
	if(m_AccelerationWander[2] > m_AccelerationMax)	
		m_AccelerationWander[2] = m_AccelerationMax;	


    steer[0] = m_AccelerationWander[0];
	steer[1] = m_AccelerationWander[1];
	steer[2] = m_AccelerationWander[2];

	ClipVec3(steer, 0, m_AccelerationMax);
}

void Butterfly::SteerForSeek(const RealVec3 target, RealVec3 steer)
{
	RealVec3 desiredVelocity;
	RealVec3 pos;
	GetPosition(pos);
	desiredVelocity[0] = target[0] - pos[0];
	desiredVelocity[1] = target[1] - pos[1];
	desiredVelocity[2] = target[2] - pos[2];
    
	steer[0] = desiredVelocity[0] - m_Velocity[0];
	steer[1] = desiredVelocity[1] - m_Velocity[1];
	steer[2] = desiredVelocity[2] - m_Velocity[2];

	ClipVec3(steer, 0, m_AccelerationMax);
}

void Butterfly::SteerForFlee(const RealVec3 target, RealVec3 steer)
{
	RealVec3 desiredVelocity;
	RealVec3 pos;
	GetPosition(pos);
	desiredVelocity[0] = pos[0] - target[0];
	desiredVelocity[1] = pos[1] - target[1];
	desiredVelocity[2] = pos[2] - target[2];
    
	steer[0] = desiredVelocity[0] - m_Velocity[0];
	steer[1] = desiredVelocity[1] - m_Velocity[1];
	steer[2] = desiredVelocity[2] - m_Velocity[2];

	ClipVec3(steer, 0, m_AccelerationMax);
}

bool Butterfly::IsInBoidNeighborhood(const Butterfly* other, Real minDistance, Real maxDistance, Real cosMaxAngle)
{
    if (other == this)       
		return false;
    else
    {
		RealVec3 offset;
		RealVec3 posself;
		RealVec3 posother;
		GetPosition(posother);
		GetPosition(posself);
		offset[0] = posother[0] - posself[0];
		offset[1] = posother[1] - posself[1];
		offset[2] = posother[2] - posself[2];

		Real distance = NormalizeVec3(offset);

        // definitely in neighborhood if inside minDistance sphere
        if (distance < minDistance)		
			return true;
        else
        {
            // definitely not in neighborhood if outside maxDistance sphere
            if (distance > maxDistance)	
				return false;
            else
            {
                // otherwise, test angular offset from forward axis
				RealVec3 forward;
				GetForward(forward);
                Real forwardness = DotProductVec3(forward, offset);
                return forwardness > cosMaxAngle;
            }
        }
    }
}

void Butterfly::SteerForSeparation(Real maxDistance, Real cosMaxAngle, const Butterfly* pGroup, int nNumbers, RealVec3 steer)
{
    int neighbors = 0;

	for(int i=0; i<nNumbers; i++)
    {
		const Butterfly* btf = pGroup+i;
		Real minDistance = Product(GetRadius(), FTOR(3.0));
        if (IsInBoidNeighborhood(btf, minDistance, maxDistance, cosMaxAngle))
        {
            RealVec3 offset, posself, posother;
			GetPosition(posself);
			btf->GetPosition(posother);
			
			offset[0] = posself[0] - posother[0];
			offset[1] = posself[1] - posother[1];
			offset[2] = posself[2] - posother[2];
			
			Real offset2 = DotProductVec3(offset, offset);
			Real r = GetRadius(); 
			Real r2 = Product(r, r);
			if(offset2 > r2)
			{
				steer[0] += Quotient(offset[0], offset2);
				steer[1] += Quotient(offset[1], offset2);
				steer[2] += Quotient(offset[2], offset2);
			}
			else
			{
				Real r25 = Product(r2, FTOR(5));
				steer[0] += Quotient(offset[0], r25);
				steer[1] += Quotient(offset[1], r25);
				steer[2] += Quotient(offset[2], r25);
			}
            neighbors++;
        }
    }
    if (neighbors > 0)
	{
		steer[0] = Quotient(steer[0], ITOR(neighbors));
		steer[1] = Quotient(steer[1], ITOR(neighbors));
		steer[2] = Quotient(steer[2], ITOR(neighbors));
	}
    ClipVec3(steer, 0, m_AccelerationMax);
}

void Butterfly::SteerForAlignment(Real maxDistance, Real cosMaxAngle, const Butterfly* pGroup, int nNumbers, RealVec3 steer)
{
    int neighbors = 0;

	for(int i=0; i<nNumbers; i++)
    {
		const Butterfly* btf = pGroup+i;
		Real minDistance = Product(GetRadius(), FTOR(3.0));
        if (IsInBoidNeighborhood(btf, minDistance, maxDistance, cosMaxAngle))
        {
            RealVec3 forw;
			btf->GetForward(forw);			
			steer[0] += forw[0];
			steer[1] += forw[1];
			steer[2] += forw[2];
            neighbors++;
        }
    }
    if (neighbors > 0)
	{
		RealVec3 forward;
		GetForward(forward);
		steer[0] = Quotient(steer[0], ITOR(neighbors)) - forward[0];
		steer[1] = Quotient(steer[1], ITOR(neighbors)) - forward[1];
		steer[2] = Quotient(steer[2], ITOR(neighbors)) - forward[2];
	}
    ClipVec3(steer, 0, m_AccelerationMax);
}

void Butterfly::SteerForCohesion(Real maxDistance, Real cosMaxAngle, const Butterfly* pGroup, int nNumbers, RealVec3 steer)
{
    int neighbors = 0;

	for(int i=0; i<nNumbers; i++)
    {
		const Butterfly* btf = pGroup+i;
		Real minDistance = Product(GetRadius(), FTOR(3.0));
        if (IsInBoidNeighborhood(btf, minDistance, maxDistance, cosMaxAngle))
        {
            RealVec3 pos;
			btf->GetPosition(pos);
			steer[0] += pos[0];
			steer[1] += pos[1];
			steer[2] += pos[2];
            neighbors++;
        }
    }
    if (neighbors > 0)
	{
		RealVec3 position;
		GetPosition(position);
		steer[0] = Quotient(steer[0], ITOR(neighbors)) - position[0];
		steer[1] = Quotient(steer[1], ITOR(neighbors)) - position[1];
		steer[2] = Quotient(steer[2], ITOR(neighbors)) - position[2];
	}
    ClipVec3(steer, 0, m_AccelerationMax);
}

void Butterfly::RespondToBoundary(const RealVec3 center, Real radius, RealVec3 steer)
{
	RealVec3 pos, dis;
	GetPosition(pos);

	dis[0] = center[0] - pos[0];
	dis[1] = center[1] - pos[1];
	dis[2] = center[2] - pos[2];

	Real len = NormalizeVec3(dis);
	if(len < radius)
	{
		steer[0] = R_ZERO;
		steer[1] = R_ZERO;
		steer[2] = R_ZERO;
		return;
	}
	
	Real delta = len - radius;

	Real temp = Product(delta, m_AccelerationMax);
	steer[0] = Product(dis[0], temp);
	steer[1] = Product(dis[1], temp);
	steer[2] = Product(dis[2], temp);
    ClipVec3(steer, 0, m_AccelerationMax);
}

void Butterfly::ClampAboveTerrain(const RealVec3 pos)
{
	Real speed = GetSpeed();
	m_Acceleration;
	Real height = R_ZERO;
	Real delta = height + FTOR(1) - pos[2];
	if(delta>R_ZERO)
	{
		delta = Product(delta, FTOR(0.5));
		if(delta>R_ONE)	delta = R_ONE;

		RealVec3 newvel;
		newvel[0] = Product(m_Velocity[0], FTOR(0.5));
		newvel[1] = Product(m_Velocity[1], FTOR(0.5));
		Real speedmax = Product(m_SpeedMax, FTOR(0.3));
		if(m_Velocity[2] > speedmax)
			newvel[2] = m_Velocity[2];
		else
			newvel[2] = speedmax;
		
		m_Velocity[0] = Interpolate(m_Velocity[0], newvel[0], delta);
		m_Velocity[1] = Interpolate(m_Velocity[1], newvel[1], delta);
		m_Velocity[2] = Interpolate(m_Velocity[2], newvel[2], delta);

		NormalizeVec3(m_Velocity);
		m_Velocity[0] = Product(m_Velocity[0], speed);
		m_Velocity[1] = Product(m_Velocity[1], speed);
		m_Velocity[2] = Product(m_Velocity[2], speed);
	}
}

void Butterfly::ClampPosition(TimeValue deltaTime)
{
	TimeValue dt = deltaTime*5;
	RealVec3 newpos;
	PredictPosition(dt, newpos);

	ClampAboveTerrain(newpos);
}

