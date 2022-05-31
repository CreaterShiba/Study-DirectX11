
#pragma once

#include <d3d11.h>
#include <vector>
#include <dxgi.h>

#pragma comment ( lib, "dxgi.lib" )
#pragma comment ( lib, "d3d11.lib" )

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DXMng.h
	@brief		DirectX管理
	@author		shiba
	@date		2012/2/29 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class DXMng
{
	NS_SINGLETON(DXMng);
	DXMng();
	~DXMng();

private:
	//@brief 初期化
	bool Init();

public:
	enum EWINDOWMODE
	{
		eWINDOWMODE_SCREEN,
		eWINDOWMODE_FULL,
		eWINDOWMODE_MAX
	};

	//DirectXの初期化
	bool InitDirectX(const HWND hWnd, const EWINDOWMODE eWindowMode);

	inline ID3D11Device&		 GetDevice() const;
	inline ID3D11DeviceContext&	 GetConDevice() const;
	inline IDXGISwapChain&		GetSwapChain() const;
	inline DXGI_SWAP_CHAIN_DESC GetSwapChainDesc() const;
	inline D3D_FEATURE_LEVEL GetFeatureLevel() const;

//private:
	//Utilityから通す
	//ブレンドステート設定
	void SetBlendState(const D3D11_BLEND_DESC& desc);

	//サンプラステート設定
	void SetSamplerState(const D3D11_SAMPLER_DESC& desc);

	//ラスタライザステート設定
	void SetRasterizerState(const D3D11_RASTERIZER_DESC& desc);

	//深度ステンシル設定
	void SetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc);

	void SetAllDefaultState();

	// 各種デフォルト設定
	D3D11_BLEND_DESC GetDefaultBlendDesc() const;
	D3D11_SAMPLER_DESC GetDefaultSamplerDesc() const;
	D3D11_RASTERIZER_DESC GetDefaultRasterizerDesc() const;
	D3D11_DEPTH_STENCIL_DESC GetDefaultDepthStencilDesc() const;

private:
	ID3D11Device* mpDevice;					//!< デバイス
	ID3D11DeviceContext* mpConDevice;		//!< デバイスコンテキスト
	IDXGISwapChain* mpSwapChain;			//!< スワップチェイン
	IDXGIFactory* mpGIFactory;
	IDXGIAdapter* mpGIAdapter;

	DXGI_SWAP_CHAIN_DESC mGISwapChain;		//!< スワップチェイン設定
	D3D_FEATURE_LEVEL mFeatureLevel;		//!< 機能レベル

	//レンダリング設定
	std::vector<DXGI_SAMPLE_DESC>	mMultiSampleDescList;
	std::vector<DXGI_MODE_DESC>		mDisplayModeList;

	//各種ステート設定
	ID3D11SamplerState* m_pSamplerState;
	ID3D11BlendState* m_pBlendState;
	ID3D11RasterizerState* m_pRasterizerState;
	ID3D11DepthStencilState* m_pDepthStencil;
};

/*==========================================================================*/
/*		inline																*/
/*==========================================================================*/

//デバイス取得関数
inline ID3D11Device& DXMng::GetDevice() const
{
	return *mpDevice;
}

//コンテキスト取得
inline ID3D11DeviceContext& DXMng::GetConDevice() const
{
	return *mpConDevice;
}

//スワップチェイン取得関数
inline IDXGISwapChain& DXMng::GetSwapChain() const
{
	return *mpSwapChain;
}

//スワップチェイン設定取得関数
inline DXGI_SWAP_CHAIN_DESC DXMng::GetSwapChainDesc() const
{
	return mGISwapChain;
}

//機能レベルを返す
inline D3D_FEATURE_LEVEL DXMng::GetFeatureLevel() const
{
	return mFeatureLevel;
}

//EOF
