#ifndef __VECTOR4_H__
#define __VECTOR4_H__

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		vector4.h
	@brief		4次元ベクトル
	@author		shiba
	@date		2013/2/15 create this file.
	@note		DirectXMathのラッパー
*/
/*==========================================================================*/
/*==========================================================================*/

//XMVECTORにおきかえ中

#if 0
class NSVector4
{
public:
	NSVector4();
	NSVector4(const NSVector4& v);
	NSVector4(f32 x, f32 y, f32 z, f32 w);
	NSVector4(f32 num);

	// 演算子
	const NSVector4 operator+ (const NSVector4& v)const;
	const NSVector4 operator- (const NSVector4& v)const;
	const NSVector4 operator* (f32 mul)const;
	const NSVector4 operator/ (f32 div)const;

	// 代入演算子
	NSVector4& operator= (const NSVector4& v);
	NSVector4& operator+= (const NSVector4& v);
	NSVector4& operator-= (const NSVector4& v);
	NSVector4& operator*= (f32 mul);
	NSVector4& operator/= (f32 div);

	// 比較演算子	
	bool operator== (const NSVector4& v)const;
	bool operator!= (const NSVector4& v)const;

public:
	// 設定関数
	NSVector4& Normalize();				// ベクトルを正規化する
	NSVector4& SetLength(f32 len);	 				// ベクトルの長さを設定する
	//NSVector4& SetVectorR(f32 len, f32 rad);		// 指定した長さと角度のベクトルへ変更 (1)ラジアン角
	//NSVector4& SetVectorD(f32 len, f32 deg);		// 指定した長さと角度のベクトルへ変更 (2)ディグリー角

	// 取得関数
	f32 GetLength() const;				// ベクトルの長さを取得する
	f32 GetLengthSquare()const;						// ベクトルの長さの2乗を取得する
	//f32 GetAngleRad()const;							// ベクトルの角度を取得する (1)ラジアン角
	//f32 GetAngleDeg()const;							// ベクトルの角度を取得する (2)ディグリー角

	// 算術演算系
	f32 Dot(const NSVector4& v)const;				// 内積
	static void Cross(NSVector4& outV, const NSVector4& inV, const NSVector4& inV2);
	NSVector4 Projection(const NSVector4& v)const;		// 射影
	f32 ProjectionLength(const NSVector4& v)const;	// 射影の長さ 
	//f32 GetAngleRad(const NSVector4& v)const;		// vベクトルとの角度を算出 (1)ラジアン角を取得
	//f32 GetAngleDeg(const NSVector4& v)const;		// vベクトルとの角度を算出 (2)ディグリー角を取得
	//NSVector4 Distance(const NSVector4& v)const;			// vベクトルへ向かうベクトルを取得

public:
	f32 x;
	f32 y;
	f32 z;
	f32 w;
};
#endif
#endif //__VECTOR4_H__

//EOF 
