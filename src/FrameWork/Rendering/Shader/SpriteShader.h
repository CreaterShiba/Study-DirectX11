#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SpriteShader.h
	@brief		スプライト用シェーダ
	@author		shiba
	@date		2013/1/14 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"

/*==========================================================================*/
/*!
	@class	CSpriteShader
	@brief	スプライト用シェーダ
*/
/*==========================================================================*/
class CSpriteShader : public CShaderBase
{

public:
	CSpriteShader();
	~CSpriteShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 各種値設定
	void SetProjection( NSMatrix& ProjMat );
	void SetView( NSMatrix& ViewMat );
	void SetLocal( NSMatrix& Local );

private:

	//ほぼ変更がない
	struct cbNeverChanges 
	{
		NSMatrix Projection;//透視変換
		NSMatrix View;		//ビュー
	};
	//毎フレーム変更
	struct cbChangesEveryObject 
	{
		NSMatrix World;			//ワールド変換行列
	};

	static const int CONBUFFERNUM = 2; //バッファ数
	ID3D11Buffer* mpNeverBuffer;
	ID3D11Buffer* mpEveryObjectBuffer;
	cbNeverChanges m_cNeverChange;
};

//EOF
