#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GetBright.h
	@brief		輝度抽出
	@author		shiba
	@date		2015/4/5 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"

/*==========================================================================*/
/*!
	@class	CGetBrightShader
	@brief	輝度抽出シェーダ
*/
/*==========================================================================*/
class CGetBrightShader : public CShaderBase
{
public:
	CGetBrightShader();
	~CGetBrightShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 各種値設定
	void SetProjection(NSMatrix& ProjMat);
	void SetView(NSMatrix& ViewMat);
	void SetLocal(NSMatrix& Local);

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
	cbChangesEveryObject m_cEveryChange;
};

//EOF

