#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SkinTransShader.h
	@brief		スキンシェーダ
	@author		ta.Ishii
	@date		2012/10/30 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"

/*==========================================================================*/
/*!
	@class	CSkinTransShader
	@brief	スキンシェーダ
*/
/*==========================================================================*/
class CSkinTransShader : public CShaderBase
{
public:
	CSkinTransShader();
	~CSkinTransShader();

	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 合成行列設定
	void SetCombMatrix( NSMatrix* pCombMat, const s32 nMatSize );
	//@brief プロジェクション行列設定
	void SetProjection( NSMatrix& ProjMat );
	//@brief ビュー行列設定
	void SetView( NSMatrix& ViewMat );

private:

	struct cbChangesEveryObject 
	{
		NSMatrix World[32];		//ワールド変換行列
		NSMatrix View;			//ビュー
		NSMatrix Proj;			//プロジェクション
	};

	static const int CONBUFFERNUM = 1; //バッファ数
	ID3D11Buffer* mpEveryObjectBuffer;
	cbChangesEveryObject cb;
};

//EOF
