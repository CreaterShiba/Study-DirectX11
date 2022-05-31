#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <DirectXMath.h>

//XMMATRIXにおきかえ中

#if 0
class NSMatrix
{
public:
	DirectX::XMMATRIX m_Mat;

	// 演算子
	NSMatrix operator+ (NSMatrix& v);
	NSMatrix operator- (NSMatrix& v);
	NSMatrix operator* (NSMatrix& v);
	NSMatrix operator* (f32 mul);
	NSMatrix operator/ (f32 div);

	// 代入演算子
	NSMatrix& operator= (const NSMatrix& v);
	NSMatrix& operator+= (const NSMatrix& v);
	NSMatrix& operator-= (const NSMatrix& v);
	NSMatrix& operator*= (f32 mul);
	NSMatrix& operator*= (const NSMatrix& v);
	NSMatrix& operator/= (f32 div);

//private:
//	// 比較演算子	
//	bool operator== (const NSMatrix& v)const;
//	bool operator!= (const NSMatrix& v)const;

public:
	void Identity();
	void MatrixLookAtLH(const NSVector4& vEye, const NSVector4& vAt, const NSVector4& vUp);
	void MatrixInverse();
	void MatrixTranspose();
};
#endif
#endif // __MATRIX_H__

//EOF 