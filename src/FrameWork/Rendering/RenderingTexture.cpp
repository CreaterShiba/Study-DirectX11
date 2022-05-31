/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingTexture.cpp
	@brief		レンダリングテクスチャ
	@author		shiba
	@date		2012/3/3 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "RenderingTexture.h"

#include "FrameWork/DXUtil/UtilDX11.h"
#include "FrameWork/Utility/UtilMacro.h"

#include "Sprite.h"

//#define USE_PIX

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CRenderingTexture::CRenderingTexture() 
: m_pRenderTargetView( nullptr )
, m_pDepthStencilView( nullptr )
#if defined( _DEBUG )
, m_pcSprite( nullptr )
#endif
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CRenderingTexture::~CRenderingTexture()
{
#if defined( _DEBUG )
	SAFE_DELETE_TASK( m_pcSprite );
#endif
	SAFE_RELEASE( m_pDepthStencilView );
	SAFE_RELEASE( m_pRenderTargetView );
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CRenderingTexture::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		レンダリングターゲットの作成
	@param[in]	D3D11_TEXTURE2D_DESC	テクスチャ設定
*/
//---------------------------------------------------------------------------
bool CRenderingTexture::CreateRenderingTarget( const D3D11_TEXTURE2D_DESC& desc )
{	
	ID3D11Texture2D* pRenderTexture = nullptr;

	DXMng* const pcDirectX = DXMng::GetInstance();
	NS_ASSERT( pcDirectX );

	HRESULT hr;
	//テクスチャの作成をする。
	hr = pcDirectX->GetDevice().CreateTexture2D( &desc, nullptr, &pRenderTexture );
	if( FAILED( hr ) )
	{
		return false;
	}

	// シェーダーリソースビューの作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	ZeroMemory( &srDesc, sizeof(srDesc) );
 
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* pShaderView = nullptr;
	hr = pcDirectX->GetDevice().CreateShaderResourceView( pRenderTexture, &srDesc, &pShaderView );
	if( FAILED( hr ) )
	{
		return false;
	}

	m_pTextureReVi = pShaderView;

	//ターゲットビューの作成
	hr = pcDirectX->GetDevice().CreateRenderTargetView( pRenderTexture, nullptr, &m_pRenderTargetView );
	if( FAILED( hr ) )
	{
		return false;
	}

	SAFE_RELEASE( pRenderTexture ); 

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	スワップチェインから獲得したレンダリングターゲットの作成
*/
//---------------------------------------------------------------------------
bool CRenderingTexture::CreateRenderTargetForSwapchain()
{	
	ID3D11Texture2D* pRenderTexture = nullptr;

	DXMng* const pcDirectX = DXMng::GetInstance();
	NS_ASSERT( pcDirectX );

	//バッファはスワップチェインから取得してくる。
	if( FAILED(pcDirectX->GetSwapChain().GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&pRenderTexture ) ) )
	{
		return false;
	}
#if !defined( USE_PIX )
	// 描画ターゲットビューをバックバッファから作成する
	// @note ここはずすとpix動く
	if( FAILED( pcDirectX->GetDevice().CreateRenderTargetView( pRenderTexture, nullptr, &m_pRenderTargetView ) ) )
	{
		return false;
	}
#endif
	SAFE_RELEASE( pRenderTexture ); 

	return true;
}

#if defined( _DEBUG )

//---------------------------------------------------------------------------
/*!
	@brief		デバッグ用スプライトの作成
	@param[in]	unSizeX	テクスチャサイズX
	@param[in]	unSizeY	テクスチャサイズY
*/
//---------------------------------------------------------------------------
bool CRenderingTexture::CreateDebugSprite( const NSVector2& pos, const NSVector2& size )
{
	NS_ASSERT( !m_pcSprite );
	CLASS_NEW( m_pcSprite, CSprite() );

	m_pcSprite->SetPos( NSVector3( pos.x, pos.y, 0.0f ) );
	m_pcSprite->SetScale( NSVector3( size.x, size.y, 0.0f) );
	m_pcSprite->SetTexture( m_pTextureReVi );

	//m_pTextureReVi->AddRef();	//これいるの？

	CRenderingContext cContext;
	cContext.SetShader( CShaderManager::eSHADERID_SPRITE);
	m_pcSprite->SetRenderContext( cContext );
	m_pcSprite->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_SPRITE );

	//SAFE_RELEASE( m_pTextureReVi );

	return true;
}

#endif

//EOF