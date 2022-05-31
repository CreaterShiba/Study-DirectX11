#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		TessellationShader.h
	@brief		テッセレーションテスト
	@author		ta.Ishii
	@date		2013/6/30 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "ShaderBase.h"
#include "FrameWork/Utility/UtilMacro.h"

/*==========================================================================*/
/*!
	@class	CTessellationShader
	@brief	テスト用テッセレーションシェーダ
*/
/*==========================================================================*/
class CTessellationShader : public CShaderBase
{
public:
	CTessellationShader();
	~CTessellationShader();
	
public:
	// @brief 初期化
	virtual bool Init();

	// @brief シェーダロード
	virtual HRESULT LoadShader();

	// @brief シェーダ有効
	virtual bool ShaderOn();

	// @brief 変換行列設定
	void setWVP( const NSMatrix& wMat );

	// @brief テッセレーション係数設定
	void setTessellationConf( const f32 fTes1, const f32 fTes2 );

private:
	//!< 必要バッファ1
	ID3D11Buffer*	m_pConstantBuffer;

	//!< 必要バッファ2
	ID3D11Buffer*	m_pConstantBuffer2;

	// 定数定義
	typedef struct _CBUFFER0
	{
		NSMatrix  g_matWVP; // ワールド × ビュー × 射影 行列
	} CBUFFER0;

	typedef struct _CBUFFER1
	{
		f32 g_fTessFactor;       // ポリゴンのエッジのテッセレーション係数
		f32 g_fInsideTessFactor; // ポリゴン内部のテッセレーション係数
		f32 g_Dummy2;
		f32 g_Dummy3;
	} CBUFFER1;

	//!< WVP
	CBUFFER0 m_cBuffer;

	//!< 係数いろいろ
	CBUFFER1 m_cBuffer2;
};

//EOF