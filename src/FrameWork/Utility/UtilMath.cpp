/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilMath.cpp
	@brief		汎用演算
	@author		shiba
	@date		2012/7/23 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "UtilMath.h"
#include "FrameWork/DXUtil/DXMng.h"

using namespace UtilMath;
using namespace DirectX;

//---------------------------------------------------------------------------
/*!
@brief	角度から2Dベクトル算出
*/
//---------------------------------------------------------------------------
NSVector2 UtilMath::DegreeToVector(const f32 fDegree)
{
	NSVector2 vec;
	vec.x = Cos(DegreeToRadian(fDegree));
	vec.y = Sin(DegreeToRadian(fDegree));

	return vec;
}

//---------------------------------------------------------------------------
/*!
@brief		球座標系算出
@param[in]	r	半径
@param[in]	fDegreeX	X角度
@param[in]	fDegreeZ	Z角度
*/
//---------------------------------------------------------------------------
//NSVector3 UtilMath::CalcSpherePos( const f32 r, const f32 fDegreeX, const f32 fDegreeZ )
//{
//	f32 xRadian = DegreeToRadian(fDegreeX);	//θ
//	f32 zRadian = DegreeToRadian(fDegreeZ);	//φ
//
//	f32 sx = Sin( xRadian );
//	f32 sz = Sin( zRadian );
//	f32 cx = Cos( xRadian );
//	f32 cz = Cos( zRadian );
//
//	NSVector3 vec;
//	vec.x = r * sx * sz;
//	vec.y = r * cx;
//	vec.z = r * sx * cz;
//	
//	return vec;
//}

//----------------------------------------------------------------------------
/*!
@brief 			ラジアン → 角度

@param[in]		f32			ラジアン
@retval			f32			角度
*/
//----------------------------------------------------------------------------
f32 UtilMath::RadianToDegree(f32 fRadian)
{
	return fRadian / (PI / 180.0f);
}

//----------------------------------------------------------------------------
/*!
@brief 			角度→ラジアン

@param[in]		f32			角度
@retval			f32			ラジアン
*/
//----------------------------------------------------------------------------
f32 UtilMath::DegreeToRadian(f32 fDegree)
{
	return PI / 180.0f * fDegree;
}

//----------------------------------------------------------------------------
/*!
@brief			サイン
@param[in]		f32			ラジアン角度
@return			f32			サイン値
*/
//----------------------------------------------------------------------------
f32			UtilMath::Sin(f32 fRadian)
{
	return sinf(fRadian);
}


//----------------------------------------------------------------------------
/*!
@brief			コサイン
@param[in]		f32			ラジアン角度
@return			f32			コサイン値
*/
//----------------------------------------------------------------------------
f32			UtilMath::Cos(f32 fRadian)
{
	return cosf(fRadian);
}

//----------------------------------------------------------------------------
/*!
@brief			タンジェント
@param[in]		f32			ラジアン角度
@return			f32			タンジェント値
*/
//----------------------------------------------------------------------------
f32			UtilMath::Tan(f32 fRadian)
{
	return tanf(fRadian);
}

//----------------------------------------------------------------------------
/*!
@brief			アークサイン
@param[in]		f32			値
@return			f32			ラジアン角度
*/
//----------------------------------------------------------------------------
f32			UtilMath::ASin(f32 fValue)
{
	return asinf(fValue);
}

//----------------------------------------------------------------------------
/*!
@brief			アークコサイン
@param[in]		f32			値
@return			f32			ラジアン角度
*/
//----------------------------------------------------------------------------
f32			UtilMath::ACos(f32 fValue)
{
	return acosf(fValue);
}

//----------------------------------------------------------------------------
/*!
@brief			アークタンジェント
@param[in]		f32			値
@return			f32			ラジアン角度
*/
//----------------------------------------------------------------------------
f32			UtilMath::ATan(f32 fValue)
{
	return atanf(fValue);
}

//----------------------------------------------------------------------------
/*!
	@brief			アークタンジェント
	@param[in]		f32	fY
	@param[in]		f32	fX
	@return			ラジアン角度
*/
//----------------------------------------------------------------------------
f32			UtilMath::ATan2(f32 fY, f32 fX)
{
	return atan2f(fY, fX);
}

#if true

//---------------------------------------------------------------------------
/*!
@brief	正射影行列取得
@return 正射影行列
*/
//---------------------------------------------------------------------------
NSMatrix UtilMath::GetOrthoMatrix()
{
	NSMatrix orthoMat = XMMatrixIdentity();

	DXMng* pcDXM = DXMng::GetInstance();
	if (pcDXM)
	{
		u32 unWinSizeX = pcDXM->GetSwapChainDesc().BufferDesc.Width;
		u32 unWinSizeY = pcDXM->GetSwapChainDesc().BufferDesc.Height;

		//正射影行列を作成。
		orthoMat._11 = 2.0f / static_cast<f32>(unWinSizeX);
		orthoMat._22 = 2.0f / static_cast<f32>(unWinSizeY);
	}

	return orthoMat;
}

//----------------------------------------------------------------------------
/*	@brief	テクスチャ座標系行列取得
*/
//----------------------------------------------------------------------------
NSMatrix UtilMath::GetTextureTransFormMatrix()
{
#if false
	//[0.5, 0, 0, 0.5]
	//[ 0, -0.5, 0, 0.5]
	//[ 0, 0, 1, 0]
	//[ 0, 0, 0, 1]
	//テクスチャ座標系に変換する為の行列。
	//テクスチャを射影する時とかによく使うので。
	NSMatrix mTextureMat;
	mTextureMat._11 = 0.5f;	mTextureMat._12 = 0;
	mTextureMat._13 = 0;	mTextureMat._14 = 0.5f;
	mTextureMat._21 = 0;	mTextureMat._22 = -0.5f;
	mTextureMat._23 = 0;	mTextureMat._24 = 0.5f;
	mTextureMat._31 = 0;	mTextureMat._32 = 0;
	mTextureMat._33 = 1.0f;	mTextureMat._34 = 0;
	mTextureMat._41 = 0;	mTextureMat._42 = 0;
	mTextureMat._43 = 0;	mTextureMat._44 = 1.0f;

#else
	//[0.5, 0, 0, 0]
	//[ 0, -0.5, 0, 0]
	//[ 0, 0, 1, 0]
	//[ 0.5, 0.5, 0, 1]
	//テクスチャ座標系に変換する為の行列。
	//テクスチャを射影する時とかによく使うので。

	NSMatrix mTextureMat;
	mTextureMat._11 = 0.5f;	mTextureMat._12 = 0;
	mTextureMat._13 = 0;	mTextureMat._14 = 0.0f;
	mTextureMat._21 = 0;	mTextureMat._22 = -0.5f;
	mTextureMat._23 = 0;	mTextureMat._24 = 0.0f;
	mTextureMat._31 = 0;	mTextureMat._32 = 0;
	mTextureMat._33 = 1.0f;	mTextureMat._34 = 0;
	mTextureMat._41 = 0.5f;	mTextureMat._42 = 0.5f;
	mTextureMat._43 = 0;	mTextureMat._44 = 1.0f;

#endif
	return mTextureMat;
}

//---------------------------------------------------------------------------
/*!
	@brief		法線から接ベクトル算出
	@param[in]	vNorm	法線
*/
//---------------------------------------------------------------------------
NSVector4 UtilMath::ConvertNomToTangent(const NSVector4& vNorm)
{
	NSVector4 vTan = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	//NSVector4 vZ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//XMVector4Cross(vTan, vNorm, vZ);
	NS_UNUSED(vNorm);

	return vTan;
}

//---------------------------------------------------------------------------
/*!
	@brief		法線と接ベクトルから従法線ベクトル算出
	@param[in]	vNorm	法線
	@param[in]	vTan	接ベクトル
*/
//---------------------------------------------------------------------------
NSVector4 UtilMath::ConvertNomToBiNormal(NSVector4& vNorm, const NSVector4& vTan)
{
	NSVector4 vBi = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	//XMVector4Cross(vBi, vNorm, vTan);
	NS_UNUSED(vNorm);
	NS_UNUSED(vTan);

	return vBi;
}
//---------------------------------------------------------------------------
/*!
	@brief		３頂点から法線ベクトルの算出
	@param[in]	A,B,C	各頂点
	@param[out]	NSVector3	法線ベクトル
*/
//---------------------------------------------------------------------------
NSVector3 UtilMath::CalcNormal(const NSVector3& A, const NSVector3& B, const NSVector3& C)
{
	NSVector3 AB(B - A);
	NSVector3 BC(C - B);

	NSVector3 normal = AB.Cross(BC);	//AB BCの外積
	normal.Normalize();					//単位ベクトルにする

	return normal;
}

//---------------------------------------------------------------------------
/*!
	@brief		反射ベクトル演算
	@param[out]	refNormal	反射ベクトル
	@param[in]	vMoving		進行ベクトル
	@param[in]	normal		法線ベクトル
	@return		反射ベクトル
*/
//---------------------------------------------------------------------------
NSVector3 UtilMath::CalcReflectVector(NSVector3* refNormal, const NSVector3& vMoving, const NSVector3& normal)
{
	NSVector3 out;

	NSVector3 normal_n = normal;
	normal_n.Normalize();

	out = (vMoving - normal_n * 2.0f * vMoving.Dot(normal_n));

	if (refNormal != nullptr)
	{
		NSVector3 outnom = out;
		outnom.Normalize();
		*refNormal = outnom;
	}

	return out;
}

//---------------------------------------------------------------------------
/*!
	@brief		ベクトル射影
	@param[out]	wallNormal	射影ベクトル（正規化済み）
	@param[in]	vMoving		射影をするベクトル
	@param[in]	normal		射影対象ベクトル
	@return		射影ベクトル
*/
//---------------------------------------------------------------------------
NSVector3 UtilMath::CalcProjVec(NSVector3* wallNormal, const NSVector3& vMoveDir, const NSVector3& normal)
{
	NSVector3 out;

	NSVector3 normal_n = normal;
	normal_n.Normalize();

	out = vMoveDir -  normal_n * vMoveDir.Dot(normal_n);

	if (wallNormal != nullptr)
	{
		NSVector3 outnom = out;
		outnom.Normalize();
		*wallNormal = outnom;
	}

	return out;
}

#endif

//EOF