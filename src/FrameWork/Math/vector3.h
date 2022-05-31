
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Vector3.h
	@brief		3Dベクトル
	@author		shiba
	@date		2013/2/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

/*==========================================================================*/
/*!
@class	NSVector3
@brief	3Dベクトル
*/
/*==========================================================================*/
class NSVector3
{
public:
	NSVector3();
	NSVector3(const NSVector3& v);
	NSVector3(f32 x, f32 y, f32 z);

	// 演算子
	const NSVector3 operator+ (const NSVector3& v)const;
	const NSVector3 operator- (const NSVector3& v)const;
	//const NSVector3 operator* (const NSVector3& v)const;
	const NSVector3 operator* (f32 mul)const;
	const NSVector3 operator/ (f32 div)const;
	const NSVector3 operator -();

	// 代入演算子
	NSVector3& operator= (const NSVector3& v);
	NSVector3& operator+= (const NSVector3& v);
	NSVector3& operator-= (const NSVector3& v);
	NSVector3& operator*= (f32 mul);
	NSVector3& operator/= (f32 div);

	// 比較演算子	
	bool operator== (const NSVector3& v)const;
	bool operator!= (const NSVector3& v)const;

public:
	
	// 設定関数
	NSVector3& Normalize();								// ベクトルを正規化する
	NSVector3& SetLength(f32 len);	 					// ベクトルの長さを設定する
	void SetZero();										// 0ベクトルにする
	void SetOne();										// 1ベクトルにする

	// 取得関数
	f32 GetLength()const;								// ベクトルの長さを取得する
	f32 GetLengthSquare()const;							// ベクトルの角度を取得する (2)ディグリー角

	// 他のベクトルとの関連
	f32 Dot(const NSVector3& v)const;					// 内積
	NSVector3 Cross(const NSVector3& v)const;			// 外積
	NSVector3 Projection(const NSVector3& v)const;		// 射影
	f32 ProjectionLength(const NSVector3& v)const;		// 射影の長さ 

	NSVector3 Distance(const NSVector3& v)const;		// vベクトルへ向かうベクトルを取得

	static NSVector3 Lerp(const NSVector3& v, const NSVector3& v2, f32 fLerp);

public:
	f32 x;
	f32 y;
	f32 z;
};

//EOF

