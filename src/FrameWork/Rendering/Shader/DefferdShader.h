#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DefferdShader.h
	@brief		DefferdShading
	@author		shiba
	@date		2013/06/18 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"
#include "FrameWork/Rendering/RenderingManager.h"

/*==========================================================================*/
/*!
	@class	CDefferdShader
	@brief	DefferdShading
*/
/*==========================================================================*/
class CDefferdShader : public CShaderBase
{
public:
	CDefferdShader();
	~CDefferdShader();

public:
	//@brief シェーダロード
	virtual HRESULT LoadShader();
	//@brief シェーダを有効化
	virtual bool ShaderOn();

	//@brief 各種値設定
	void setLightPos( const s32 idx, const NSVector3& lightPos );
	void setLightCol(const s32 idx, const SColor& lightCol);
	void SetLightPow(const s32 idx, const f32 fPow);
	void setCamPos( const NSVector4& camPos );
private:

	struct cbLightObject 
	{
		NSVector4 pos[ CRenderingManager::eLIGHT_MAX ];	//ライト座標
		NSVector4 col[ CRenderingManager::eLIGHT_MAX ];	//ライトカラー(aはライト強度)
		NSVector4 campos;			//カメラ座標
	};

	const static s32 CONBUFFERNUM = 1;	//!< バッファ数
	ID3D11Buffer* mpLightBuffer;		//!< バッファ

	cbLightObject m_LightObj;

private: 
	void WriteLightInfo();
};

//EOF