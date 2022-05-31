#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderManager.h
	@brief		シェーダーマネージャー
	@author		shiba
	@date		2012/3/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Utility/UtilMacro.h"
#include "FrameWork/Utility/UtilSafeArray.h"

class CShaderBase;
/*==========================================================================*/
/*!
	@class	CShaderManager
	@brief	シェーダマネージャ
*/
/*==========================================================================*/
class CShaderManager
{
	//シングルトン
	NS_SINGLETON( CShaderManager );
	CShaderManager();
	~CShaderManager();

public:
	enum E_SHADERID
	{
		eSHADERID_MRT,							//マルチレンダリングターゲット(Pos,Nom,color)
		eSHADERID_ZRENDER,						//Z値描画シェーダ
		eSHADERID_SPRITE,						//スプライトシェーダ
		eSHADERID_DEFFERD,						//DefferdShading
		eSHADERID_GETBRIGHT,					//輝度抽出
		eSHADERID_BLUR,							//ブラー
		eSHADERID_POSTEFFECT,					//ポストエフェクト(分割予定)

		//eSHADERID_GEOMETRYTEST,					//ジオメトリシェーダテスト
		//eSHADERID_TESSELLATION,					//Tessellationテスト

		eSHADERID_END,
	};

	enum E_SHADERTYPE
	{
		eSHADERTYPE_NONE,
		eSHADERTYPE_VERTEX,		//頂点シェーダ
		eSHADERTYPE_PIXEL,		//ピクセルシェーダ
		eSHADERTYPE_GEOMETRY,	//ジオメトリシェーダ
		eSHADERTYPE_HAL,		//ハルシェーダ
		eSHADERTYPE_DOMAIN,		//ドメインシェーダ
		eSHADERTYPE_END,
	};

public:
	//@brief	初期化
	bool Init();	

	//@brief	指定シェーダー獲得
	CShaderBase* GetShader( const E_SHADERID eShaderID );

	//@brief	指定シェーダー追加
	bool AddShader( const E_SHADERID eShaderID );

	//@brief	指定シェーダー削除
	bool RemoveShader( const E_SHADERID eShaderID );

private:
	//@brief	破棄
	void Term();

private:
	CSafeArray< CShaderBase*, eSHADERID_END > m_aShader; //!< シェーダー
};

//EOF
