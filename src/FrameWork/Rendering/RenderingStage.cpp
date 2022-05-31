/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingStage.cpp
	@brief		レンダリングステージ
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include <algorithm>
#include <functional>

//#include "FrameWork/Resource/ResourceManager.h"
//#include "FrameWork/Asset/AssetLoader.h"

#include "FrameWork/Rendering/RenderingContext.h"
#include "FrameWork/DXUtil/UtilDX11.h"

#include "FrameWork/Rendering/RenderingStage.h"
#include "FrameWork/Rendering/Render.h"
#include "FrameWork/Rendering/RenderingTexture.h"
#include "FrameWork/Rendering/ModelDataType.h"

#include "FrameWork/Rendering/Shader/ShaderBase.h"
#include "FrameWork/Rendering/Shader/ShaderManager.h"
#include "FrameWork/Rendering/Shader/ShaderBinder.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CRenderingStage::CRenderingStage( const CRenderingManager::E_RENDER_STAGE eStage )
: m_paRenderList()
, m_eRenderStage( eStage )
, m_pcStageShader(nullptr)
, m_bClearRenderTarget( false )
, m_bClearDepth( false )
, m_bClearStencil( false )
, m_bRenderEnable( true )
, m_bUpdateRenderList(false)
{
	m_paRenderList.clear();
	m_paRenderingTarget.clear();
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CRenderingStage::~CRenderingStage()
{
	Term();
}

//static u32 unNomUIID = 0;
//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CRenderingStage::Init()
{
	//ステージによっては使用するシェーダは固定とする
	CShaderManager* pcShaderM = CShaderManager::GetInstance();
	NS_ASSERT( pcShaderM );
	switch ( m_eRenderStage )
	{
	case CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF:
		m_pcStageShader = pcShaderM->GetShader(CShaderManager::eSHADERID_MRT);
		break;
	case CRenderingManager::E_RENDER_STAGE_WRITEZ:
		m_pcStageShader = pcShaderM->GetShader(CShaderManager::eSHADERID_ZRENDER);
		break;
	default:
		break;
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CRenderingStage::Update( const f32 fDelta )
{
	NS_UNUSED( fDelta );
}

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CRenderingStage::Term()
{
	m_pcStageShader = nullptr;

	std::vector< CRenderingTexture* >::iterator it = m_paRenderingTarget.begin();
	for( ; it != m_paRenderingTarget.end(); ++it )
	{
		SAFE_DELETE( *it );
	}
	m_paRenderingTarget.clear();
	m_paRenderList.clear();
}

//---------------------------------------------------------------------------
/*!
	@brief	レンダリングターゲットを作成
	@param[in]	desc	テクスチャ設定
*/
//---------------------------------------------------------------------------
bool CRenderingStage::CreateRenderingTarget( const D3D11_TEXTURE2D_DESC& desc )
{
	//レンダリングターゲット生成
	m_paRenderingTarget.push_back( new CRenderingTexture );
	bool bRet = m_paRenderingTarget[ m_paRenderingTarget.size() - 1 ]->CreateRenderingTarget( desc );

	return bRet;
}

//---------------------------------------------------------------------------
/*!
	@brief	スワップチェインから取得してレンダリングターゲット作成
*/
//---------------------------------------------------------------------------
bool CRenderingStage::CreateRenderTargetForSwapchain()
{
	//レンダリングターゲット生成
	NS_ASSERT( m_paRenderingTarget.size() == 0 );

	//複数対応はしない予定
	m_paRenderingTarget.push_back( new CRenderingTexture );
	bool bRet = m_paRenderingTarget[0]->CreateRenderTargetForSwapchain();

	return bRet;
}


//---------------------------------------------------------------------------
/*!
	@brief	描画タスクに追加
*/
//---------------------------------------------------------------------------
void CRenderingStage::AddRender( class CRender* pcRender )
{
	if (!IsExistRender(pcRender))
	{
		m_paRenderList.push_back(pcRender);
		m_bUpdateRenderList = true;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	描画タスクから消す
*/
//---------------------------------------------------------------------------
void CRenderingStage::DelRender(class CRender* pcRender)
{
	if (pcRender != nullptr)
	{
		m_paRenderList.remove(pcRender);
		m_bUpdateRenderList = true;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	描画切り替え
*/
//---------------------------------------------------------------------------
void CRenderingStage::SetRenderEnable( const bool bEnable )
{
	m_bRenderEnable = bEnable;
}

//---------------------------------------------------------------------------
/*!
	@brief	描画コマンド発行
*/
//---------------------------------------------------------------------------
void CRenderingStage::AllRenderBatch()
{	
	if( !m_bRenderEnable )
	{
		//描画無効化されてる。
		return;
	}

	//	描画
	DXMng* const pcDirectX = DXMng::GetInstance();
	if( pcDirectX )
	{
		CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
		NS_ASSERT( pcRenderM );

		if( m_paRenderingTarget.size() > 0 )
		{
			// レンダリングターゲットの設定
#if !defined(WRITEZ_HARDWARE_TEST)
			std::vector< ID3D11RenderTargetView* > pTbl;
			pTbl.clear();
			pTbl.reserve( m_paRenderingTarget.size() );
			for( u32 i = 0; i < m_paRenderingTarget.size(); ++i )
			{
				pTbl.push_back( m_paRenderingTarget[ i ]->GetRenderTargetView() );
			}
			pcDirectX->GetConDevice().OMSetRenderTargets( pTbl.size(), &pTbl[0] , pcRenderM->GetRenderTargetDS() );
#else
			if (m_eRenderStage != CRenderingManager::E_RENDER_STAGE_WRITEZ)
			{
				std::vector< ID3D11RenderTargetView* > pTbl;
				pTbl.clear();
				pTbl.reserve(m_paRenderingTarget.size());
				for (u32 i = 0; i < m_paRenderingTarget.size(); ++i)
				{
					pTbl.push_back(m_paRenderingTarget[i]->GetRenderTargetView());
				}
				pcDirectX->GetConDevice().OMSetRenderTargets(pTbl.size(), &pTbl[0], pcRenderM->GetRenderTargetDS());
			}
			else
			{
				pcDirectX->GetConDevice().OMSetRenderTargets(0, nullptr, pcRenderM->GetRenderTargetDS());
			}
#endif
			if( m_bClearRenderTarget )
			{
				//@todo クラス分けしてもいいかもね
				for( u32 i = 0; i < m_paRenderingTarget.size(); i++ )
				{
					ID3D11RenderTargetView* pcRenderView = m_paRenderingTarget[i]->GetRenderTargetView();
					if ( pcRenderView )
					{
						f32 faColor[4] = { 0.5f, 0.5f, 1.0f, 1.0f };
#if 1
						if( m_eRenderStage == CRenderingManager::E_RENDER_STAGE_WRITEZ  )
						{
							faColor[0] = 0.8f;
						}
						else if( m_eRenderStage == CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF )
						{
							faColor[0] = faColor[1] = faColor[2] = faColor[3] = 0.0f;

							//カラー出力時にシャドウの処理を行ってみるので設定しておく
							const CRenderingStage& cStageSprite = pcRenderM->GetRenderStage(CRenderingManager::E_RENDER_STAGE_WRITEZ);
							CRenderingTexture* pcZBuffer = cStageSprite.m_paRenderingTarget[0];
							if (pcZBuffer)
							{
								ID3D11ShaderResourceView* pResouceView = pcZBuffer->GetTextureRevi();
								pcDirectX->GetConDevice().PSSetShaderResources(2, 1, &pResouceView);
							}
						}
						else if (m_eRenderStage == CRenderingManager::E_RENDER_GET_BRIGHT)
						{
							faColor[0] = faColor[1] = faColor[2] = faColor[3] = 0.0f;
							faColor[3] = 1.0f;
							//輝度抽出
							const CRenderingStage& cMainStage = pcRenderM->GetRenderStage(CRenderingManager::E_RENDER_STAGE_MAIN);
							CRenderingTexture* pcMainRenderTarget = cMainStage.m_paRenderingTarget[0];
							if (pcMainRenderTarget)
							{
								ID3D11ShaderResourceView* pResouceView = pcMainRenderTarget->GetTextureRevi();
								pcDirectX->GetConDevice().PSSetShaderResources(0, 1, &pResouceView);
							}
						}
						else if (m_eRenderStage == CRenderingManager::E_RENDER_BLUR_BRIGHT)
						{
							//輝度ボカシ
							const CRenderingStage& cMainStage = pcRenderM->GetRenderStage(CRenderingManager::E_RENDER_GET_BRIGHT);
							CRenderingTexture* pcMainRenderTarget = cMainStage.m_paRenderingTarget[0];
							if (pcMainRenderTarget)
							{
								ID3D11ShaderResourceView* pResouceView = pcMainRenderTarget->GetTextureRevi();
								pcDirectX->GetConDevice().PSSetShaderResources(0, 1, &pResouceView);
							}
						}
						else if (m_eRenderStage == CRenderingManager::E_RENDER_STAGE_POSTEFFECT)
						{
							//ポストエフェクト用にターゲット登録しておく
							const CRenderingStage& cMainStage = pcRenderM->GetRenderStage(CRenderingManager::E_RENDER_STAGE_MAIN);
							CRenderingTexture* pcMainRenderTarget = cMainStage.m_paRenderingTarget[0];
							if (pcMainRenderTarget)
							{
								ID3D11ShaderResourceView* pResouceView = pcMainRenderTarget->GetTextureRevi();
								pcDirectX->GetConDevice().PSSetShaderResources(0, 1, &pResouceView);
							}
							//UI等はポストエフェクトにかからないので別枠とする
							const CRenderingStage& cStageSprite = pcRenderM->GetRenderStage(CRenderingManager::E_RENDER_STAGE_SPRITE);
							CRenderingTexture* pcUIRenderTarget = cStageSprite.m_paRenderingTarget[0];
							if (pcUIRenderTarget)
							{
								ID3D11ShaderResourceView* pResouceView = pcUIRenderTarget->GetTextureRevi();
								pcDirectX->GetConDevice().PSSetShaderResources(1, 1, &pResouceView);
							}

							//Bloom用テクスチャ
							const CRenderingStage& cBloomSprite = pcRenderM->GetRenderStage(CRenderingManager::E_RENDER_BLUR_BRIGHT);
							CRenderingTexture* pcBloomRenderTarget = cBloomSprite.m_paRenderingTarget[0];
							if (pcBloomRenderTarget)
							{
								ID3D11ShaderResourceView* pResouceView = pcBloomRenderTarget->GetTextureRevi();
								pcDirectX->GetConDevice().PSSetShaderResources(2, 1, &pResouceView);
							}
						}
						else if (m_eRenderStage == CRenderingManager::E_RENDER_STAGE_SPRITE)
						{
							faColor[0] = faColor[1] = faColor[2] = faColor[3] = 0.0f;
						}
#endif
						pcDirectX->GetConDevice().ClearRenderTargetView( pcRenderView, faColor);
					}
				}
			}

			ID3D11DepthStencilView* pcStencilView = pcRenderM->GetRenderTargetDS();
			if( pcStencilView )
			{
				u32 unClearFlag = 0;
				if( m_bClearDepth )
				{
					unClearFlag |= D3D11_CLEAR_DEPTH;
				}
				if( m_bClearStencil )
				{
					unClearFlag |= D3D11_CLEAR_STENCIL;
				}
				if (unClearFlag != 0)
				{
					pcDirectX->GetConDevice().ClearDepthStencilView(pcStencilView, unClearFlag, 1.0f, 0);
				}
			}
		}

		//描画優先順ソート
		if (m_bUpdateRenderList)
		{
			SortRenderList();

			//@todo 
			//オクルージョンカリング
			//半透明、不透明でリストを分割
			//不透明をマテリアルソート
			//半透明をZソート

			m_bUpdateRenderList = false;
		}
		
		//--------------------------------------------------------------------
		// 描画コマンドの発行
		//--------------------------------------------------------------------
		std::list< CRender* >::iterator it = m_paRenderList.begin();
		for( ; it != m_paRenderList.end(); ++it )
		{
			if ((*it)->GetDrawEnable(m_eRenderStage))
			{
				//@todo メッシュを全部持ってきて、ソートして描画する予定

				//ステージに関連づけられたオーバーライドシェーダがあるならそちらを使用させる
				CRenderingContext& conText = (*it)->GetRenderContext();
				conText.SetOverShader( m_pcStageShader );
				
				(*it)->PreDrawCalc();
				(*it)->Draw();
				(*it)->PostDrawCalc();

				conText.SetOverShader(nullptr);
			}
		}

		//全ステージ描画完了。
		//画面に出力。
		if( m_eRenderStage == CRenderingManager::E_RENDER_STAGE_MAX - 1 )
		{
			pcDirectX->GetSwapChain().Present( 0, 0 );
		}

		for (s32 i = 0; i < TEXTURE_MAX_NUM; i++)
		{
			//nullptrをセットしておく
			ID3D11ShaderResourceView* pcResoure = nullptr;
			pcDirectX->GetConDevice().PSSetShaderResources(i, 1, &pcResoure);
		}
	}
}

bool AscendingSortFunc(CRender* val1, CRender* val2) 
{
	return val1->m_unDrawPriority < val2->m_unDrawPriority;
}

//---------------------------------------------------------------------------
/*!
	@brief	優先度にそってソート
*/
//---------------------------------------------------------------------------
void CRenderingStage::SortRenderList()
{
	//m_paRenderList.sort(m_paRenderList.begin(), m_paRenderList.end(), [](const CRender& a, const CRender &b) { return a.m_unDrawPriority > b.m_unDrawPriority; });

	m_paRenderList.sort(AscendingSortFunc);
}

//---------------------------------------------------------------------------
/*!
	@brief	すでにリストに存在しているかチェック
*/
//---------------------------------------------------------------------------
bool CRenderingStage::IsExistRender(class CRender* pcRender)
{
	bool bExist = false;
	std::list< CRender* >::iterator it = m_paRenderList.begin();
	for (; it != m_paRenderList.end(); ++it)
	{
		if (*it == pcRender)
		{
			bExist = true;
		}
	}
	return bExist;
}

#if defined( _DEBUG )
//---------------------------------------------------------------------------
/*!
	@brief	ターゲット描画用スプライトを作成
*/
//---------------------------------------------------------------------------
bool CRenderingStage::CreateDebugSprite( const NSVector2& pos, const NSVector2& size, u16 idx/* = 0 */)
{
	if( m_paRenderingTarget.size() > 0 )
	{
		if( m_paRenderingTarget[ idx ] )
		{
			m_paRenderingTarget[ idx ]->CreateDebugSprite( pos, size );
		}
	}

	return true;
}
#endif

//EOF
