/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Vector2.cpp
	@brief		2次元ベクトル
	@author		shiba
	@date		2013/2/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "vector2.h"
#include <cmath>   
#include "FrameWork/Utility/UtilMath.h"

// 角度0の基準方向
// スクリーン上の0度のベクトルを指定してください。
const NSVector2 NSVector2::PIVOT_VECTOR2(1.0f, 0.0f);

// コンストラクタ
NSVector2::NSVector2() : x(0.0f), y(0.0f) {}
NSVector2::NSVector2(const NSVector2& v) : x(v.x), y(v.y) {}
NSVector2::NSVector2(f32 x, f32 y) : x(x), y(y) {}
NSVector2::NSVector2(f32 num) : x(num), y(num) {}

// 演算子
const NSVector2 NSVector2::operator+ (const NSVector2& v)const
{
	return NSVector2(x + v.x, y + v.y);
}
const NSVector2 NSVector2::operator- (const NSVector2& v)const
{
	return NSVector2(x - v.x, y - v.y);
}
const NSVector2 NSVector2::operator* (f32 mul)const
{
	return NSVector2(x * mul, y * mul);
}
const NSVector2 NSVector2::operator/ (f32 div)const
{
	return NSVector2(x / div, y / div);
}

const NSVector2 NSVector2::operator- ()
{
	x = -x; y = -y;
	return *this;
}

// 代入演算子
NSVector2& NSVector2::operator= (const NSVector2& v)
{
	if (this == &v) return *this;
	x = v.x;
	y = v.y;
	return *this;
}
NSVector2& NSVector2::operator+= (const NSVector2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}
NSVector2& NSVector2::operator-= (const NSVector2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}
NSVector2& NSVector2::operator*= (f32 mul)
{
	x *= mul;
	y *= mul;
	return *this;
}
NSVector2& NSVector2::operator/= (f32 div)
{
	x /= div;
	y /= div;
	return *this;
}


// 比較演算子	
bool NSVector2::operator== (const NSVector2& v)const
{
	return (x == v.x) && (y == v.y);
}
bool NSVector2::operator!= (const NSVector2& v)const
{
	return !(*this == v);
}


// ベクトルを正規化する
NSVector2& NSVector2::Normalize()
{
	f32 length = GetLength();
	if (length != 0.0f)
	{
		x /= length;
		y /= length;
	}
	return *this;
}

// ベクトルの長さを設定する
NSVector2& NSVector2::SetLength(f32 len)
{
	f32 length = GetLength();
	x = len * x / length;		// 元のベクトルと相似関係にあるので比で算出する
	y = len * y / length;
	return *this;
}

// ベクトルを回転する (1)ラジアン角を指定
NSVector2& NSVector2::SetAngleRad(f32 rad)
{
	NSVector2 temp(PIVOT_VECTOR2);
	temp.SetLength(GetLength());
	f32 cos_ = std::cos(rad);
	f32 sin_ = std::sin(rad);
	x = temp.x * cos_ - temp.y * sin_;
	y = temp.x * sin_ + temp.y * cos_;
	return *this;
}

// ベクトルを回転する (2)ディグリー角を指定
NSVector2& NSVector2::SetAngleDeg(f32 deg)
{
	return SetAngleRad(UtilMath::DegreeToRadian(deg));
}

// 指定した長さと角度のベクトルへ変更 (1)ラジアン角
NSVector2& NSVector2::SetVectorR(f32 len, f32 rad)
{
	SetLength(len);
	SetAngleRad(rad);
	return *this;
}

// 指定した長さと角度のベクトルへ変更 (2)ディグリー角
NSVector2& NSVector2::SetVectorD(f32 len, f32 deg)
{
	return SetVectorR(len, UtilMath::DegreeToRadian(deg));
}

// 0ベクトルにする
void NSVector2::SetZero()
{
	x = y = 0.0f;
}

// ベクトルの長さを取得する
f32 NSVector2::GetLength()const
{
	return std::sqrt(x * x + y * y);
}

// ベクトルの長さの2乗を取得する
f32 NSVector2::GetLengthSquare()const
{
	return x * x + y * y;
}

// ベクトルの角度を取得する (1)ラジアン角
f32 NSVector2::GetAngleRad()const
{
	return GetAngleRad(PIVOT_VECTOR2);
}

// ベクトルの角度を取得する (2)ディグリー角
f32 NSVector2::GetAngleDeg()const
{
	return GetAngleDeg(PIVOT_VECTOR2);
}

// 内積
f32 NSVector2::Dot(const NSVector2& v)const
{
	return x * v.x + y * v.y;
}

// 外積
f32 NSVector2::Cross(const NSVector2& v)const
{
	return x * v.y - y * v.x;
}

// 射影
NSVector2 NSVector2::Projection(const NSVector2& v)const
{
	f32 len = v.GetLength();
	return v * (Dot(v) / (len * len));
}

// 射影の長さ
f32 NSVector2::ProjectionLength(const NSVector2& v)const
{
	return Dot(v) / v.GetLength();
}

// vベクトルとの角度を算出 (1)ラジアン角を取得
f32 NSVector2::GetAngleRad(const NSVector2& v)const
{
	//  マイナス角度からプラス角度へ補正されたディグリー角をラジアンに変換する
	f32 angle = GetAngleDeg(v);
	return UtilMath::DegreeToRadian(angle);
}

// vベクトルとの角度を算出 (2)ディグリー角を取得
f32 NSVector2::GetAngleDeg(const NSVector2& v)const
{
	f32 temp = Dot(v) / (GetLength() * v.GetLength());
	f32 rad = UtilMath::ACos(temp);
	if (Cross(v) >= 0.0f) rad *= -1.0f;
	f32 f_angle = UtilMath::RadianToDegree(rad);

	int i_angle = static_cast<int>(f_angle);			// 小数の値を残す
	f_angle -= static_cast<f32>(i_angle);
	i_angle = (i_angle + 360) % 360;				// マイナス角度をプラスへ補正する
	return i_angle + f_angle;
}

// vベクトルへ向かうベクトルを取得
NSVector2 NSVector2::Distance(const NSVector2& v)const
{
	return NSVector2(v.x - x, v.y - y);
}

NSVector2 NSVector2::Lerp(const NSVector2& v, const NSVector2& v2, f32 fLerp)
{
	NSVector2 vOut;

	if (fLerp > 1.0f)
		fLerp = 1.0f;

	if (fLerp < 0.0f)
		fLerp = 0.0f;

	vOut.x = v.x + fLerp * (v2.x - v.x);
	vOut.y = v.y + fLerp * (v2.y - v.y);

	return vOut;
}

NSVector2 NSVector2::Spline(const NSVector2 &v, const NSVector2& v2, const NSVector2 &vMove, const NSVector2& vMove2, const f32 fLerp)
{
	NSVector4 t = DirectX::XMVectorSet(fLerp*fLerp*fLerp, fLerp*fLerp, fLerp, 1.0f);

	NSMatrix h;
	h._11 = 2.0f; h._12 = -2.0f; h._13 = 1.0f; h._14 = 1.0f;
	h._21 =-3.0f; h._22 =  3.0f; h._23 =-2.0f; h._24 =-1.0f;
	h._31 = 0.0f; h._32 =  0.0f; h._33 = 1.0f; h._34 = 0.0f;
	h._41 = 1.0f; h._42 =  0.0f; h._43 = 0.0f; h._44 = 0.0f;

	NSMatrix g;
	g._11 = v.x;		g._12 = v.y;		g._13 = 0.0f; g._14 = 1.0f;
	g._21 = v2.x;		g._22 = v2.y;		g._23 = 0.0f; g._24 = 1.0f;
	g._31 = vMove.x;	g._32 = vMove2.y;	g._33 = 0.0f; g._34 = 1.0f;
	g._41 = vMove2.x;	g._42 = vMove2.y;	g._43 = 0.0f; g._44 = 1.0f;

	NSVector4 temp = XMVector4Transform(t, h);
	NSVector4 ans = XMVector4Transform(temp, g);

	return NSVector2(XMVectorGetX(ans), XMVectorGetY(ans));
}

//EOF
