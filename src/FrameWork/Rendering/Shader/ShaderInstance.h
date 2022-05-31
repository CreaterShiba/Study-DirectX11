#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderInstance.h
	@brief		インスタンス描画
	@author		ta.Ishii
	@date		2013/5/19 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "ShaderBase.h"

/*==========================================================================*/
/*!
	@class	CShaderInstance
	@brief	インスタンス描画
*/
/*==========================================================================*/
class CShaderInstance : public CShaderBase
{
public:
	CShaderInstance();
	~CShaderInstance();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	// @brief 行列設定
	void setMatrixs( 
		const NSMatrix* pMatrix, 
		const NSMatrix& view,  
		const NSMatrix& proj );
	// @brief ライト座標
	void SetLightPos( const NSVector3& lightPos );

private:

	struct cbChangesEveryObject 
	{
		NSVector3 lightPos;	//ライト座標
	};

	//!< バッファ数
	static const s32 CONBUFFERNUM = 1; 

	//!< バッファ
	ID3D11Buffer* mpEveryObjectBuffer;

public:
	//!< インスタンス数
	static const s32 INSTANCE_NUM = 20*20;

	//!< 行列用バッファ
	ID3D11Buffer* mpMatrixBuffer;
};

//EOF