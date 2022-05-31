#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		NormalMapShader.h
	@brief		法線書き込みシェーダ
	@author		ta.Ishii
	@date		2013/4/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"

/*==========================================================================*/
/*!
	@class	CZRenderShader
	@brief	法線書き込みシェーダ
*/
/*==========================================================================*/
class CNormalMapShader : public CShaderBase
{
public:
	CNormalMapShader();
	~CNormalMapShader();

	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	void SetWVP( const NSMatrix& mWVP );

private:
	struct cbChangesEveryObject 
	{
		NSMatrix mWVP;			//ワールド、ビュー、投資変換行列
	};

	const static s32 CONBUFFERNUM = 1;	//!< バッファ数
	ID3D11Buffer* mpEveryFrameBuffer;	//!< バッファ
};

//EOF