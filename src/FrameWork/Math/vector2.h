
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Vector2.h
	@brief		2次元ベクトル
	@author		shiba
	@date		2013/2/15 create this file.
	@note		メンドイのでひとまず	http://magotaru.blog49.fc2.com/blog-entry-131.htmlから
*/
/*==========================================================================*/
/*==========================================================================*/

/*==========================================================================*/
/*!
@class	NSVector3
@brief	2Dベクトル
*/
/*==========================================================================*/
class NSVector2
{
	// 角度0の基準方向
	static const NSVector2 PIVOT_VECTOR2;
public:
	NSVector2();
	NSVector2(const NSVector2& v);
	NSVector2(f32 x, f32 y);
	NSVector2(f32 num);

	// 演算子
	const NSVector2 operator+ (const NSVector2& v)const;
	const NSVector2 operator- (const NSVector2& v)const;
	const NSVector2 operator* (f32 mul)const;
	const NSVector2 operator/ (f32 div)const;
	const NSVector2 operator -();

	// 代入演算子
	NSVector2& operator= (const NSVector2& v);
	NSVector2& operator+= (const NSVector2& v);
	NSVector2& operator-= (const NSVector2& v);
	NSVector2& operator*= (f32 mul);
	NSVector2& operator/= (f32 div);

	// 比較演算子	
	bool operator== (const NSVector2& v)const;
	bool operator!= (const NSVector2& v)const;

public:
	// 設定関数
	NSVector2& Normalize();								// ベクトルを正規化する
	NSVector2& SetLength(f32 len);	 					// ベクトルの長さを設定する
	NSVector2& SetAngleRad(f32 rad);					// ベクトルを回転する (1)ラジアン角を指定
	NSVector2& SetAngleDeg(f32 deg);					// ベクトルを回転する (2)ディグリー角を指定
	NSVector2& SetVectorR(f32 len, f32 rad);			// 指定した長さと角度のベクトルへ変更 (1)ラジアン角
	NSVector2& SetVectorD(f32 len, f32 deg);			// 指定した長さと角度のベクトルへ変更 (2)ディグリー角
	void SetZero();										// 0ベクトルにする

															// 取得関数
	f32 GetLength()const;							// ベクトルの長さを取得する
	f32 GetLengthSquare()const;						// ベクトルの長さの2乗を取得する
	f32 GetAngleRad()const;							// ベクトルの角度を取得する (1)ラジアン角
	f32 GetAngleDeg()const;							// ベクトルの角度を取得する (2)ディグリー角

														// 他のベクトルとの関連
	f32 Dot(const NSVector2& v)const;					// 内積
	f32 Cross(const NSVector2& v)const;				// 外積
	NSVector2 Projection(const NSVector2& v)const;		// 射影
	f32 ProjectionLength(const NSVector2& v)const;		// 射影の長さ 
	f32 GetAngleRad(const NSVector2& v)const;			// vベクトルとの角度を算出 (1)ラジアン角を取得
	f32 GetAngleDeg(const NSVector2& v)const;			// vベクトルとの角度を算出 (2)ディグリー角を取得
	NSVector2 Distance(const NSVector2& v)const;			// vベクトルへ向かうベクトルを取得

	static NSVector2 Lerp(const NSVector2& v, const NSVector2& v2, f32 fLerp);		//線形補間
	static NSVector2 Spline(
		const NSVector2 &v, const NSVector2& v2,
		const NSVector2 &vMove, const NSVector2& vMove2,
		const f32 fLerp);	//丸み不均一スプライン
public:
	f32 x;
	f32 y;
};

//EOF

