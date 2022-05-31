/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Vector3.cpp
	@brief		3次元ベクトル
	@author		shiba
	@date		2013/2/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#if 1

#include "vector3.h"
#include <cmath>   
#include "FrameWork/Utility/UtilMath.h"

// コンストラクタ
NSVector3::NSVector3() : x(0.0f), y(0.0f), z(0.0f) {}
NSVector3::NSVector3(const NSVector3& v) : x(v.x), y(v.y), z(v.z) {}
NSVector3::NSVector3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

// 演算子
const NSVector3 NSVector3::operator+ (const NSVector3& v)const
{
	return NSVector3(x + v.x, y + v.y, z + v.z);
}
const NSVector3 NSVector3::operator- (const NSVector3& v)const
{
	return NSVector3(x - v.x, y - v.y, z - v.z);
}
//const NSVector3 NSVector3::operator* (const NSVector3& v)const
//{
//	return NSVector3(x * v.x, y * v.y, z * v.z);
//}
const NSVector3 NSVector3::operator* (f32 mul)const
{
	return NSVector3(x * mul, y * mul, z * mul);
}
const NSVector3 NSVector3::operator/ (f32 div)const
{
#if defined( _DEBUG )
	NS_ASSERT(div != 0.0f);
#endif
	return NSVector3(x / div, y / div, z / div);
}

const NSVector3 NSVector3::operator- ()
{
	x = -x; y = -y; z = -z;
	return *this;
}

// 代入演算子
NSVector3& NSVector3::operator= (const NSVector3& v)
{
	if (this == &v) return *this;
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}
NSVector3& NSVector3::operator+= (const NSVector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
NSVector3& NSVector3::operator-= (const NSVector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
NSVector3& NSVector3::operator*= (f32 mul)
{
	x *= mul;
	y *= mul;
	z *= mul;
	return *this;
}
NSVector3& NSVector3::operator/= (f32 div)
{
	x /= div;
	y /= div;
	z /= div;
	return *this;
}


// 比較演算子	
bool NSVector3::operator== (const NSVector3& v)const
{
	return (x == v.x) && (y == v.y) && (z == v.z);
}
bool NSVector3::operator!= (const NSVector3& v)const
{
	return !(*this == v);
}


// ベクトルを正規化する
NSVector3& NSVector3::Normalize()
{
	f32 length = GetLength();
	if (length != 0.0f)
	{
		x /= length;
		y /= length;
		z /= length;
	}
	return *this;
}

// ベクトルの長さを設定する
NSVector3& NSVector3::SetLength(f32 len)
{
	f32 length = GetLength();
	x = len * x / length;		// 元のベクトルと相似関係にあるので比で算出する
	y = len * y / length;
	z = len * z / length;

	return *this;
}

// 0ベクトルにする
void NSVector3::SetZero()
{
	x = y = z = 0.0f;
}

// 1ベクトルにする
void NSVector3::SetOne()
{
	x = y = z = 1.0f;
}

// ベクトルの長さを取得する
f32 NSVector3::GetLength()const
{
	return std::sqrt(GetLengthSquare());
}

// ベクトルの長さの2乗を取得する
f32 NSVector3::GetLengthSquare()const
{
	return x * x + y * y + z * z;
}

// 内積
f32 NSVector3::Dot(const NSVector3& v)const
{
	return (v.x * x + v.y * y + v.z * z);
}

// 外積
NSVector3 NSVector3::Cross(const NSVector3& v)const
{
	NSVector3 dv;
	dv.x = y * v.z - z * v.y;
	dv.y = z * v.x - x * v.z;
	dv.z = x * v.y - y * v.x;

	return dv;
}

// 射影
NSVector3 NSVector3::Projection(const NSVector3& v)const
{
	f32 len = v.GetLength();
	return v * (Dot(v) / (len * len));
}

// 射影の長さ
f32 NSVector3::ProjectionLength(const NSVector3& v)const
{
	return Dot(v) / v.GetLength();
}

// vベクトルへ向かうベクトルを取得
NSVector3 NSVector3::Distance(const NSVector3& v)const
{
	return NSVector3(v.x - x, v.y - y, v.z - z);
}

NSVector3 NSVector3::Lerp(const NSVector3& v, const NSVector3& v2, f32 fLerp )
{
	NSVector3 vOut;

	if (fLerp > 1.0f)
		fLerp = 1.0f;

	if (fLerp < 0.0f)
		fLerp = 0.0f;

	vOut.x = v.x + fLerp * (v2.x - v.x);
	vOut.y = v.y + fLerp * (v2.y - v.y);
	vOut.z = v.z + fLerp * (v2.z - v.z);

	return vOut;
}

#endif

//EOF
