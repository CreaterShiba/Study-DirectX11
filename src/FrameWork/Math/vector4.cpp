/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		vector4.cpp
	@brief		4次元ベクトル
	@author		shiba
	@date		2013/2/15 create this file.
	@note		DirectXMathのラッパー
*/
/*==========================================================================*/
/*==========================================================================*/


#include "vector4.h"
#if 0
#include <cmath>   
#include "FrameWork/Utility/UtilMath.h"

// コンストラクタ
NSVector4::NSVector4() : x(0.0f), y(0.0f) {}
NSVector4::NSVector4(const NSVector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
NSVector4::NSVector4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w) {}
NSVector4::NSVector4(f32 num) : x(num), y(num), z(num), w(num) {}

// 演算子
const NSVector4 NSVector4::operator+ (const NSVector4& v)const
{
	return NSVector4(x + v.x, y + v.y, z + v.z, w + v.w);
}
const NSVector4 NSVector4::operator- (const NSVector4& v)const
{
	return NSVector4(x - v.x, y - v.y, z - v.z, w - v.w);
}
const NSVector4 NSVector4::operator* (f32 mul)const
{
	return NSVector4(x * mul, y * mul, z * mul, w * mul );
}
const NSVector4 NSVector4::operator/ (f32 div)const
{
	return NSVector4(x / div, y / div, z / div, w / div);
}


// 代入演算子
NSVector4& NSVector4::operator= (const NSVector4& v)
{
	if (this == &v) return *this;
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}
NSVector4& NSVector4::operator+= (const NSVector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}
NSVector4& NSVector4::operator-= (const NSVector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.x;
	w -= v.y;
	return *this;
}
NSVector4& NSVector4::operator*= (f32 mul)
{
	x *= mul;
	y *= mul;
	z *= mul;
	w *= mul;
	return *this;
}
NSVector4& NSVector4::operator/= (f32 div)
{
	x /= div;
	y /= div;
	z /= div;
	w /= div;
	return *this;
}


// 比較演算子	
bool NSVector4::operator== (const NSVector4& v)const
{
	return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
}
bool NSVector4::operator!= (const NSVector4& v)const
{
	return !(*this == v);
}

// ベクトルを正規化する
NSVector4& NSVector4::Normalize()
{
	f32 length = GetLength();
	if (length != 0.0f)
	{
		x /= length;
		y /= length;
		z /= length;
		w /= length;
	}
	return *this;
}

// ベクトルの長さを設定する
NSVector4& NSVector4::SetLength(f32 len)
{
	f32 length = GetLength();
	x = len * x / length;		// 元のベクトルと相似関係にあるので比で算出する
	y = len * y / length;
	z = len * z / length;

	return *this;
}

//// ベクトルを回転する (1)ラジアン角を指定
//NSVector4& NSVector4::SetAngleRad(f32 rad)
//{
//	NSVector4 temp(0, 0, 0);
//	temp.SetLength(GetLength());
//	f32 cos_ = std::cos(rad);
//	f32 sin_ = std::sin(rad);
//	x = temp.x * cos_ - temp.y * sin_;
//	y = temp.x * sin_ + temp.y * cos_;
//	return *this;
//}
//
//// ベクトルを回転する (2)ディグリー角を指定
//NSVector4& NSVector4::SetAngleDeg(f32 deg)
//{
//	return SetAngleRad(UtilMath::DegreeToRadian(deg));
//}

//// 指定した長さと角度のベクトルへ変更 (1)ラジアン角
//NSVector4& NSVector4::SetVectorR(f32 len, f32 rad)
//{
//	SetLength(len);
//	SetAngleRad(rad);
//	return *this;
//}

//// 指定した長さと角度のベクトルへ変更 (2)ディグリー角
//NSVector4& NSVector4::SetVectorD(f32 len, f32 deg)
//{
//	return SetVectorR(len, UtilMath::DegreeToRadian(deg));
//}

// ベクトルの長さを取得する
f32 NSVector4::GetLength() const
{
	//return std::sqrt(x * x + y * y);
	return std::sqrt(x * x + y * y + z * z + w * w);
}

// ベクトルの長さの2乗を取得する
float NSVector4::GetLengthSquare()const
{
	return x * x + y * y;
}

//// ベクトルの角度を取得する (1)ラジアン角
//float NSVector4::GetAngleRad()const
//{
//	return GetAngleRad(NSVector4(x, y, 0));
//}
//
//// ベクトルの角度を取得する (2)ディグリー角
//float NSVector4::GetAngleDeg()const
//{
//	return GetAngleDeg(NSVector4(x, y, 0));
//}

// 内積
float NSVector4::Dot(const NSVector4& v)const
{
	return x * v.x + y * v.y;
}

// 外積
void NSVector4::Cross(NSVector4& outV, const NSVector4& inV, const NSVector4& inV2)
{
	outV.x = inV.y * inV2.z - inV.z * inV2.y;
	outV.y = inV.z * inV2.x - inV.x * inV2.z;
	outV.z = inV.x * inV2.y - inV.y * inV2.x;
	outV.w = 0.0f;	// 0.0fにしておく 
}

// 射影
NSVector4 NSVector4::Projection(const NSVector4& v)const
{
	float len = v.GetLength();
	return v * (Dot(v) / (len * len));
}

// 射影の長さ
float NSVector4::ProjectionLength(const NSVector4& v)const
{
	return Dot(v) / v.GetLength();
}
//
//// vベクトルとの角度を算出 (1)ラジアン角を取得
//float NSVector4::GetAngleRad(const NSVector4& v)const
//{
//	//  マイナス角度からプラス角度へ補正されたディグリー角をラジアンに変換する
//	float angle = GetAngleDeg(v);
//	return UtilMath::DegreeToRadian(angle);
//}
//
//// vベクトルとの角度を算出 (2)ディグリー角を取得
//float NSVector4::GetAngleDeg(const NSVector4& v)const
//{
//	float temp = Dot(v) / (GetLength() * v.GetLength());
//	float rad = UtilMath::ACos(temp);
//
//	Vector4 out;
//	Cross(out, m)
//
//	if (Cross(v) >= 0.0f) rad *= -1.0f;
//	float f_angle = UtilMath::RadianToDegree(rad);
//
//	int i_angle = static_cast<int>(f_angle);			// 小数の値を残す
//	f_angle -= static_cast<float>(i_angle);
//	i_angle = (i_angle + 360) % 360;				// マイナス角度をプラスへ補正する
//	return i_angle + f_angle;
//}
//
//// vベクトルへ向かうベクトルを取得
//NSVector4 NSVector4::Distance(const NSVector4& v)const
//{
//	return NSVector4(v.x - x, v.y - y, v.z - z, v.w - w);
//}
#endif

//EOF
