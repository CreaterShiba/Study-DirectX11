#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingStage.h
	@brief		レンダリングステージ
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include <list>
#include "RenderingManager.h"
#include "FrameWork/Utility/UtilDefine.h"

/*==========================================================================*/
/*!
	@class	CRenderingStage
	@brief	レンダリングステージ
*/
/*==========================================================================*/
class CRenderingStage
{
public:
	// @brief コンストラクタ
	explicit CRenderingStage( const CRenderingManager::E_RENDER_STAGE eStage );

	// @brief デストラクタ
	virtual ~CRenderingStage();

	// @brief 初期化
	bool Init();

	// @brief 更新
	void Update( const f32 fDelta );

	//@brief 破棄
	void Term();					

public:
	//@brief レンダリングターゲット作成
	bool CreateRenderingTarget( const D3D11_TEXTURE2D_DESC& desc );
	bool CreateRenderTargetForSwapchain();

	//@brief 描画タスクに追加する
	void AddRender( class CRender* pcRender );	

	//@brief 描画タスクから取り外す
	void DelRender(class CRender* pcRender);

	//@brief 描画の切り替え
	void SetRenderEnable( const bool bEnable );

	//@brief 描画コマンド発行
	void AllRenderBatch();						

#if defined( _DEBUG )
	//@brief レンダリングスプライト
	bool CreateDebugSprite( const NSVector2& pos, const NSVector2& size, u16 idx = 0 );
#endif

private:
	void SortRenderList();
	bool IsExistRender(class CRender* pcRender);

private:
	std::list< CRender* > m_paRenderList;							//!< レンダリングリスト
public:
	CRenderingManager::E_RENDER_STAGE m_eRenderStage;				//!< ステージID
	//class CRenderingTexture* m_pcRenderingTarget;					//!< レンダリングターゲット(MRTは複数必要)
	std::vector< class CRenderingTexture* > m_paRenderingTarget;	//!< レンダリングターゲット
	class CShaderBase* m_pcStageShader;								//!< ステ－ジシェーダー(必要なら)

	bool	m_bClearRenderTarget : 1;	//!< クリアフラグ
	bool	m_bClearDepth : 1;			//!< クリアフラグ
	bool	m_bClearStencil : 1;		//!< クリアフラグ
	bool	m_bRenderEnable : 1;		//!< 描画コマンド出します？

private: 
	bool m_bUpdateRenderList = false;
};

//EOF