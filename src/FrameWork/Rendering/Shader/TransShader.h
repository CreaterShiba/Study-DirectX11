#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		TransShader.h
	@brief		ただの変換シェーダ(じゃなくなってきた)
	@author		ta.Ishii
	@date		2012/3/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"


/*==========================================================================*/
/*!
	@class	CTransShader
	@brief		ただの変換シェーダ(じゃなくなってきた)
*/
/*==========================================================================*/
class CTransShader : public CShaderBase
{

public:
	CTransShader();
	~CTransShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 各種値設定
	void SetProjection( NSMatrix& ProjMat );
	void SetView( NSMatrix& ViewMat );
	void SetLocal( NSMatrix& Local );
	void SetLocalWLPWLPT( const NSMatrix& mW, const NSMatrix& mW2, const NSMatrix& mW3, 
		const NSVector3& vLightPos,
		const NSVector3& vEyePos,
		const NSMatrix& wvp );

	//視線座標の設定
	void SetEyePos( const NSVector3& vEyePos );

	void SetWVP( NSMatrix& wvpMat );
private:

	//ほぼ変更がない
	struct cbNeverChanges 
	{
		NSMatrix Projection;	//透視変換
	};

	//たまに変更
	struct cbChangesEveryFrame 
	{
		NSMatrix View;			//ビュー変換
	};

	//毎フレーム変更
	struct cbChangesEveryObject 
	{
		NSMatrix World;			//ワールド変換行列
		NSMatrix WLP;			//World LightLocal Projection
		NSMatrix WLPT;			//World LightLocal Projection
		NSVector4 vLightPos;		//ライトの座標
		NSVector4 vEyePos;		//視点座標	
		NSMatrix wvp;			//WVP
	};

	static const int CONBUFFERNUM = 3; //バッファ数
	ID3D11Buffer* mpNeverBuffer;
	ID3D11Buffer* mpEveryFrameBuffer;
	ID3D11Buffer* mpEveryObjectBuffer;

	cbChangesEveryObject m_cbEveryChange;	//!< 保存しておく
};

//EOF