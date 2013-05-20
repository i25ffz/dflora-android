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


#include <stdlib.h>
#include <stdio.h>
#include "model.h"

static const Real sqrt3_2 = FTOR(0.86602540378f);


void Bone::Create()
{
	m_ParentID = -1;
	
	m_ScaleFactor[0] = R_ONE;
	m_ScaleFactor[1] = R_ONE;
	m_ScaleFactor[2] = R_ONE;
	
	m_Flex				= FTOR(1);
	m_InverseInertia	= FTOR(30);
	m_Damp				= R_ZERO;	//FTOR(0.0515);
	
	m_AngularVelocity[0] = m_AngularVelocity[1] = m_AngularVelocity[2] = ITOR(0);
	m_AngularDisplace[0] = m_AngularDisplace[1] = m_AngularDisplace[2] = ITOR(0);
	
	m_TorqueEl[0] = m_TorqueEl[1] = m_TorqueEl[2] = ITOR(0);
	m_TorqueEx[0] = m_TorqueEx[1] = m_TorqueEx[2] = ITOR(0);
	m_TorqueTo[0] = m_TorqueTo[1] = m_TorqueTo[2] = ITOR(0);
	
	m_TranslateOrg[0] = m_TranslateOrg[1] = m_TranslateOrg[2] = R_ZERO;
	LoadIdentityQuat4(m_RotateOrg);
	
	m_TranslateRel[0] = m_TranslateRel[1] = m_TranslateRel[2] = R_ZERO;
	LoadIdentityQuat4(m_RotateRel);
	
	m_TranslateAbs[0] = m_TranslateAbs[1] = m_TranslateAbs[2] = R_ZERO;
	LoadIdentityQuat4(m_RotateAbs);
	
	LoadIdentityQuat4(m_RotateMS);
	LoadIdentityMat4(m_SkinTM);
}

Model::Model()
{
	m_nNumVertices	= 0;
	m_nNumIndices	= 0;
	m_nNumBones		= 0;
	m_pBoneID		= NULL;
	m_pBoneWeight	= NULL;

	m_pIndices		= NULL;
	m_pBones		= NULL;
	

	m_pVertices		= NULL;
	m_pNormals		= NULL;
	m_pVerticesTM	= NULL;
	m_pNormalsTM	= NULL;

	
	m_pTexCoords	= NULL;

	m_nNumMaterials = 0;
	m_pMaterials	= NULL;

	m_pSkeletonVertices	= NULL;
	m_pSkeletonIndices	= NULL;
	LoadIdentityMat4(m_TMModel2World);
}


Model::~Model()
{
	SafeDeleteArray(m_pVertices);
	SafeDeleteArray(m_pNormals);
	SafeDeleteArray(m_pVerticesTM);
	SafeDeleteArray(m_pNormalsTM);

	SafeDeleteArray(m_pBoneID);
	SafeDeleteArray(m_pBoneWeight);
	SafeDeleteArray(m_pTexCoords);

	SafeDeleteArray(m_pIndices);

	SafeDeleteArray(m_pBones);
	SafeDeleteArray(m_pSkeletonVertices);
	SafeDeleteArray(m_pSkeletonIndices);

	SafeDeleteArray(m_pMaterials);
}

void Model::SetScaleFactor(const RealVec3 s)
{
	for(uint8 i=1; i<m_nNumBones; i++)
	{
		m_pBones[i].SetScaleFactor(s);
	}
}

void Model::SetExternalTorques(const RealVec3 exT, Real extent)
{	
	for(uint8 i=0; i<m_nNumBones; i++)
	{
		m_pBones[i].SetExternalTorque(exT, extent);
	}
}

void Model::UpdateSkeleton(TimeValue deltatime)	// deltatime: msec
{
	uint8 i;
	for(i=0; i<m_nNumBones; i++)
	{
		Bone& bone = m_pBones[i];

		bone.ComputeTotalTorque();

		// update the relative angle m_Velocityocity and displace
		bone.UpdateAngularMotion(deltatime);
		bone.UpdateTransformRel();
		
		int parent = bone.GetParent();
		// update the absolute transform matrix
		if(parent != -1)	// not the root joint
		{
			bone.m_TranslateRel[0] = Product(bone.m_TranslateRel[0], bone.m_ScaleFactor[0]);
			bone.m_TranslateRel[1] = Product(bone.m_TranslateRel[1], bone.m_ScaleFactor[1]);
			bone.m_TranslateRel[2] = Product(bone.m_TranslateRel[2], bone.m_ScaleFactor[2]);

			RotateVec3ByQuat4(bone.m_TranslateRel, m_pBones[parent].m_RotateAbs, bone.m_TranslateAbs);

			bone.m_TranslateAbs[0] += m_pBones[parent].m_TranslateAbs[0];
			bone.m_TranslateAbs[1] += m_pBones[parent].m_TranslateAbs[1];
			bone.m_TranslateAbs[2] += m_pBones[parent].m_TranslateAbs[2];

			PostMultiplyQuat4(m_pBones[parent].m_RotateAbs, bone.m_RotateRel, bone.m_RotateAbs);
		}
		else
		{
			// add global transformation to root
			bone.m_TranslateRel[0] = Product(bone.m_TranslateRel[0], bone.m_ScaleFactor[0]);
			bone.m_TranslateRel[1] = Product(bone.m_TranslateRel[1], bone.m_ScaleFactor[1]);
			bone.m_TranslateRel[2] = Product(bone.m_TranslateRel[2], bone.m_ScaleFactor[2]);
			
			bone.m_TranslateAbs[0] = bone.m_TranslateRel[0] + m_TMModel2World[12];
			bone.m_TranslateAbs[1] = bone.m_TranslateRel[1] + m_TMModel2World[13];
			bone.m_TranslateAbs[2] = bone.m_TranslateRel[2] + m_TMModel2World[14];

			RealQuat4 rotRoot;
			SetQuat4FromRotationMat4(rotRoot, m_TMModel2World);
			PostMultiplyQuat4(rotRoot, bone.m_RotateRel, bone.m_RotateAbs);
		}
	}

//    glMatrixMode(GL_MATRIX_PALETTE_OES);
	for(i=0; i<m_nNumBones; i++)
	{
		Bone & bone = m_pBones[i];
		RealMat4 & mat = bone.m_SkinTM;

		// Scale -> Rotate -> Translate: vertices from bone space to model space
		// SkinTM = (T4x4)*(R4x4)*(S4x4)*(V4x1)
		// = [RaSa Ta]*|Vbonespace|
		//   [0     1] |          |
		// = [RaSa Ta]*[Ri 0]*[0 Ti]*|Vmodelspace|
		//   [0     1] [0  1] [0  1] |           |
		// = [RaSaRi RaSaRiTi+Ta]*|Vmodelspace|
		//   [0      1          ] |           |
		// Ra Ta Sa - absolute rotate, translate, scale from bone space to model space
		// Ri Ti - inverse rotate, translate

		PostMultiplyQuat4(bone.m_RotateAbs, bone.m_RotateInv, bone.m_RotateMS);

		//	update m_SkinTM via homogeneous matrix multiplication
		RealMat4 invmatT, invmatR;
		LoadIdentityMat4(mat);
		SetRotationMat4FromQuat4(mat, bone.m_RotateAbs);
		ScaleMat4ByVec3(mat, bone.m_ScaleFactor);
		// tranlate is the sum of root bone and model
		mat[12] = bone.m_TranslateAbs[0];
		mat[13] = bone.m_TranslateAbs[1];
		mat[14] = bone.m_TranslateAbs[2];

		LoadIdentityMat4(invmatT);
		invmatT[12] = bone.m_TranslateInv[0];
		invmatT[13] = bone.m_TranslateInv[1];
		invmatT[14] = bone.m_TranslateInv[2];

		LoadIdentityMat4(invmatR);
		SetRotationMat4FromQuat4(invmatR, bone.m_RotateInv);

		PostMultiplyMat4(mat, invmatR);
		PostMultiplyMat4(mat, invmatT);

		// matrix plalette seems not work, comment out!
/*		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
        glLoadMatrixr(bone.m_SkinTM);
        glCurrentPaletteMatrixOES(i);
		glLoadPaletteFromModelViewMatrixOES();
		glPopMatrix();
*/
	}
}

void Model::Draw(uint8 num, bool drawskin)
{
	if(drawskin)
	{
		TransformSkinMesh();
		DrawSkins(num);
	}
	else
	{
		TransformSkeletonMesh();
		glDisable(GL_LIGHTING);
		DrawSkeleton(num);
		glEnable(GL_LIGHTING);
	}
}

void Model::DrawSkeleton(uint8 num)
{
	Real offset = -Product(FTOR(num-1), FTOR(0.5));
	Real distance = FTOR(18);
	offset = Product(offset, distance);
	Real trans = offset;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_REAL, 0, m_pSkeletonVertices);

	GLsizei numBones2 = m_nNumBones*2;
	for(uint8 j=0; j<num; j++)
	{
		glPushMatrix();
		glTranslater(trans, R_ZERO, R_ZERO);
		glColor4r(FTOR(0), FTOR(1.0), FTOR(0), FTOR(0));

		glDrawElements(GL_LINES, numBones2, GL_UNSIGNED_SHORT, m_pSkeletonIndices);

		glDisable(GL_DEPTH_TEST);
		glColor4r(FTOR(1), FTOR(0), FTOR(0), FTOR(0));
		glDrawArrays(GL_POINTS, 0, m_nNumBones);
		glEnable(GL_DEPTH_TEST);

		glPopMatrix();
		trans += distance;
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Model::DrawSkins(uint8 num)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// matrix plalette seems not work, comment out!
//	glEnableClientState(GL_WEIGHT_ARRAY_OES);
//	glEnableClientState(GL_MATRIX_INDEX_ARRAY_OES);

	glVertexPointer(3, GL_REAL, 0, m_pVerticesTM);
	glNormalPointer(GL_REAL, 0, m_pNormalsTM);

	// matrix plalette seems not work, comment out!
//	glVertexPointer(3, GL_REAL, 0, m_pVertices);
//	glNormalPointer(GL_REAL, 0, m_pNormals);
//	glWeightPointerOES(1, GL_REAL, 0, m_pBoneWeight);
//	glMatrixIndexPointerOES(1, GL_UNSIGNED_BYTE, 0, m_pBoneID);

	Real offset = -Product(FTOR(num-1), FTOR(0.5));
	Real distance = FTOR(18);
	offset = Product(offset, distance);
	Real trans = offset;

	for(uint16 i=0; i<m_nNumMaterials; i++)
	{
		glMaterialrv( GL_FRONT_AND_BACK, GL_AMBIENT, m_pMaterials[i].ambient );
		glMaterialrv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_pMaterials[i].diffuse );
		glMaterialrv( GL_FRONT_AND_BACK, GL_SPECULAR, m_pMaterials[i].specular );
		glMaterialrv( GL_FRONT_AND_BACK, GL_EMISSION, m_pMaterials[i].emissive );
		glMaterialr( GL_FRONT_AND_BACK, GL_SHININESS, m_pMaterials[i].shininess );
		uint32 numIndices = 0;
		uint32 startIndex = 0;

		if(i>0)
		{
			startIndex = m_pMaterials[i-1].indices;
			numIndices = m_pMaterials[i].indices - m_pMaterials[i-1].indices;
		}
		else
		{
			startIndex = 0;
			numIndices = m_pMaterials[i].indices;
		}

		trans = offset;
		for(uint8 j=0; j<num; j++)
		{
			glPushMatrix();

			glTranslater(trans, R_ZERO, R_ZERO);
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, m_pIndices+startIndex);

			glPopMatrix();
			trans += distance;
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	// matrix plalette seems not work, comment out!
//	glDisableClientState(GL_WEIGHT_ARRAY_OES);
//	glDisableClientState(GL_MATRIX_INDEX_ARRAY_OES);

}

void Model::SetupModelSpaceToBoneSpaceTM()
{
	uint8 i;
	for(i=0; i<m_nNumBones; i++)
	{
		Bone& bone = m_pBones[i];
		int parent = bone.GetParent();

		if(parent != -1)	// not the root joint
		{
			RotateVec3ByQuat4(bone.m_TranslateOrg, m_pBones[parent].m_RotateAbs, bone.m_TranslateAbs);

			bone.m_TranslateAbs[0] += m_pBones[parent].m_TranslateAbs[0];
			bone.m_TranslateAbs[1] += m_pBones[parent].m_TranslateAbs[1];
			bone.m_TranslateAbs[2] += m_pBones[parent].m_TranslateAbs[2];

			PostMultiplyQuat4(m_pBones[parent].m_RotateAbs, bone.m_RotateOrg, bone.m_RotateAbs);

			bone.m_TranslateInv[0] = -bone.m_TranslateAbs[0];
			bone.m_TranslateInv[1] = -bone.m_TranslateAbs[1];
			bone.m_TranslateInv[2] = -bone.m_TranslateAbs[2];

			bone.m_RotateInv[0] = bone.m_RotateAbs[0];
			bone.m_RotateInv[1] = -bone.m_RotateAbs[1];
			bone.m_RotateInv[2] = -bone.m_RotateAbs[2];
			bone.m_RotateInv[3] = -bone.m_RotateAbs[3];			
		}
		else
		{
			// inverse translation
			bone.m_TranslateInv[0] = -bone.m_TranslateOrg[0];
			bone.m_TranslateInv[1] = -bone.m_TranslateOrg[1];
			bone.m_TranslateInv[2] = -bone.m_TranslateOrg[2];

			// inverse rotation
			bone.m_RotateInv[0] = bone.m_RotateOrg[0];
			bone.m_RotateInv[1] = -bone.m_RotateOrg[1];
			bone.m_RotateInv[2] = -bone.m_RotateOrg[2];
			bone.m_RotateInv[3] = -bone.m_RotateOrg[3];			
		}
	}
}

void Model::TransformSkinMesh()
{
	int16 i = 0;
	RealVec3 * gvert = m_pVertices;
	RealVec3 * gtvert = m_pVerticesTM;
	RealVec3 * gnorm = m_pNormals;
	RealVec3 * gtnorm = m_pNormalsTM;

	for(i=0; i<m_nNumVertices; i++, gtvert++, gvert++, gtnorm++, gnorm++)
	{
		uint8 boneID = m_pBoneID[i];
		Bone & bone = m_pBones[boneID];

		TransformVec3ByMat4(*gvert, bone.m_SkinTM, *gtvert);
		RotateVec3ByQuat4(*gnorm, bone.m_RotateMS, *gtnorm);
	}
}

void Model::TransformSkeletonMesh()
{
	// if draw skeleton, update the skeleton/bones
	RealVec3 * slider = m_pSkeletonVertices;
	if(slider != NULL)
	{
		for(uint8 i=0; i<m_nNumBones; i++)
		{
			m_pBones[i].GetPositionAbs(*slider);
			slider++;
		}
	}
}

