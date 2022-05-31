#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilMath.h
	@brief		算術ユーティリティ
	@author		shiba
	@date		2012/7/23 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "UtilDefine.h"

namespace UtilMath
{
	//!< 円周率
	const f32 PI = 3.141592654f;

	//!<  掛けるとディグリー角へ変換される
	const f32 RadToDeg = 57.29577951f ;	

	//!< 掛けるとラジアン角へ変換される
	const f32 DegToRad = 0.017453293f ;	

	// @brief Abs
	template<class T>
	T Abs( T _a )
	{
		return ( (_a) > 0 ) ? (_a) : -(_a);
	}

	//@brief 角度から2Dベクトルを算出
	NSVector2 DegreeToVector( const f32 fDegree );	

	//@brief 球座標算出(Spherical Polar Coordinates)
	//NSVector3 CalcSpherePos( const f32 r, const f32 fDegreeX, const f32 fDegreeZ );

	// @brief ラジアン→角度変換
	f32 RadianToDegree( f32 fRadian );				//ラジアン→角度変換

	// @brief 角度→ラジアン変換
	f32 DegreeToRadian( f32 fDegree );

	// @brief サイン
	f32	Sin( f32 fRadian );

	// @brief コサイン
	f32 Cos( f32 fRadian );	

	// @brief タンジェント
	f32 Tan( f32 fRadian );							

	// @brief アークサイン
	f32 ASin( f32 fValue );							

	// @brief アークコサイン
	f32 ACos( f32 fValue );							

	// @brief アークタンジェント
	f32 ATan( f32 fValue );							

	// @brief アークタンジェント
	f32 ATan2( f32 fY, f32 fX );

	NSMatrix GetOrthoMatrix();

	NSMatrix GetTextureTransFormMatrix();

	NSVector4 ConvertNomToTangent(const NSVector4& vNorm);

	NSVector4 ConvertNomToBiNormal(NSVector4& vNorm, const NSVector4& vTan);

	// @brief 数値制限
	template <class T>
	inline T Clamp(T num, T minNum, T maxNum)
	{
		if (num > maxNum)
		{
			num = maxNum;
		}

		if (num < minNum)
		{
			num = minNum;
		}
		return num;
	}
	
	//3頂点から法線を求める
	NSVector3 CalcNormal(const NSVector3& A, const NSVector3& B, const NSVector3& C);

	//反射ベクトル演算
	NSVector3 CalcReflectVector(NSVector3* refNormal, const NSVector3& vMoving, const NSVector3& normal);

	//ベクトル射影
	NSVector3 CalcProjVec(NSVector3* projNormal, const NSVector3& vMoving, const NSVector3& normal);
}

//EOF
