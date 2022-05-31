/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingContext.cpp
	@brief		レンダリングコンテキスト
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "RenderingContext.h"
#include "FrameWork/Rendering/Shader/ShaderBase.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CRenderingContext::CRenderingContext() 
: m_pcShader( nullptr )
, m_pcOverShader( nullptr )
, m_bChangeBlendDesc(false)
, m_bChangeSamplerDesc(false)
, m_bChangeRasterizeDesc(false)
, m_bChangeDepthStencilDesc(false)
{
	ZeroMemory( &m_eBlendDesc, sizeof( D3D11_BLEND_DESC ) );  
	ZeroMemory( &m_eSamplerState, sizeof( D3D11_SAMPLER_DESC ) );  
	ZeroMemory( &m_eRasterizeState, sizeof( D3D11_RASTERIZER_DESC ) );  
	ZeroMemory( &m_eDepthStencilState, sizeof( D3D11_DEPTH_STENCIL_DESC ) );  
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CRenderingContext::~CRenderingContext()
{
	m_pcOverShader = nullptr;
	m_pcShader = nullptr;
}


//---------------------------------------------------------------------------
/*!
	@brief	各種設定
*/
//---------------------------------------------------------------------------
void CRenderingContext::SetContextBlendDesc( const D3D11_BLEND_DESC& desc )
{
	m_bChangeBlendDesc = true;
	m_eBlendDesc = desc;
}

void CRenderingContext::SetContextSamplerState(const D3D11_SAMPLER_DESC& desc)
{
	m_bChangeSamplerDesc = true;
	m_eSamplerState = desc;
}

void CRenderingContext::SetContextRasterizeState(const D3D11_RASTERIZER_DESC& desc)
{
	m_bChangeRasterizeDesc = true;
	m_eRasterizeState = desc;
}

void CRenderingContext::SetContextDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc)
{
	m_bChangeDepthStencilDesc = true;
	m_eDepthStencilState = desc;
}

void CRenderingContext::ApplyRenderDesc()
{
	if (m_bChangeBlendDesc)
	{
		CUtilDX11::SetBlendState(m_eBlendDesc);
	}

	if (m_bChangeSamplerDesc)
	{
		CUtilDX11::SetSamplerState(m_eSamplerState);
	}

	if (m_bChangeRasterizeDesc)
	{
		CUtilDX11::SetRasterizerState(m_eRasterizeState);
	}

	if (m_bChangeDepthStencilDesc)
	{
		CUtilDX11::SetDepthStencilState(m_eDepthStencilState);
	}
}

//EOF
