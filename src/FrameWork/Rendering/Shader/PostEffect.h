#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		PostEffect.h
	@brief		ポストエフェクト
	@author		shiba
	@date		2012/3/4 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"

/*==========================================================================*/
/*!
	@class	CPostEffectShader
	@brief	ポストエフェクト系シェーダ
*/
/*==========================================================================*/
class CPostEffectShader : public CShaderBase
{
public:
	CPostEffectShader();
	~CPostEffectShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 各種値設定
	void SetProjection(NSMatrix& ProjMat);
	void SetView(NSMatrix& ViewMat);

	//@brief タイプ指定
	void SetPostEffectType(const int nType);
	void SetBloomEnable(const bool bEnable);
	void ChangeBloomEnable()
	{
		SetBloomEnable(!m_bBloomEnable);
	}
	void SetLocal(NSMatrix& Local);

	enum POST_EFEFCT_TYPE
	{
		ePOST_EFFECT_TYPE_NONE,
		ePOST_EFFECT_TYPE_NEGA,
		ePOST_EFFECT_TYPE_MONO,
		ePOST_EFFECT_TYPE_SEPIA,
		eMAX
	};

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
		int nPostEffectType;	//ポストエフェクトタイプ
		int nBloomOnOff;		//Bloom処理するか？
		NSVector2 alignment;	//アライン
	};
	
	static const int CONBUFFERNUM = 2; //バッファ数
	ID3D11Buffer* mpNeverBuffer;
	ID3D11Buffer* mpEveryObjectBuffer;

	cbNeverChanges m_cNeverChange;
	cbChangesEveryObject m_cEveryChange;

	bool m_bBloomEnable;
};

//EOF

