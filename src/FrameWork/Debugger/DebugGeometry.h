#pragma once

#if defined( _DEBUG )

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugGeometry.h
	@brief		デバッグ用ジオメトリ
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Rendering/RenderingManager.h"
#include "FrameWork/Rendering/Model.h"

class CAssetModel;
//==========================================================================*/
/*!
	@class	CDebugGeometry
	@brief	デバッグ用ジオメトリ
	@note	描画のテスト等に用いるだけ
*/
//==========================================================================*/
class CDebugGeometry
{
public:
	//デバッグのジオメトリタイプ
	enum  E_GEOM_TYPE
	{
		eCUBE,
		eSKIN,
		eMAX
	};

public:
	CDebugGeometry();
	~CDebugGeometry();

	//初期化
	bool Init();

	//ジオメトリセットアップ
	void SetupGeom(E_GEOM_TYPE eGeomType);

	//破棄
	void Destroy();

	//モデルへのインターフェイス
	inline void SetPos(const NSVector3& vPos);
	inline void SetScale(const NSVector3& vScale);
	inline void SetRotate(const NSVector3& vRotate);
	
	inline void SetRenderContext(const CRenderingContext& cContext);
	inline void SetDrawEnable(const bool bEnable, const CRenderingManager::E_RENDER_STAGE eStageID);

private:
	//モデル
	CModel* m_pcDebugGeomModel;

private:
	void Term();
};

void CDebugGeometry::SetPos(const NSVector3& vPos)
{
	if (m_pcDebugGeomModel != nullptr)
	{
		m_pcDebugGeomModel->SetPos(vPos);
	}
}

void CDebugGeometry::SetScale(const NSVector3& vScale)
{
	if (m_pcDebugGeomModel != nullptr)
	{
		m_pcDebugGeomModel->SetScale(vScale);
	}
}

void CDebugGeometry::SetRotate(const NSVector3& vRotate)
{
	if (m_pcDebugGeomModel != nullptr)
	{
		m_pcDebugGeomModel->SetRotate(vRotate);
	}
}

void CDebugGeometry::SetRenderContext(const CRenderingContext& cContext)
{
	if (m_pcDebugGeomModel != nullptr)
	{
		m_pcDebugGeomModel->SetRenderContext(cContext);
	}
}

void CDebugGeometry::SetDrawEnable(const bool bEnable, const CRenderingManager::E_RENDER_STAGE eStageID)
{
	if (m_pcDebugGeomModel != nullptr)
	{
		m_pcDebugGeomModel->SetDrawEnable(bEnable, eStageID);
	}
}

#endif	//	_DEBUG

//EOF