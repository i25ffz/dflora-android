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

#ifndef _MODEL_H_
#define _MODEL_H_

//#include "windows.h"
#include "utility.h"

/** A physically-based bone in an articulated skeleton.
	@remarks	
	This class is a node in the joint hierarchy. Mesh vertices also have assignments
	to bones to define how they move in relation to the skeleton. A bone is essentially
	a transforming from its parent, and can be regarded as a local coordinate.
	@par
	Besides traditional skeleton animation controlled only with kinematics, Bone has 
	dynmaic parameters and will be controlled by a PD controller, similar with a mass-
	spring system. Thus Bone is also 2nd order dynamic system from the perspetive of 
	dynamics.

*/
class Bone
{	
public:
	Bone() { Create(); }
	~Bone() { }
	void Create();

	void SetParent(int8 parent) { m_ParentID = parent; }
	int8 GetParent() { return m_ParentID; }

	void SetPositionAbs(const RealVec3 v)
	{ 
		memcpy(m_TranslateAbs, v, sizeof(RealVec3));
	}

	void GetPositionAbs(RealVec3 v)
	{
		memcpy(v, m_TranslateAbs, sizeof(RealVec3));
	}

	void	SetInverseInertia(Real i)	{ m_InverseInertia = i; }
	Real	GetInverseInertia() 		{ return m_InverseInertia; }

	void	SetFlex(Real f) { m_Flex = f; }
	Real	GetFlex() 		{ return m_Flex; }

	void	SetDamp(Real d) { m_Damp = d; }
	Real	GetDamp()		{ return m_Damp; }

	void	SetScaleFactor(const RealVec3 v)	// local scale
	{ m_ScaleFactor[0] = v[0]; m_ScaleFactor[1] = v[1]; m_ScaleFactor[2] = v[2]; }
	void	GetScaleFactor(RealVec3 v)
	{ v[0] = m_ScaleFactor[0]; v[1] = m_ScaleFactor[1]; v[2] = m_ScaleFactor[2]; }
		

	void UpdateTransformRel()
	{
		memcpy(m_TranslateRel, m_TranslateOrg, sizeof(RealVec3));

		RealQuat4 disp;
		SetQuat4FromAngleDisp(disp, m_AngularDisplace);
		PostMultiplyQuat4(m_RotateOrg, disp, m_RotateRel);
	}

	void UpdateAngularMotion(TimeValue dt)
	{
		Real tmp = Product(ITOR(dt), m_InverseInertia);	// v=F*dt/m
		tmp = Quotient(tmp, ITOR(1000));

		m_AngularVelocity[0] += Product(m_TorqueTo[0], tmp);
		m_AngularVelocity[1] += Product(m_TorqueTo[1], tmp);
		m_AngularVelocity[2] += Product(m_TorqueTo[2], tmp);

		m_AngularVelocity[0] = Product(m_AngularVelocity[0], R_ONE-m_Damp);
		m_AngularVelocity[1] = Product(m_AngularVelocity[1], R_ONE-m_Damp);
		m_AngularVelocity[2] = Product(m_AngularVelocity[2], R_ONE-m_Damp);

		// convert dt's time unit from millisecond to second, 
		Real dt_s = Quotient(ITOR(dt), ITOR(1000));

		m_AngularDisplace[0] += Product(m_AngularVelocity[0], dt_s);
		m_AngularDisplace[1] += Product(m_AngularVelocity[1], dt_s);
		m_AngularDisplace[2] += Product(m_AngularVelocity[2], dt_s);

	}

	void ComputeTotalTorque()
	{
		m_TorqueEl[0] = - Product(m_AngularDisplace[0], m_Flex);
		m_TorqueEl[1] = - Product(m_AngularDisplace[1], m_Flex);
		m_TorqueEl[2] = - Product(m_AngularDisplace[2], m_Flex);

		m_TorqueTo[0] = m_TorqueEl[0] + m_TorqueEx[0];
		m_TorqueTo[1] = m_TorqueEl[1] + m_TorqueEx[1];
		m_TorqueTo[2] = m_TorqueEl[2] + m_TorqueEx[2];		
	}

	void SetExternalTorque(const RealVec3 exT, Real extent = R_ONE)
	{
		m_TorqueEx[0] = Product(extent, exT[0]);
		m_TorqueEx[1] = Product(extent, exT[1]);
		m_TorqueEx[2] = Product(extent, exT[2]);
	}

private:
	int8		m_ParentID;				// parentID = -1, if no parent

	// kinematic parameters
	RealVec3	m_AngularVelocity;		// in local coodinate
	RealVec3	m_AngularDisplace;		// in local coodinate, angle*axis representation

	// dynamical parameters
	Real		m_Flex;
	Real		m_InverseInertia;		// 1/inertial, avoid divide in later computing
	Real		m_Damp;					// [0, 1]

	RealVec3	m_TorqueEl;				// elastic torque, in local coordinate
	RealVec3	m_TorqueEx;				// external torque
	RealVec3	m_TorqueTo;				// total torque

public:

	RealVec3	m_TranslateInv;		// it is the inverse original transform of bone space to model space
	RealQuat4	m_RotateInv;

	RealVec3	m_TranslateOrg;		// original translation from the parent bone to this bone 
	RealQuat4	m_RotateOrg;		// original rotation from the parent bone to this bone 

	RealVec3	m_TranslateRel;		// temporary translation from the parent bone to this bone during animation
	RealQuat4	m_RotateRel;		// temporary rotation from the parent bone to this bone during animation

	RealVec3	m_TranslateAbs;		// temporary translation from this bone space to model space (root bone space) during animation
	RealQuat4	m_RotateAbs;		// temporary rotation from this bone space to model space (root bone space) during animation

	RealVec3	m_TranslateMS;		// temporary translation in model space, vertex blending translation
	RealQuat4	m_RotateMS;			// temporary rotation in model space, vertex blending rotation

	RealVec3	m_ScaleFactor;		// locally scale factor for each bone space

	RealMat4	m_SkinTM;			// transform skin mesh vertices in model space;

/* a bone is a local coordinate, its tranform matrix
	the vector is column vector, the matrix is column prior

    |x| | m0  m4  m8  m12 | |u|         Z <up>
    |y|=| m1  m5  m9  m13 |*|v|         |   / Y <forward>
    |z| | m2  m6  m10 m14 | |w|         |  /               Global(x,y,z)
    |1| | m3  m7  m11 m15 | |1|         | /
                ||                      |/_______ X <side>
                ||
                ||                
               \||/                     W <up>
                \/                      |    / V <forward>
        | Ux  Vx  Wx  Px |              |   /               Local(u,v,w)
        | Uy  Vy  Wy  Py |              |  /
        | Uz  Vz  Wz  Pz |              | /
        | 0   0   0   1  |              |/________ U <side>

  (Ux, Uy, Uz) is the global coordinate of local U-axis
  (Vx, Vy, Vz) is the global coordinate of local V-axis
  (Wx, Wy, Wz) is the global coordinate of local W-axis
 */
};


/** Model is a physically-based skeletal/skin model
	@remarks
	Model has a skeleton and associated skin meshes. Model not only supports rigid skeleton 
	tranformation as rotation and translation, but also supports uniform scaling for each 
	individual bone (non-uniform scaling will unreasonably distort the skin meshes).
	@par
	The skeleton of the model consists of the physically-based joints: Bone. So the model 
	is dynamically as well as kinemically active.
	@bugs
	I tried to use matrix palette OES extension of gerbera, but it didn't work.
	So I have to blend/tranform the vertices by myself.
	But the code give an option of using the matrix palette extension in case other 
	implementations of OpenGL ES or later version of gerbera support it.
*/

class Model
{
public:

	typedef struct _Material
	{
		RealVec4	ambient, diffuse, specular, emissive;
		Real		shininess;
		Real		transparency;
		uint32		indices;
	} Material;

public:		
	Model();
	virtual ~Model();

	void UpdateSkeleton(TimeValue deltatime=0);	// deltatime: msec
	
protected:	
	void TransformSkinMesh();
	void TransformSkeletonMesh();
	
public:
	virtual void Draw(uint8 num = 1, bool drawskin = true);
	virtual void DrawSkins(uint8 num = 1);
	virtual void DrawSkeleton(uint8 num = 1);

	void SetScaleFactor(const RealVec3 s);
	void SetExternalTorques(const RealVec3 exT, Real extent = R_ONE);

protected:
	void SetupModelSpaceToBoneSpaceTM();

	void GetRootPositionAbs(RealVec3 pos) const
	{
		pos[0] = m_TMModel2World[12];
		pos[1] = m_TMModel2World[13];
		pos[2] = m_TMModel2World[14];
	}

	void SetRootPositionAbs(const RealVec3 pos)
	{
		m_TMModel2World[12] = pos[0];
		m_TMModel2World[13] = pos[1];
		m_TMModel2World[14] = pos[2];
	}

protected:

	RealMat4	m_TMModel2World;	// model space to world space transform

	uint16		m_nNumVertices;		// less than 65535

	// skin mesh vertices
	RealVec3	* m_pVertices;		// skin vertex coordination in model space, will not change during animation
	RealVec3	* m_pNormals;		// skin vertex normal in model space, will not change during animation

	RealVec3	* m_pVerticesTM;	// transformed vertex coordination in model space, change during animation
	RealVec3	* m_pNormalsTM;		// transformed vertex normal in model space, change during animation

	uint8		* m_pBoneID;		// which bone (only one) the vertex associates, less than 0 meams no bones
	Real		* m_pBoneWeight;

	RealVec2	* m_pTexCoords;
	
	// skin mesh indices
	uint16		m_nNumIndices;	// less than 65535
	GLushort	* m_pIndices;

	// materials / meshes
	uint16		m_nNumMaterials;
	Material	* m_pMaterials;

	// skeletal bones/joints
	Bone		* m_pBones;
	uint8		m_nNumBones;			// less than 255

	RealVec3	* m_pSkeletonVertices;	// vertices for draw skeleton	
	GLushort	* m_pSkeletonIndices;	// indices for draw skeleton
};

#endif