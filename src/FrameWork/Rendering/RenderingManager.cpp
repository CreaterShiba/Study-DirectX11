/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingManager.cpp
	@brief		描画マネージャー
	@author		shiba
	@date		2012/9/29 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "RenderingManager.h"
#include "Render.h"
#include "RenderingStage.h"
#include "RenderingTexture.h"
#include "light.h"

#include "FrameWork/Camera/CameraBase.h"
#include "FrameWork/Camera/CameraManager.h"

NS_SINGLETON_INSTANCE_DEFINE( CRenderingManager );

struct SStageContext
{
	SStageContext( bool bClear, bool bDepth, bool bStencil) :
	bClearTarget( bClear ),
	bClearDepth( bDepth ),
	bClearStencil( bStencil )
	{}
	bool bClearTarget;
	bool bClearDepth;
	bool bClearStencil;
};

//一応ここでやる
//ここらへんの設定項目は外だしする
SStageContext bCreateTartget[ CRenderingManager::E_RENDER_STAGE_MAX ] =
{
	SStageContext(true, true, true),		//E_RENDER_STAGE_WRITEZ,		
	SStageContext(true, true, true),		//E_RENDER_STAGE_MRT_POS_NOM_DIF,	
	SStageContext(true, true, true),		//E_RENDER_STAGE_MAIN,		
	SStageContext(true, true, true),		//E_RENDER_BRIGHT_GET,		
	SStageContext(true, true, true),		//E_RENDER_BRIGHT_BLUR,	
	SStageContext(true, true, true),		//E_RENDER_STAGE_SPRITE,	
	SStageContext(true, true, true)			//E_RENDER_STAGE_POSTEFFECT	
};

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CRenderingManager::CRenderingManager() 
: m_paRenderStage()
, m_pDepthStencilView( nullptr )
, m_bDrawFlag( false )
{
}

//---------------------------------------------------------------------------
/*!
	@brief		デストラクタ
*/
//---------------------------------------------------------------------------
CRenderingManager::~CRenderingManager()
{
	SAFE_RELEASE( m_pDepthStencilView );

	for ( s32 i = 0; i < E_RENDER_STAGE_MAX; i++)
	{
		SAFE_DELETE( m_paRenderStage.Get( i ) );
		m_paRenderStage.Set( i, nullptr );
	}

	for (s32 i = 0; i < eLIGHT_MAX; i++)
	{
		SAFE_DELETE_TASK(m_paLightData.Get(i));
		m_paLightData.Set(i, nullptr);
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		初期化
*/
//---------------------------------------------------------------------------
bool CRenderingManager::Init()
{
	DXMng* const pcDirectX = DXMng::GetInstance();
	NS_ASSERT( pcDirectX );

	//-----------------------------------------------------------------------------
	// レンダリングステージの作成
	//-----------------------------------------------------------------------------
	for ( s32 i = 0; i < E_RENDER_STAGE_MAX; i++)
	{
		CRenderingStage* pStage = nullptr;
		CLASS_NEW( pStage, CRenderingStage( static_cast<E_RENDER_STAGE>( i ) ) );
		m_paRenderStage.Set( i, pStage );

		if (i == E_RENDER_STAGE_MRT_POS_NOM_DIF)
		{
			for (s32 j = 0; j < 3; j++)
			{
				D3D11_TEXTURE2D_DESC texDesc = CUtilDX11::GetDescRenderTarget();

				if (j == 0)
					//カラー
					texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				else
					//法線、ワールド座標テクスチャ
					texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

				pStage->CreateRenderingTarget(texDesc);
			}
		}
#if !defined(WRITEZ_HARDWARE_TEST)
		else if (i == E_RENDER_STAGE_WRITEZ)
		{
			D3D11_TEXTURE2D_DESC texDesc = CUtilDX11::GetDescRenderTarget();
			//@note https://wgld.org/d/webgl/w051.html
			texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	//32ビットのつかうけど、8*4で圧縮して使ったほうがよさげ
			pStage->CreateRenderingTarget(texDesc);
		}
#else
		//else if (i == E_RENDER_STAGE_WRITEZ)
		//{
		//	//なにもしない
		//}
#endif
		else if (i == E_RENDER_STAGE_POSTEFFECT)	//最後に描画するやつ
		{
			pStage->CreateRenderTargetForSwapchain();
		}
		else
		{
			//レンダリング対象のターゲットを生成
			D3D11_TEXTURE2D_DESC texDesc = CUtilDX11::GetDescRenderTarget();
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			pStage->CreateRenderingTarget(texDesc);
		}

		pStage->m_bClearRenderTarget = bCreateTartget[i].bClearTarget;
		pStage->m_bClearDepth = bCreateTartget[i].bClearDepth;
		pStage->m_bClearStencil = bCreateTartget[i].bClearStencil;
	}

#if defined( _DEBUG )
	CRenderingStage* pcStage = m_paRenderStage.Get(E_RENDER_STAGE_MRT_POS_NOM_DIF);
	for (u16 i = 0; i < 3; i++)
	{
		pcStage->CreateDebugSprite(NSVector2(64.0f + 128.0f*i, 64.0f), NSVector2(128.0f), i);
	}
	pcStage = m_paRenderStage.Get(E_RENDER_STAGE_WRITEZ);
	pcStage->CreateDebugSprite(NSVector2(64.0f + 128.0f * 3, 64.0f), NSVector2(128.0f), 0);

	pcStage = m_paRenderStage.Get(E_RENDER_GET_BRIGHT);
	pcStage->CreateDebugSprite(NSVector2(64.0f + 128.0f * 4, 64.0f), NSVector2(128.0f), 0);

	pcStage = m_paRenderStage.Get(E_RENDER_BLUR_BRIGHT);
	pcStage->CreateDebugSprite(NSVector2(64.0f + 128.0f * 5, 64.0f), NSVector2(128.0f), 0);
#endif

	//--------------------------------------------------------------------
	// 深度ステンシルテクスチャの生成
	D3D11_TEXTURE2D_DESC TexDesc;
	ZeroMemory( &TexDesc, sizeof(TexDesc) );
	TexDesc.Width			= pcDirectX->GetSwapChainDesc().BufferDesc.Width;
	TexDesc.Height			= pcDirectX->GetSwapChainDesc().BufferDesc.Height;
	TexDesc.MipLevels		= 1;
	TexDesc.ArraySize		= 1;
	TexDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;
	//texDesc.SampleDesc.Count = 1;
	//TexDesc.SampleDesc.Quality = 0;
	TexDesc.SampleDesc		= pcDirectX->GetSwapChainDesc().SampleDesc;
	TexDesc.Usage			= D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	TexDesc.CPUAccessFlags	= 0;
	TexDesc.MiscFlags		= 0;

	//テクスチャ作成
	ID3D11Texture2D* pTexDepth = nullptr;
	pcDirectX->GetDevice().CreateTexture2D( &TexDesc, nullptr, &pTexDepth );

	//--------------------------------------------------------------------
	// 深度ステンシルビューの生成
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc; //深度ステンシルバッファの設定
	ZeroMemory( &DepthViewDesc, sizeof(DepthViewDesc) );
	//DepthViewDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthViewDesc.Format		= TexDesc.Format;
	//DepthViewDesc.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2DMS;
	DepthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//DepthViewDesc.Flags				 = 0;
	DepthViewDesc.Texture2D.MipSlice = 0;
	pcDirectX->GetDevice().CreateDepthStencilView( pTexDepth, &DepthViewDesc, &m_pDepthStencilView );

	SAFE_RELEASE( pTexDepth );

	//-----------------------------------------------------------------------------
	// ライトの作成
	//-----------------------------------------------------------------------------
	for (s32 i = 0; i < eLIGHT_MAX; i++)
	{
		CLight* pcLight = nullptr;
		CLASS_NEW(pcLight, CLight());
		m_paLightData.Set(i, pcLight);

		if (i == 0)
			pcLight->SetLightType(CLight::E_LIGHT_TYPE::eDIRECTIONAL);
		else
			pcLight->SetLightType(CLight::E_LIGHT_TYPE::ePOINT);
	}

	//m_paRenderStage.Get(static_cast<u32>(E_RENDER_STAGE_MRT_POS_NOM_DIF))->SetRenderEnable(false);

	return true;
}


//---------------------------------------------------------------------------
/*!
	@brief		描画タスク追加
	@param[in]	pcRender	レンダリングクラス
	@param[in]	eStage		追加レンダリングステージ
*/
//---------------------------------------------------------------------------
void CRenderingManager::AddRender( class CRender* pcRender, const E_RENDER_STAGE eStage  )
{
	CRenderingStage* pcStage = m_paRenderStage.Get( eStage );
	if( pcStage )
	{
		pcStage->AddRender( pcRender );
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		描画タスク削除
	@param[in]	pcRender	レンダリングクラス
	@param[in]	eStage		削除レンダリングステージ
*/
//---------------------------------------------------------------------------
void CRenderingManager::DelRender(class CRender* pcRender, const E_RENDER_STAGE eStage)
{
	CRenderingStage* pcStage = m_paRenderStage.Get(eStage);
	if (pcStage)
	{
		pcStage->DelRender(pcRender);
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		描画コマンド発行
*/
//---------------------------------------------------------------------------
void CRenderingManager::AllRenderBatch()
{
	//ビューポート設定
	CCameraManager* pcCameraManager = CCameraManager::GetInstance();
	if (pcCameraManager)
	{
		CCameraBase* pcCamera = pcCameraManager->GetActiveCamera();
		if (pcCamera)
		{
			DXMng* pcDXM = DXMng::GetInstance();
			if (pcDXM)
			{
				pcDXM->GetConDevice().RSSetViewports(1, &pcCamera->GetViewport());
			}
		}
	}

	//全ステージの描画コマンドを発行
	m_unRenderVertex = 0;
	for( s32 i = 0; i < E_RENDER_STAGE_MAX; i++ )
	{
		CRenderingStage* pcStage = m_paRenderStage.Get( i );
		if( pcStage )
		{
			pcStage->AllRenderBatch();
		}
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		描画切り替え
	@param[in]	bEnable	フラグ設定
*/
//---------------------------------------------------------------------------
void CRenderingManager::SetAllRenderEnable( const bool bEnable )
{
	m_bDrawFlag = bEnable;
}

//---------------------------------------------------------------------------
/*!
	@brief		ステージ単位描画切り替え
	@param[in]	bEnable	フラグ設定
	@param[in]	eStage	ステージID
*/
//---------------------------------------------------------------------------
void CRenderingManager::SetStageRenderEnable( const bool bEnable, const E_RENDER_STAGE eStage )
{
	CRenderingStage* pcStage = m_paRenderStage.Get( eStage );
	if( pcStage )
	{
		pcStage->SetRenderEnable( bEnable );
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		ライトデータ取得
	@param[in]	nIndex	ライトインデックス
*/
//---------------------------------------------------------------------------
CLight* CRenderingManager::GetLightData(const s32 nIndex)
{
	if (nIndex >= 0 && nIndex < (s32)eLIGHT_MAX)
	{
		return m_paLightData.Get(nIndex);
	}
	return nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief		ディレクショナルライト取得
*/
//---------------------------------------------------------------------------
const CLight* CRenderingManager::GetDirectionalLight()
{
	for (s32 i = 0; i < static_cast<s32>(m_paLightData.GetSize()); ++i)
	{
		const CLight* pLight = m_paLightData.Get(i);
		if (pLight != nullptr && pLight->GetLightType() == CLight::E_LIGHT_TYPE::eDIRECTIONAL)
		{
			return pLight;
		}
	}
	return nullptr;
}

//EOF