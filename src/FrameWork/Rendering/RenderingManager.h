#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		RenderingManager.h
	@brief		描画マネージャー
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Utility/UtilSafeArray.h"

class CRenderingStage;
class CLight;

class CRenderingManager
{
	//シングルトン
	NS_SINGLETON(CRenderingManager);
	CRenderingManager();
	~CRenderingManager();

public:

	//レンダリングステージ宣言
	//描画順は、上から下にかけて。
	enum E_RENDER_STAGE
	{
		E_RENDER_STAGE_WRITEZ,			//Z値レンダリングステージ(DX10以降からオフスクリーンで描画しなくてもいいらしい)
		E_RENDER_STAGE_MRT_POS_NOM_DIF,	//MRT使用。座標、法線、アルベドの書き込み
		E_RENDER_STAGE_MAIN,			//メインレンダリングステージ←現在Defferd処理のステージになってる

		E_RENDER_GET_BRIGHT,			//輝度抽出
		E_RENDER_BLUR_BRIGHT,			//ボカシ

		E_RENDER_STAGE_SPRITE,			//UIやらスプライト。ポストエフェクトかけないものはここに描画
		E_RENDER_STAGE_POSTEFFECT,		//ポストエフェクトステージ
		E_RENDER_STAGE_MAX,
	};

	// @brief 初期化
	bool Init();

	// @brief 描画タスクに追加する
	void AddRender(class CRender* pcRender, const E_RENDER_STAGE eStage);

	// @brief 描画タスクから外す
	void DelRender(class CRender* pcRender, const E_RENDER_STAGE eStage);

	// @brief 描画コマンド発行
	void AllRenderBatch();

	// @brief 描画切り替え
	void SetAllRenderEnable(const bool bEnable);

	// @brief ステージ単位描画切り替え
	void SetStageRenderEnable(const bool bEnable, const E_RENDER_STAGE eStage);

	// @brief ステージデータ取得
	inline const CRenderingStage& GetRenderStage(const E_RENDER_STAGE eStage);

	enum
	{
		eLIGHT_MAX = 12,
	};

	// @brief ライトデータ取得
	CLight* GetLightData(const s32 nIndex);

	// @brief ディレクショナルライト取得
	const CLight* GetDirectionalLight();

	//--------------------------------------------------------------------
	// 深度ステンシル系
	//--------------------------------------------------------------------
	inline void SetDepthStencilView( ID3D11DepthStencilView* pPtr );
	
	inline ID3D11DepthStencilView* GetRenderTargetDS() const;
	inline const ID3D11DepthStencilView* GetRenderTargetDSConst() const;		

private:

	//!< レンダリングステージ
	CSafeArray< CRenderingStage*, E_RENDER_STAGE_MAX > m_paRenderStage;	

	//!< 深度ステンシル
	ID3D11DepthStencilView* m_pDepthStencilView;							

	//!< 描画フラグ
	bool m_bDrawFlag;	

	//!< ライトデータ管理
	CSafeArray< class CLight*, eLIGHT_MAX > m_paLightData;

public:
	u32 m_unRenderVertex;
};

//--------------------------------------------------------------------
// inline
//--------------------------------------------------------------------
inline void CRenderingManager::SetDepthStencilView( ID3D11DepthStencilView* pPtr )
{
	m_pDepthStencilView = pPtr;
}
inline ID3D11DepthStencilView* CRenderingManager::GetRenderTargetDS() const
{
	return m_pDepthStencilView;
}
inline const ID3D11DepthStencilView* CRenderingManager::GetRenderTargetDSConst() const
{
	return m_pDepthStencilView;
}

inline const CRenderingStage& CRenderingManager::GetRenderStage( const E_RENDER_STAGE eStage )
{
	return *m_paRenderStage.Get( eStage );
}

//EOF