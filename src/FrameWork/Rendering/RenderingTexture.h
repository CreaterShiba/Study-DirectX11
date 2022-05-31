#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingTexture.h
	@brief		レンダリングテクスチャ
	@author		shiba
	@date		2012/3/3 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Asset/AssetTexture.h"

/*==========================================================================*/
/*!
	@class	CRenderingTexture
	@brief	レンダリングターゲット
*/
/*==========================================================================*/
class CRenderingTexture : public CAssetTexture
{
public:
	CRenderingTexture();
	~CRenderingTexture();

	//初期化
	bool Init();

	//@brief レンダリングターゲットの作成
	bool CreateRenderingTarget( const D3D11_TEXTURE2D_DESC& desc );

	//@brief スワップチェインから、レンダリングターゲットを取得して作成
	bool CreateRenderTargetForSwapchain();

	// @brief レンダリングターゲットビュー設定
	inline void SetRenderTargetView( ID3D11RenderTargetView* pPtr );

	// @brief レンダリングターゲットビュー獲得
	inline ID3D11RenderTargetView* GetRenderTargetView() const;

#if defined( _DEBUG )
	//@brief デバッグ用スプライトの作成
	bool CreateDebugSprite( const NSVector2& pos, const NSVector2& size );
#endif

private:
	//!< レンダリングターゲット
	ID3D11RenderTargetView* m_pRenderTargetView;	

	//!< 深度ステンシル
	ID3D11DepthStencilView* m_pDepthStencilView;	

#if defined( _DEBUG )
	class CSprite* m_pcSprite;
#endif

};

//---------------------------------------------------------------------------
/*!
	@brief	レンダリングターゲットビューセット
*/
//---------------------------------------------------------------------------
inline void CRenderingTexture::SetRenderTargetView( ID3D11RenderTargetView* pPtr )
{
	m_pRenderTargetView = pPtr;
}

//---------------------------------------------------------------------------
/*!
	@brief	レンダリングターゲットビュー獲得
*/
//---------------------------------------------------------------------------
inline ID3D11RenderTargetView* CRenderingTexture::GetRenderTargetView() const
{
	return m_pRenderTargetView;
}

//EOF
