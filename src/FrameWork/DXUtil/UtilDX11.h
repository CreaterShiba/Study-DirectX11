/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilDX11.h
	@brief		DirectX11系ユーティリティクラス
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#pragma once

#include <d3d11.h>
#include "FrameWork/DXUtil/DXMng.h"

class CResourceTexture;

class CUtilDX11
{
public:
	//friend class DXMng;

	CUtilDX11();
	~CUtilDX11();

	// @brief 頂点バッファの作成
	static void CreateVertexBuffer( ID3D11Buffer** pBuffer, const UINT unBufferSize, void* pResouce );
	
	// @brief インデックスバッファの生成
	static void CreateIndexBuffer( ID3D11Buffer** pBuffer, const UINT unBufferSize, void* pResouce );
	
	// @brief RWバッファの作成
	static void CreateRWBuffer( ID3D11Buffer** pBuffer, const UINT unBufferSize, const bool bStructured );

	// @brief 定数バッファの生成(バッファサイズは4096*16以下であり、16の倍数である必要が有る)
	static void CreateConstantBuffer( const u32 Size, ID3D11Buffer** ppBuffer );

	// @brief マップ
	static HRESULT Map( ID3D11Resource* const pResource, const D3D11_MAP eMapType, D3D11_MAPPED_SUBRESOURCE* const pMappedResource );
	
	// @brief アンマップ
	static void UnMap( ID3D11Resource* const pResource );

	// @brief サンプラステート設定
	static void SetSamplerState( const D3D11_SAMPLER_DESC& desc );
	
	// @brief ブレンドステート設定
	static void SetBlendState( const D3D11_BLEND_DESC& desc );
	
	// @brief ブレンドステートの色々サンプル取得
	static D3D11_RENDER_TARGET_BLEND_DESC GetOffBlendDesc();
	static D3D11_RENDER_TARGET_BLEND_DESC GetAlignmentBlendDesc();
	static D3D11_RENDER_TARGET_BLEND_DESC GetAddBlendDesc();
	static D3D11_RENDER_TARGET_BLEND_DESC GetSubtractBlendDesc();
	static D3D11_RENDER_TARGET_BLEND_DESC GetMultipleBlendDesc();

	// @brief ラスタライザステート設定
	static void SetRasterizerState( const D3D11_RASTERIZER_DESC& desc );

	// @brief 深度ステンシル設定
	static void SetDepthStencilState( const D3D11_DEPTH_STENCIL_DESC& desc );

	// @brief デフォルト設定の設定
	static void SetAllDefaultState();

	// @brief 標準的なレンダリングターゲット用テクスチャ設定獲得
	static D3D11_TEXTURE2D_DESC GetDescRenderTarget();

	// @brief ピクセルシェーダへのテクスチャ設定
	static void PSSetShaderResources( const u32 unStartSlot, const u32 unViews, const CResourceTexture* pTex );

	// @brief Device取得
	static const ID3D11Device& GetDevice();
	// @brief ContextDevice取得
	static ID3D11DeviceContext& GetConDevice();
};

//EOF
