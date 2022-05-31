#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		BlurShader.h
	@brief		ブラー
	@author		shiba
	@date		2015/4/5 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"

#define GAUSSPARAM_ALIGN

/*==========================================================================*/
/*!
	@class	CBlurShader
	@brief	ブラー
*/
/*==========================================================================*/
class CBlurShader : public CShaderBase
{
public:
	CBlurShader();
	~CBlurShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 各種値設定
	void SetProjection(NSMatrix& ProjMat);
	void SetView(NSMatrix& ViewMat);
	void SetLocal(NSMatrix& Local);

	void SetBlurType(const s32 nBlurType);

private:
	void WriteGaussParam();
	void WriteGaussDrawType();

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

	static const s32 SAMPLE_COUNT = 16;

	//ガウス係数
	struct cbGaussParamObject
	{
#if defined(GAUSSPARAM_ALIGN)
		NSVector4 offsetWeights[SAMPLE_COUNT];	//32バイト毎じゃないとうまくいかない？
#else
		NSVector2 offset[SAMPLE_COUNT];	//どれぐらいずらしてサンプリングするか
		f32 weights[SAMPLE_COUNT];		//どれだけサンプリング時に影響を受けるか(ガウス関数を用いる)
#endif
	};

	struct  cbGaussDrawType
	{
		s32 nBlurType;
		NSVector3 align;
	};

	static const int CONBUFFERNUM = 4; //バッファ数
	ID3D11Buffer* mpNeverBuffer;
	ID3D11Buffer* mpEveryObjectBuffer;
	ID3D11Buffer* mpGaussParamBuffer;
	ID3D11Buffer* mpGaussDrawType;

	cbNeverChanges m_cNeverChange;
	cbChangesEveryObject m_cEveryChange;
	cbGaussParamObject m_cGaussParam;
	cbGaussDrawType m_cGaussDrawType;
};

//EOF

