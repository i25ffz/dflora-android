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

#ifdef WIN32
#pragma warning( disable : 4514 4100 4505 )
#endif

#ifndef FIXED_REAL
#include <math.h>
#endif

#include "utility.h"

static const GLfixed _fixed_sin_tab[] = {
#include "fixed_sin.h"
};

Real Rand01()
{
#ifdef FIXED_REAL
	return (int)(rand()*rand())%((int)Q_ONE);
#else	// FLOAT_REAL
	return (float)rand()/(float)RAND_MAX;
#endif
}

Real Product( Real x, Real y )
{
#ifdef FIXED_REAL
	return (Real)( (int64)((int64)x*y)>>Q_FACTOR );
#else	// FLOAT_REAL
	return x*y;
#endif
}

Real Inverse( Real x)
{
#ifdef FIXED_REAL
	return (Real)(((int64)1<<(Q_FACTOR<<1)) / x);
#else	// FLOAT_REAL
	return 1.f/x;
#endif
}

Real Quotient( Real numerator, Real denominator)
{
#ifdef FIXED_REAL
//	Real oon = (GLfixed)(((int64)1<<(Q_FACTOR<<1)) / denominator);
//	Product(numerator, oon);

	return (Real)(((int64)numerator*((int64)1<<(Q_FACTOR<<1))/denominator)>>Q_FACTOR);
#else	// FLOAT_REAL
	return numerator/denominator;
#endif
}

Real Interpolate(Real x0, Real x1, Real alpha)
{
	return x0 + Product((x1 - x0), alpha);
}


Real Sqrt( Real val )
{
#ifdef FIXED_REAL
// Note: This fast square root function only works with an even Q_FACTOR
    int32 sqrtVal = 0;
#define step(shift)											\
    if((0x40000000l >> shift) + sqrtVal <= val)				\
    {														\
        val -= (0x40000000l >> shift) + sqrtVal;			\
        sqrtVal = (sqrtVal >> 1) | (0x40000000l >> shift);  \
    }														\
    else													\
    {														\
        sqrtVal = sqrtVal >> 1;                             \
    }    
    step(0);
    step(2);
    step(4);
    step(6);
    step(8);
    step(10);
    step(12);
    step(14);
    step(16);
    step(18);
    step(20);
    step(22);
    step(24);
    step(26);
    step(28);
    step(30);
#undef step
    
    if(sqrtVal < val)
    {
        ++sqrtVal;
    }
    
    sqrtVal <<= (Q_FACTOR)/2;
    return sqrtVal;	
#else	// FLOAT_REAL
	return (GLfloat)sqrt((GLfloat)val);
#endif
}

Real Sin( Real a )
{
#ifdef FIXED_REAL
    GLfixed v;
    /* reduce to [0,1) */
    while (a < Q_ZERO) a += Q_2PI;
    a *= Q_R2PI;
    a >>= Q_FACTOR;

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

	v = (a & 0x400) ? _fixed_sin_tab[0x3ff - (a & 0x3ff)] : _fixed_sin_tab[a & 0x3ff];
    v = Product(v, R_ONE);
    return (a & 0x800) ? -v : v;

#else	// FLOAT_REAL
	return (GLfloat)sin(a);
#endif
}

Real Cos( Real a )
{
#ifdef FIXED_REAL
	GLfixed v;
    /* reduce to [0,1) */
	while (a < Q_ZERO) a += Q_2PI;
	a *= Q_R2PI;
    a >>= Q_FACTOR;
    a += 0x4000;

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

	v = (a & 0x400) ? _fixed_sin_tab[0x3ff - (a & 0x3ff)] : _fixed_sin_tab[a & 0x3ff];
	v = Product(v, R_ONE);
	return (a & 0x800) ? -v : v;
#else	// FLOAT_REAL
	return (GLfloat)cos(a);
#endif
}

Real DotProductVec3( const RealVec3 v1, const RealVec3 v2 )
{
#ifdef FIXED_REAL
	return (GLfixed)(((int64)((int64)v1[0]*v2[0]) + ((int64)v1[1]*v2[1]) + ((int64)v1[2]*v2[2]))>>Q_FACTOR);
#else	// FLOAT_REAL
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
#endif
}

void CrossProductVec3( RealVec3 n, const RealVec3 a, const RealVec3 b )
{

#ifdef FIXED_REAL
	n[0] = (GLfixed)((((int64)a[1]*b[2]) - ((int64)a[2]*b[1]))>>Q_FACTOR); 
	n[1] = (GLfixed)((((int64)a[2]*b[0]) - ((int64)a[0]*b[2]))>>Q_FACTOR); 
	n[2] = (GLfixed)((((int64)a[0]*b[1]) - ((int64)a[1]*b[0]))>>Q_FACTOR);
#else	// FLOAT_REAL
	n[0] = a[1]*b[2] - a[2]*b[1];
	n[1] = a[2]*b[0] - a[0]*b[2];
	n[2] = a[0]*b[1] - a[1]*b[0];
#endif
}

Real LengthVec3( const RealVec3 a )
{
	if(a[0] == R_ZERO && a[1] == R_ZERO && a[2] == R_ZERO)
		return R_ZERO;
	return Sqrt(DotProductVec3(a, a));
}

Real NormalizeVec3( RealVec3 a)
{
	Real n, oon;
	n = DotProductVec3(a, a);
	n = Sqrt(n);
	if (n>R_ZERO)
	{
		oon = Inverse(n);
		a[0] = Product(a[0], oon);	
		a[1] = Product(a[1], oon);
		a[2] = Product(a[2], oon);
		return n;
	}
	else
	{
		return R_ZERO;
	}
}

Real UnifyVec3( const RealVec3 in, RealVec3 out )
{
	memcpy(out, in, sizeof(RealVec3));
	return NormalizeVec3(out);
}

void ScaleVec3( RealVec3 vec, Real s)
{
	vec[0] = Product(vec[0], s);
	vec[1] = Product(vec[1], s);
	vec[2] = Product(vec[2], s);
}

void ScaleVec3ByVec3( RealVec3 vec, RealVec3 s )
{
	vec[0] = Product(vec[0], s[0]);
	vec[1] = Product(vec[1], s[1]);
	vec[2] = Product(vec[2], s[2]);
}

void TranslateVec3ByVec3( RealVec3 vec, RealVec3 translate )
{
	vec[0] += translate[0];
	vec[1] += translate[1];
	vec[2] += translate[2];
}

void ClipVec3(RealVec3 vec, Real minimun, Real maximun)
{
//	assert(minimun<=maximun);
	Real vec2 = DotProductVec3(vec, vec);
	Real lm2, lM2;
	lm2 = Product(minimun, minimun);
	if(vec2 < lm2)
	{
		NormalizeVec3(vec);
		vec[0] = Product(vec[0], minimun);
		vec[1] = Product(vec[1], minimun);
		vec[2] = Product(vec[2], minimun);
		return;
	}

	lM2 = Product(maximun, maximun);
	if(vec2 > lM2)
	{
		NormalizeVec3(vec);
		vec[0] = Product(vec[0], maximun);
		vec[1] = Product(vec[1], maximun);
		vec[2] = Product(vec[2], maximun);
		return;
	}
	return;
}

void LoadIdentityQuat4( RealQuat4 q )
{
	q[0] = R_ONE; q[1] = q[2] = q[3] = R_ZERO;
}

void InverseQuat4( RealQuat4 q )
{
	q[1] -= q[1];
	q[2] -= q[2];
	q[3] -= q[3];	
}

void PostMultiplyQuat4( const RealQuat4 qin1, const RealQuat4 qin2, RealQuat4 qout )
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.
	
	qout[0] = Product(qin1[0], qin2[0]) - Product(qin1[1], qin2[1]) 
			- Product(qin1[2], qin2[2]) - Product(qin1[3], qin2[3]);
	qout[1] = Product(qin1[0], qin2[1]) + Product(qin1[1], qin2[0]) 
			+ Product(qin1[2], qin2[3]) - Product(qin1[3], qin2[2]);
	qout[2] = Product(qin1[0], qin2[2]) + Product(qin1[2], qin2[0])
			+ Product(qin1[3], qin2[1]) - Product(qin1[1], qin2[3]);
	qout[3] = Product(qin1[0], qin2[3]) + Product(qin1[3], qin2[0])
			+ Product(qin1[1], qin2[2]) - Product(qin1[2], qin2[1]);

}

void SetQuat4FromAngleAxis( RealQuat4 q, Real angle, const RealVec3 axis )
{
	// assert:  axis is unit length
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
	
	Real HalfAngle = Product( FTOR(0.5), angle );
	Real fSin = Sin(HalfAngle);
	q[0] = Cos(HalfAngle);
	q[1] = Product(fSin, axis[0]);
	q[2] = Product(fSin, axis[1]);
	q[3] = Product(fSin, axis[2]);

}

void SetQuat4FromAngleDisp( RealQuat4 q, const RealVec3 disp )
{
	RealVec3 axis;
	memcpy(axis, disp, sizeof(RealVec3));
	Real angle = NormalizeVec3(axis);
	if( angle>R_ZERO )
	{
		SetQuat4FromAngleAxis(q, angle, axis);
	}
	else
	{
		LoadIdentityQuat4(q);
	}
}

void SetQuat4FromRotationMat4( RealQuat4 q, const RealMat4 m )
{
	// Algorithm adpated from Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".
	// This function seems not robust on some occasions especially when real represented
	// fixed point with lower precision !
	
	Real trace = m[0]+m[5]+m[10];
	Real root;
	
	{
		// |w| > 1/2, may as well choose w > 1/2
		root = Sqrt(trace + R_ONE);		// 2w
		if(root != R_ZERO)
		{
			q[0] = Product(FTOR(0.5), root);
			q[1] = Quotient(Product(m[2+1*4]-m[1+2*4], FTOR(0.5)), root);
			q[2] = Quotient(Product(m[0+2*4]-m[2+0*4], FTOR(0.5)), root);
			q[3] = Quotient(Product(m[1+0*4]-m[0+1*4], FTOR(0.5)), root);
		}
		else
		{
			q[0] = R_ZERO;
			q[1] = R_ZERO;
			q[2] = R_ZERO;
			q[3] = R_ONE;
		}
	}
}

void RotateVec3ByQuat4( const RealVec3 vin, const RealQuat4 q, RealVec3 vout )
{
	// nVidia SDK implementation
	RealVec3 uv, uuv, qvec; 
	qvec[0] = q[1];
	qvec[1] = q[2];
	qvec[2] = q[3];

	CrossProductVec3(uv, qvec, vin);
	CrossProductVec3(uuv, qvec, uv); 
	ScaleVec3(uv, Product(FTOR(2.0f), q[0]));
	ScaleVec3(uuv, FTOR(2.0f));
	
	vout[0] =  vin[0] + uv[0] + uuv[0];
	vout[1] =  vin[1] + uv[1] + uuv[1];
	vout[2] =  vin[2] + uv[2] + uuv[2];
}

void SetRotationMat4FromQuat4( RealMat4 m, const RealQuat4 q )
{
	Real Tx  = Product(FTOR(2.0), q[1]);
	Real Ty  = Product(FTOR(2.0), q[2]);
	Real Tz  = Product(FTOR(2.0), q[3]);
	Real Twx = Product(Tx, q[0]);
	Real Twy = Product(Ty, q[0]);
	Real Twz = Product(Tz, q[0]);
	Real Txx = Product(Tx, q[1]);
	Real Txy = Product(Ty, q[1]);
	Real Txz = Product(Tz, q[1]);
	Real Tyy = Product(Ty, q[2]);
	Real Tyz = Product(Tz, q[2]);
	Real Tzz = Product(Tz, q[3]);
	
	m[0] = R_ONE-(Tyy+Tzz);
	m[4] = Txy - Twz;
	m[8] = Txz + Twy;
	m[1] = Txy + Twz;
	m[5] = R_ONE-(Txx+Tzz);
	m[9] = Tyz - Twx;
	m[2] = Txz - Twy;
	m[6] = Tyz + Twx;
	m[10] = R_ONE-(Txx+Tyy);	
}

void LoadIdentityMat4( RealMat4 m )
{
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = 
	m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0;
	m[0] = m[5] = m[10] = m[15] = ITOR(1);
}

void CopyMat4( RealMat4 from, RealMat4 to )
{
	memcpy(to, from, sizeof(RealMat4));
}


void PostMultiplyMat4( RealMat4 m, const RealMat4 f )
{
	RealMat4 ret = 
	{
		R_ZERO, R_ZERO, R_ZERO, R_ZERO,
		R_ZERO, R_ZERO, R_ZERO, R_ZERO,
		R_ZERO, R_ZERO, R_ZERO, R_ZERO,
		R_ZERO, R_ZERO, R_ZERO, R_ONE,
	};

	int i, j, i4;
	for(i=0; i<4; i++)
	{
		for(j=0; j<3; j++)
		{
			i4 = i<<2;	//i*4;
#ifdef FIXED_REAL
			*(ret+i4+j) = (GLfixed)( ((int64)((int64)m[0+j]*f[i4+0]) + ((int64)m[4+j]*f[i4+1]) + 
						((int64)m[8+j]*f[i4+2]) + ((int64)m[12+j]*f[i4+3]) )>>Q_FACTOR );
#else	// FLOAT_REAL
			*(ret+i4+j) = m[0+j]*f[i4+0] + m[4+j]*f[i4+1] + m[8+j]*f[i4+2] + m[12+j]*f[i4+3];
#endif
		}
	}
	CopyMat4(ret, m);
}

void ScaleMat4ByVec3(RealMat4 m, const RealVec3 s)
{
	m[0] = Product(m[0], s[0]); m[4] = Product(m[4], s[1]); m[ 8] = Product(m[ 8], s[2]);
	m[1] = Product(m[1], s[0]); m[5] = Product(m[5], s[1]); m[ 9] = Product(m[ 9], s[2]);
	m[2] = Product(m[2], s[0]); m[6] = Product(m[6], s[1]); m[10] = Product(m[10], s[2]);
}

void SetRotationMat4FromAngleAxisVec3( RealMat4 m, Real angle, const RealVec3 axis )
{
	// do not affect tranlation value of matrix m
	
	Real x, y, z, x1, y1, z1, x2, y2, z2;
	Real x1_y1, x1_z1, y1_z1;
	Real ca_x1_y1, ca_x1_z1, ca_y1_z1;
	Real sa_x1, sa_y1, sa_z1;
	RealVec3 out;

	Real ca = Cos(angle);
	Real sa = Sin(angle);

	x = axis[0], y = axis[1],	z = axis[2];

//	if(x==R_ZERO && y==R_ZERO && z==R_ZERO)	// do nothing
//		return;

	if (x==R_ONE && y==R_ZERO && z==R_ZERO)	// ABOUT X-AXIS
	{  
		m[0]=R_ONE;		m[4]=R_ZERO;	m[8]=R_ZERO;	// m[12]=R_ZERO;
		m[1]=R_ZERO;	m[5]=ca;		m[9]=-sa;		// m[13]=R_ZERO;
		m[2]=R_ZERO;	m[6]=sa;		m[10]=ca;		// m[14]=R_ZERO;
		m[3]=R_ZERO;	m[7]=R_ZERO;	m[11]=R_ZERO;	m[15]=R_ONE;
		return;
	}
	if (x==R_ZERO && y==R_ONE && z==R_ZERO)  // ABOUT Y-AXIS
	{
		m[0]=ca;		m[4]=R_ZERO;	m[8]=sa;		// m[12]=R_ZERO;
		m[1]=R_ZERO;	m[5]=R_ONE;		m[9]=R_ZERO;	// m[13]=R_ZERO;
		m[2]=-sa;		m[6]=R_ZERO;	m[10]=ca;		// m[14]=R_ZERO;
		m[3]=R_ZERO;	m[7]=R_ZERO;	m[11]=R_ZERO;	m[15]=R_ONE;
		return;
	}
	if (x==R_ZERO && y==R_ZERO && z==R_ONE)  // ABOUT Z-AXIS
	{
		m[0]=ca;		m[4]=-sa;		m[8]=R_ZERO;	// m[12]=0;
		m[1]=sa;		m[5]=ca;		m[9]=R_ZERO;	// m[13]=0;
		m[2]=R_ZERO;	m[6]=R_ZERO;	m[10]=R_ONE;	// m[14]=0;
		m[3]=R_ZERO;	m[7]=R_ZERO;	m[11]=R_ZERO;	m[15]=R_ONE;
		return;
	}

   // ARBITRARY AXIS
	{
		UnifyVec3(axis, out);	// normalize
		x1 = out[0];
		y1 = out[1];
		z1 = out[2];
		
		x2 = Product(x1, x1);
		y2 = Product(y1, y1);
		z2 = Product(z1, z1);

		x1_y1 = Product(x1, y1);
		x1_z1 = Product(x1, z1);
		y1_z1 = Product(y1, z1);

		ca_x1_y1 = Product(ca, x1_y1);
		ca_x1_z1 = Product(ca, x1_z1);
		ca_y1_z1 = Product(ca, y1_z1);

		sa_x1 = Product(sa, x1);
		sa_y1 = Product(sa, y1);
		sa_z1 = Product(sa, z1);

		m[0] = x2 + Product(ca, (R_ONE-x2));		
		m[1] = x1_y1 - ca_x1_y1 + sa_z1;		
		m[2] = x1_z1 - ca_x1_z1 - sa_y1;
		
		m[4] = x1_y1 - ca_x1_y1 - sa_z1;
		m[5] = y2 + Product(ca, (R_ONE-y2));
		m[6] = y1_z1 - ca_y1_z1 + sa_x1;
			
		m[8] = x1_z1 - ca_x1_z1 + sa_y1;		
		m[9] = y1_z1 - ca_y1_z1 - sa_x1;		
		m[10] = z2 + Product(ca, (R_ONE-z2));

		m[3] = m[7] = m[11] = R_ZERO;		
		m[15] = R_ONE;
	}
}

void SetRotationMat4FromDispVec3( RealMat4 m, const RealVec3 disp )
{
	Real a = LengthVec3(disp);
	if(a > R_ZERO)
		SetRotationMat4FromAngleAxisVec3(m, a, disp);
}

void LookAtMatrix(RealMat4 m, Real ex, Real ey, Real ez, Real atx, Real aty, Real atz, Real ux, Real uy, Real uz)
{
	RealVec3 vEye;
	RealVec3 vDir;
	RealVec3 vUp;
	RealVec3 vRight;

	//get viewing vector
	vDir[0] = atx - ex;
	vDir[1] = aty - ey;
	vDir[2] = atz - ez;
	UnifyVec3(vDir, vDir);

	//get right vector
	vUp[0] = ux;
	vUp[1] = uy;
	vUp[2] = uz;
	CrossProductVec3(vRight,vDir,vUp);
	UnifyVec3(vRight, vRight);

	//get up vector
	CrossProductVec3(vUp,vRight,vDir);
	UnifyVec3(vUp, vUp);

	//other 3 components
	vEye[0] = ex;
	vEye[1] = ey;
	vEye[2] = ez;
	
	*(m+0)  = vRight[0]; 
    *(m+4)  = vRight[1];
	*(m+8)  = vRight[2];
	*(m+12) = -DotProductVec3(vRight,vEye);
	*(m+1)  = vUp[0]; 
	*(m+5)  = vUp[1];
	*(m+9)  = vUp[2];
	*(m+13) = -DotProductVec3(vUp,vEye);
	*(m+2)  = -vDir[0]; 
	*(m+6)  = -vDir[1];
	*(m+10) = -vDir[2];
	*(m+14) = DotProductVec3(vDir,vEye);
	*(m+3)  = R_ZERO; 
	*(m+7)  = R_ZERO;
	*(m+11) = R_ZERO;
	*(m+15) = R_ONE;
}

void TransformVec3ByMat4( const RealVec3 in, const RealMat4 m, RealVec3 out )
{
	Real w;
#ifdef FIXED_REAL
	out[0] = (int32)(((int64)((int64)in[0]*m[0]) + ((int64)in[1]*m[4]) + ((int64)in[2]*m[ 8]))>>Q_FACTOR) + m[12];
	out[1] = (int32)(((int64)((int64)in[0]*m[1]) + ((int64)in[1]*m[5]) + ((int64)in[2]*m[ 9]))>>Q_FACTOR) + m[13];
	out[2] = (int32)(((int64)((int64)in[0]*m[2]) + ((int64)in[1]*m[6]) + ((int64)in[2]*m[10]))>>Q_FACTOR) + m[14];
	w	   = (int32)(((int64)((int64)in[0]*m[3]) + ((int64)in[1]*m[7]) + ((int64)in[2]*m[11]))>>Q_FACTOR) + m[15];
	if(w != R_ONE)
		ScaleVec3(out, Inverse(w));

#else
	out[0] = in[0]*m[0] + in[1]*m[4] + in[2]*m[ 8] + m[12];
	out[1] = in[0]*m[1] + in[1]*m[5] + in[2]*m[ 9] + m[13];
	out[2] = in[0]*m[2] + in[1]*m[6] + in[2]*m[10] + m[14];
	w      = in[0]*m[3] + in[1]*m[7] + in[2]*m[11] + m[15];
	if(w != R_ONE)
		ScaleVec3(out, Inverse(w));
#endif
}

void RotateVec3ByMat4( const RealVec3 in, const RealMat4 m, RealVec3 out )
{
#ifdef FIXED_REAL
	out[0] = (int32)(((int64)((int64)in[0]*m[0]) + ((int64)in[1]*m[4]) + ((int64)in[2]*m[ 8]))>>Q_FACTOR);
	out[1] = (int32)(((int64)((int64)in[0]*m[1]) + ((int64)in[1]*m[5]) + ((int64)in[2]*m[ 9]))>>Q_FACTOR);
	out[2] = (int32)(((int64)((int64)in[0]*m[2]) + ((int64)in[1]*m[6]) + ((int64)in[2]*m[10]))>>Q_FACTOR);
#else	// FLOAT_REAL
	out[0] = in[0]*m[0] + in[1]*m[4] + in[2]*m[ 8];
	out[1] = in[0]*m[1] + in[1]*m[5] + in[2]*m[ 9];
	out[2] = in[0]*m[2] + in[1]*m[6] + in[2]*m[10];
#endif
}

bool ProjectVec3ToViewport( const RealVec3 vin, const RealMat4 viewMat, 
						   const RealMat4 projMat, const GLint viewport[4], 
						   RealVec3 vout )
{

	RealVec3 vv, vp;
	TransformVec3ByMat4(vin, viewMat, vv);
	TransformVec3ByMat4(vv, projMat, vp);
	if( -R_ONE<vp[0] && vp[0]<R_ONE && -R_ONE<vp[1] && vp[1]<R_ONE && -R_ONE<vp[2] && vp[2]<R_ONE )
	{
		Real x0 = ITOR(viewport[0]);
		Real y0 = ITOR(viewport[1]);
		Real halfwidth = Product(ITOR(viewport[2]), FTOR(0.5));
		Real halfheight = Product(ITOR(viewport[3]), FTOR(0.5));

		vout[0] = x0 + Product(halfwidth, vp[0]+R_ONE);
		vout[1] = y0 + Product(halfheight, vp[1]+R_ONE);
		vout[2] = Product(vp[2]+R_ONE, FTOR(0.5));
	
		return true;
	}
	else
	{
		return false;
	}
}

GLint Begin2DDrawing (void)
{
	// store OpenGL matrix mode
	GLint originalMatrixMode;
	glGetIntegerv (GL_MATRIX_MODE, &originalMatrixMode);
	glDisable(GL_DEPTH_TEST);

	// clear projection transform
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// set up orthogonal projection onto window's screen space
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glOrthor(ITOR(viewport[0]), ITOR(viewport[2]), ITOR(viewport[1]), ITOR(viewport[3]), -R_ONE, R_ONE);
	// clear model transform
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// return original matrix mode for saving (stacking)
	return originalMatrixMode;
}


void End2DDrawing (GLint originalMatrixMode)
{
	// restore previous model/projection transformation state
	glPopMatrix ();
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	// restore OpenGL matrix mode
	glMatrixMode (originalMatrixMode);
	glEnable(GL_DEPTH_TEST);
}