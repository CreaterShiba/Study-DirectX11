#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
@file		Render.h
@brief		描画用基底クラス
@author		shiba
@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "FrameWork/Task/Task.h"
#include "FrameWork/Rendering/RenderingManager.h"
#include "FrameWork/Rendering/Shader/ShaderManager.h"
#include "FrameWork/Utility/UtilBit.h"
#include "FrameWork/Utility/UtilCoord.h"
#include "RenderingContext.h"

class CGeometryInstance;
class CTextureContext;
class CRender;

//==========================================================================*/
/*!
	@class	CRender
	@brief	描画インターフェイス
*/
//==========================================================================*/
class CRender : public CTask, public CUtilCoord
{
public:
	// @brief コンストラクタ
	CRender();

	// @brief デストラクタ
	virtual ~CRender();

	// @brief 初期化
	bool Init();

	// @brief 更新
	virtual void Update(const f32 fDelta);

	// @brief 描画前更新
	virtual void PreDrawCalc() {};

	// @brief 描画
	virtual void Draw() = 0;

	// @brief 描画後更新
	virtual void PostDrawCalc() {};

	// @brief 描画情報設定
	void SetRenderContext(const CRenderingContext& cContext);

	// @brief 描画情報取得
	CRenderingContext& GetRenderContext();

	// @brief 描画の有効無効化(ステージへの登録も同時に行います。
	void SetDrawEnable(const bool bEnable, const CRenderingManager::E_RENDER_STAGE eStageID);

	// @brief 有効、無効獲得
	bool GetDrawEnable(const CRenderingManager::E_RENDER_STAGE eStageID) const;

	// @brief 描画優先度設定
	void SetRenderPriority(const E_RENDER_PRIORITY ePri);

	// @brief ワールド座標系獲得
	NSMatrix& GetWorldMatrix()
	{
		return m_WorldMat;
	}
private:
	// @brief 破棄
	void Term();

protected:
	//!< 描画対象ステージ、ONOFFビット
	//@todo ビット使ってない
	//CUtilBit m_cDrawBit;
	bool m_cDrawBit[CRenderingManager::E_RENDER_STAGE_MAX];

	//!< レンダリング設定
	CRenderingContext	m_cRenderContext;

	//!< ワールドマトリクス
	NSMatrix m_WorldMat;

public:
	//!< 描画プライオリティ
	u32 m_unDrawPriority = 0;
};

//EOF