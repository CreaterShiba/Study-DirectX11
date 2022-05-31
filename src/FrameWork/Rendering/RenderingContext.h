#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingContext.h
	@brief		共通レンダリング設定
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Rendering/Shader/ShaderBase.h"

enum E_RENDER_PRIORITY
{
	eREN_PRI_DEFAULT = 500,
	eREN_PRI_MODEL = 1000,
	eREN_PRI_TEXT = 5000,
	eREN_PRI_MAX
};

/*==========================================================================*/
/*!
	@class	CRenderingContext
	@brief	共通レンダリング設定
*/
/*==========================================================================*/
class CRenderingContext
{
public:
	// @brief コンストラクタ
	CRenderingContext();

	// @brief デストラクタ
	~CRenderingContext();

	//@brief シェーダセット
	inline void SetShader( CShaderBase* pcShader );
	inline void SetShader( const CShaderManager::E_SHADERID eShaderID );

	// @brief オーバーライドシェーダセット
	inline void SetOverShader( CShaderBase* pcShader );

	//@brief シェーダ獲得
	inline CShaderBase* GetShader() const;

	//@brief 各種レンダリング設定
	void SetContextBlendDesc(const D3D11_BLEND_DESC& desc);

	void SetContextSamplerState(const D3D11_SAMPLER_DESC& desc);

	void SetContextRasterizeState(const D3D11_RASTERIZER_DESC& desc);

	void SetContextDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc);

	void ApplyRenderDesc();

private:

	//!< 使用するシェーダ
	CShaderBase* m_pcShader;

	//!< オーバライドシェーダ
	CShaderBase* m_pcOverShader;

	//レンダリングステートとか

	//!< ブレンド設定
	bool m_bChangeBlendDesc;
	D3D11_BLEND_DESC			m_eBlendDesc;			

	//!< サンプラーステート
	bool m_bChangeSamplerDesc;
	D3D11_SAMPLER_DESC			m_eSamplerState;		

	//!< ラスタライズステート
	bool m_bChangeRasterizeDesc;
	D3D11_RASTERIZER_DESC		m_eRasterizeState;		

	//!< デプスステンシルステート
	bool m_bChangeDepthStencilDesc;
	D3D11_DEPTH_STENCIL_DESC	m_eDepthStencilState;	

	//!< シザリング矩形
};

//---------------------------------------------------------------------------
/*!
	@brief	シェーダセット
	@param[in]	pcShader シェーダ
*/
//---------------------------------------------------------------------------
void CRenderingContext::SetShader( CShaderBase* pcShader )
{
	m_pcShader = pcShader;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダセット
	@param[in]	eShaderID シェーダID
*/
//---------------------------------------------------------------------------
void CRenderingContext::SetShader( const CShaderManager::E_SHADERID eShaderID )
{
	CShaderManager* pcShaderM = CShaderManager::GetInstance();
	NS_ASSERT( pcShaderM );
	m_pcShader = pcShaderM->GetShader( eShaderID );
}

//---------------------------------------------------------------------------
/*!
	@brief	オーバーライドシェーダセット
	@param[in]	pcShader シェーダ
*/
//---------------------------------------------------------------------------
void CRenderingContext::SetOverShader( CShaderBase* pcShader )
{
	m_pcOverShader = pcShader;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ獲得
*/
//---------------------------------------------------------------------------
CShaderBase* CRenderingContext::GetShader() const
{
	if( m_pcOverShader )
	{
		return m_pcOverShader;
	}
	else
	{
		return m_pcShader;
	}
}

//EOF
