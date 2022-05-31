/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilDX11.cpp
	@brief		DirectX11系ユーティリティクラス
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "UtilDX11.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CUtilDX11::CUtilDX11()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CUtilDX11::~CUtilDX11()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	頂点バッファ生成
*/
//---------------------------------------------------------------------------
void CUtilDX11::CreateVertexBuffer( ID3D11Buffer** pBuffer, const UINT unBufferSize, void* pResouce )
{
	//--------------------------------------------------------------------
	// 頂点バッファ情報設定
	//--------------------------------------------------------------------
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.ByteWidth      = unBufferSize;					// サイズ
	BufferDesc.Usage          = D3D11_USAGE_DEFAULT;			// 通常バッファ（一度作ったら変更不可）
	BufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;		// 頂点バッファとしてバインド
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags      = 0;
	BufferDesc.StructureByteStride = 0;

	//--------------------------------------------------------------------
	// サブリソース
	//--------------------------------------------------------------------
	D3D11_SUBRESOURCE_DATA SubVertexInfo;
	SubVertexInfo.pSysMem		   = pResouce;
	SubVertexInfo.SysMemPitch	   = 0;
	SubVertexInfo.SysMemSlicePitch = 0;

	//--------------------------------------------------------------------
	// バッファを実際に生成する。
	//--------------------------------------------------------------------
	DXMng* pDX = DXMng::GetInstance();
	NS_ASSERT( pDX );
	pDX->GetDevice().CreateBuffer( &BufferDesc, &SubVertexInfo, pBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	インデックスバッファ生成
*/
//---------------------------------------------------------------------------
void CUtilDX11::CreateIndexBuffer( ID3D11Buffer** pBuffer, const UINT unBufferSize, void* pResouce )
{
	//--------------------------------------------------------------------
	// インデックスバッファ情報設定
	//--------------------------------------------------------------------
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.ByteWidth      = unBufferSize;					// サイズ
	BufferDesc.Usage          = D3D11_USAGE_DEFAULT;			// 通常バッファ（一度作ったら変更不可）
	BufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;		// インデックスバッファとしてバインド
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags      = 0;
	BufferDesc.StructureByteStride = 0;

	//--------------------------------------------------------------------
	// サブリソース
	//--------------------------------------------------------------------
	D3D11_SUBRESOURCE_DATA SubVertexInfo;
	SubVertexInfo.pSysMem		   = pResouce;
	SubVertexInfo.SysMemPitch	   = 0;
	SubVertexInfo.SysMemSlicePitch = 0;

	//--------------------------------------------------------------------
	// バッファを実際に生成する。
	//--------------------------------------------------------------------
	DXMng* pDX = DXMng::GetInstance();
	NS_ASSERT( pDX );
	pDX->GetDevice().CreateBuffer( &BufferDesc, &SubVertexInfo, pBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	RWバッファ作成
*/
//---------------------------------------------------------------------------
void CUtilDX11::CreateRWBuffer( ID3D11Buffer** pBuffer, const UINT unBufferSize, const bool bStructured )
{
	//--------------------------------------------------------------------
	// RWバッファ情報設定
	//--------------------------------------------------------------------
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory( &BufferDesc, sizeof( BufferDesc ) );
	BufferDesc.ByteWidth = unBufferSize;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//UsageにD3D11_USAGE_STAGINGを指定すればCPUからのアクセスが可能になる
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	if( bStructured )
	{
		// らしい。
		//BufferDesc.MiscFlags  = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // RWStructuredBufferを作成する場合はこっち
		//BufferDesc.StructureByteStride = sizeof(Particle);			// RWStructuredBufferなら構造体のサイズを指定する
	}

	//--------------------------------------------------------------------
	// バッファを実際に生成する。
	//--------------------------------------------------------------------
	DXMng* pDX = DXMng::GetInstance();
	NS_ASSERT( pDX );
	pDX->GetDevice().CreateBuffer( &BufferDesc, nullptr, pBuffer );

}

//---------------------------------------------------------------------------
/*!
	@brief	定数バッファの作成		
*/
//---------------------------------------------------------------------------
void CUtilDX11::CreateConstantBuffer( const u32 Size, ID3D11Buffer** ppBuffer )
{
	D3D11_BUFFER_DESC ConstDesc;
	//D3D11_USAGE_DYNAMICだとUpdateSubresourceが使えない
	ConstDesc.Usage				= D3D11_USAGE_DYNAMIC;		  //動的に使用する
	ConstDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER; //定数バッファ
	ConstDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;	  //CPUから書き込みできる
	ConstDesc.ByteWidth			= Size;
	ConstDesc.MiscFlags			= 0;

	//-----------------------------------------------------------------------------
	// バッファ作成
	//-----------------------------------------------------------------------------
	DXMng* pDX = DXMng::GetInstance();
	NS_ASSERT( pDX );

	pDX->GetDevice().CreateBuffer( &ConstDesc, nullptr, ppBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	リソースマップ
*/
//---------------------------------------------------------------------------
HRESULT CUtilDX11::Map( ID3D11Resource* const pResource, const D3D11_MAP eMapType, D3D11_MAPPED_SUBRESOURCE* const pMappedResource )
{
	if( pResource )
	{
		DXMng* pDX = DXMng::GetInstance();
		NS_ASSERT( pDX );
		return pDX->GetConDevice().Map( pResource, D3D11CalcSubresource(0,0,1), eMapType, 0, pMappedResource );
	}
	return S_FALSE;
}

//---------------------------------------------------------------------------
/*!
	@brief	リソースアンマップ
*/
//---------------------------------------------------------------------------
void CUtilDX11::UnMap( ID3D11Resource* const pResource )
{
	if( pResource )
	{
		DXMng* pDX = DXMng::GetInstance();
		NS_ASSERT( pDX );
		pDX->GetConDevice().Unmap( pResource, D3D11CalcSubresource(0,0,1));
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		サンプラステート設定
	@param[in]	desc	サンプラステート
*/
//---------------------------------------------------------------------------
void CUtilDX11::SetSamplerState( const D3D11_SAMPLER_DESC& desc )
{
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT( pcDXM );

	pcDXM->SetSamplerState(desc);
}

//---------------------------------------------------------------------------
/*!
	@brief		ブレンドステート設定設定
	@param[in]	desc	ブレンドステート
*/
//---------------------------------------------------------------------------
void CUtilDX11::SetBlendState( const D3D11_BLEND_DESC& desc )
{
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT( pcDXM );

	pcDXM->SetBlendState(desc);
}

//---------------------------------------------------------------------------
/*!
	@brief		ブレンド ステートを無効にするための設定を取得する
*/
//---------------------------------------------------------------------------
D3D11_RENDER_TARGET_BLEND_DESC CUtilDX11::GetOffBlendDesc()
{
	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

	RenderTarget.BlendEnable = FALSE;
	RenderTarget.SrcBlend = D3D11_BLEND_ONE;
	RenderTarget.DestBlend = D3D11_BLEND_ZERO;
	RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return RenderTarget;
}

//---------------------------------------------------------------------------
/*!
	@brief		線形合成用ブレンド ステートのためのを設定を取得する
*/
//---------------------------------------------------------------------------
D3D11_RENDER_TARGET_BLEND_DESC CUtilDX11::GetAlignmentBlendDesc()
{
	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

	RenderTarget.BlendEnable = TRUE;
	RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	RenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return RenderTarget;
}

//---------------------------------------------------------------------------
/*!
	@brief		加算合成用ブレンド ステートのためのを設定を取得する
*/
//---------------------------------------------------------------------------
D3D11_RENDER_TARGET_BLEND_DESC CUtilDX11::GetAddBlendDesc()
{
	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

	RenderTarget.BlendEnable = TRUE;
	RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	RenderTarget.DestBlend = D3D11_BLEND_ONE;
	RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return RenderTarget;
}

//---------------------------------------------------------------------------
/*!
	@brief		減算合成用ブレンド ステートのためのを設定を取得する
*/
//---------------------------------------------------------------------------
D3D11_RENDER_TARGET_BLEND_DESC CUtilDX11::GetSubtractBlendDesc()
{
	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

	RenderTarget.BlendEnable = TRUE;
	RenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	RenderTarget.DestBlend = D3D11_BLEND_ONE;
	RenderTarget.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return RenderTarget;
}

//---------------------------------------------------------------------------
/*!
	@brief		積算合成用ブレンド ステートのためのを設定を取得する
*/
//---------------------------------------------------------------------------
D3D11_RENDER_TARGET_BLEND_DESC CUtilDX11::GetMultipleBlendDesc()
{
	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget;

	RenderTarget.BlendEnable = TRUE;
	RenderTarget.SrcBlend = D3D11_BLEND_ZERO;
	RenderTarget.DestBlend = D3D11_BLEND_SRC_COLOR;
	RenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
	RenderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
	RenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
	RenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	RenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return RenderTarget;
}

//---------------------------------------------------------------------------
/*!
	@brief		ラスタライザステート設定
	@param[in]	desc	ラスタライザステート
*/
//---------------------------------------------------------------------------
void CUtilDX11::SetRasterizerState( const D3D11_RASTERIZER_DESC& desc )
{
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT( pcDXM );

	pcDXM->SetRasterizerState(desc);
}

//---------------------------------------------------------------------------
/*!
	@brief		深度ステンシル設定
	@param[in]	desc	深度ステンシルステート
*/
//---------------------------------------------------------------------------
void CUtilDX11::SetDepthStencilState( const D3D11_DEPTH_STENCIL_DESC& desc )
{
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT( pcDXM );

	pcDXM->SetDepthStencilState(desc);
}

void CUtilDX11::SetAllDefaultState()
{
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT(pcDXM);

	pcDXM->SetAllDefaultState();
}

//---------------------------------------------------------------------------
/*!
	@brief	標準的なレンダリングターゲット用テクスチャ設定獲得
*/
//---------------------------------------------------------------------------
D3D11_TEXTURE2D_DESC CUtilDX11::GetDescRenderTarget()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory( &texDesc, sizeof( texDesc ) );
	texDesc.Width = WINDOW_WIDTH;
	texDesc.Height = WINDOW_HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT( pcDXM );
	texDesc.SampleDesc		= pcDXM->GetSwapChainDesc().SampleDesc;

	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	return texDesc;
}

#if false
//---------------------------------------------------------------------------
/*!
	@brief	ピクセルシェーダへのテクスチャ設定
	@param[in]	unStartSlot	スタートスロット数
	@param[in]	unViews	設定数
	@param[in]	pTex	テクスチャ
*/
//---------------------------------------------------------------------------
void CUtilDX11::PSSetShaderResources( const u32 unStartSlot, const u32 unViews, const CResourceTexture* pTex )
{
	if( pTex )
	{
		DXMng* pcDXM = DXMng::GetInstance();
		NS_ASSERT( pcDXM );

		ID3D11ShaderResourceView* pcView = pTex->GetTextureRevi();
		pcDXM->GetConDevice().PSSetShaderResources( unStartSlot, unViews, &pcView);
	}
}
#endif

//---------------------------------------------------------------------------
/*!
	@brief	DirectXデバイス取得
*/
//---------------------------------------------------------------------------
const ID3D11Device& CUtilDX11::GetDevice()
{
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT( pcDXM );

	return pcDXM->GetDevice();
}

//---------------------------------------------------------------------------
/*!
	@brief	DirectXコンテキストデバイス取得
*/
//---------------------------------------------------------------------------
ID3D11DeviceContext& CUtilDX11::GetConDevice()
{
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT( pcDXM );

	return pcDXM->GetConDevice();
}

//EOF