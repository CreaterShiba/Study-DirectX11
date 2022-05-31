
#include "matrix.h"
#if 0
using namespace DirectX;

/*==========================================================================*/
/*==========================================================================*/
/*!
@file		matrix.cpp
@brief		行列
@author		shiba
@date		2013/2/15 create this file.
@note		DirectXMathのラッパー
*/
/*==========================================================================*/
/*==========================================================================*/
// 演算子
NSMatrix NSMatrix::operator+ (NSMatrix& v)
{
	NSMatrix cMat;
	cMat.m_Mat = m_Mat + v.m_Mat;
	return cMat;
}
NSMatrix NSMatrix::operator- (NSMatrix& v)
{
	NSMatrix cMat;
	cMat.m_Mat = m_Mat - v.m_Mat;
	return cMat;
}
NSMatrix NSMatrix::operator* (NSMatrix& v)
{
	NSMatrix cMat;
	cMat.m_Mat = m_Mat * v.m_Mat;
	return cMat;
}
NSMatrix NSMatrix::operator* (f32 mul)
{
	NSMatrix cMat;
	cMat.m_Mat = m_Mat * mul;
	return cMat;
}
NSMatrix NSMatrix::operator/ (f32 div)
{
#if defined( _DEBUG )
	NS_ASSERT(div != 0.0f);
#endif
	NSMatrix cMat;
	cMat.m_Mat = m_Mat / div;
	return cMat;
}


// 代入演算子
NSMatrix& NSMatrix::operator= (const NSMatrix& v)
{
	if (this == &v) return *this;
	m_Mat = v.m_Mat;
	return *this;
}
NSMatrix& NSMatrix::operator+= (const NSMatrix& v)
{
	m_Mat += v.m_Mat;
	return *this;
}
NSMatrix& NSMatrix::operator-= (const NSMatrix& v)
{
	m_Mat -= v.m_Mat;
	return *this;
}
NSMatrix& NSMatrix::operator*= (f32 mul)
{
	m_Mat *= mul;
	return *this;
}
NSMatrix& NSMatrix::operator*= (const NSMatrix& v)
{
	m_Mat *= v.m_Mat;
	return *this;
}
NSMatrix& NSMatrix::operator/= (f32 div)
{
#if defined( _DEBUG )
	NS_ASSERT(div != 0.0f);
#endif
	m_Mat /= div;
	return *this;
}


//// 比較演算子	
//bool NSMatrix::operator== (const NSMatrix& v)const
//{
//}
//bool NSMatrix::operator!= (const NSMatrix& v)const
//{
//}

void NSMatrix::Identity()
{
	m_Mat = XMMatrixIdentity();
}

void NSMatrix::MatrixLookAtLH(const NSVector4& vEye, const NSVector4& vAt, const NSVector4& vUp)
{
	FXMVECTOR vEyeXM = XMVectorSet(vEye.x, vEye.y, vEye.z, vEye.w);
	FXMVECTOR vAtXM = XMVectorSet(vEye.x, vEye.y, vEye.z, vEye.w);
	FXMVECTOR vUpXM = XMVectorSet(vEye.x, vEye.y, vEye.z, vEye.w);

	m_Mat = XMMatrixLookAtLH(vEyeXM, vAtXM, vUpXM);
}

void NSMatrix::MatrixInverse()
{
	XMVECTOR vVec;
	m_Mat = XMMatrixInverse(&vVec, m_Mat);
}

void NSMatrix::MatrixTranspose()
{
	m_Mat = XMMatrixTranspose(m_Mat);
}
#endif
//EOF
