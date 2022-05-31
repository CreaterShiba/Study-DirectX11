/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Render.cpp
	@brief		描画基底クラス
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "Render.h"

#include "RenderingManager.h"
#include "ModelDataType.h"

using namespace DirectX;

//---------------------------------------------------------------------------
/*!
@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CRender::CRender()
	: m_cRenderContext()
	, m_unDrawPriority((u32)E_RENDER_PRIORITY::eREN_PRI_DEFAULT)
{
	m_WorldMat = XMMatrixIdentity();
	for (s32 i = 0; i < CRenderingManager::E_RENDER_STAGE_MAX; ++i)
	{
		m_cDrawBit[i] = false;
	}
}

//---------------------------------------------------------------------------
/*!
@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CRender::~CRender()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
@brief	破棄
*/
//---------------------------------------------------------------------------
void CRender::Term()
{
	//リストから外す
	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	if (pcRenderM != nullptr)
	{
		for (s32 i = 0; i < CRenderingManager::E_RENDER_STAGE_MAX; ++i)
		{
			if (m_cDrawBit[i])
			{
				pcRenderM->DelRender(this, (CRenderingManager::E_RENDER_STAGE)i);
			}
		}
	}
}

//---------------------------------------------------------------------------
/*!
@brief	初期化
*/
//---------------------------------------------------------------------------
bool CRender::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CRender::Update(const f32 fDelta)
{
	NS_UNUSED(fDelta);
	//@todo 静的モデルは変換いらないはず
	if (m_bUpdate)
	{
		const NSVector3& vScale = GetScale();
		const NSVector3& vRot = GetRotate();

		//ワールドマトリクスの更新
		//m_pGeometry->m_InstanceAttr.m_ModelMatrix = GetOrthoLocalMatrix();
		m_WorldMat = XMMatrixScaling(vScale.x, vScale.y, vScale.z) *
			XMMatrixRotationRollPitchYaw(
				UtilMath::DegreeToRadian(vRot.x),
				UtilMath::DegreeToRadian(vRot.y),
				UtilMath::DegreeToRadian(vRot.z));

		m_WorldMat._41 = GetPos().x;
		m_WorldMat._42 = GetPos().y;
		m_WorldMat._43 = GetPos().z;
		m_bUpdate = false;
	}
}

//---------------------------------------------------------------------------
/*!
@brief	描画情報設定
*/
//---------------------------------------------------------------------------
void CRender::SetRenderContext(const CRenderingContext& cContext)
{
	m_cRenderContext = cContext;
}

//---------------------------------------------------------------------------
/*!
@brief	描画情報の取得
*/
//---------------------------------------------------------------------------
CRenderingContext& CRender::GetRenderContext()
{
	return m_cRenderContext;
}

//---------------------------------------------------------------------------
/*!
	@brief	描画の有効、無効化
*/
//---------------------------------------------------------------------------
void CRender::SetDrawEnable(const bool bEnable, const CRenderingManager::E_RENDER_STAGE eStageID)
{
	m_cDrawBit[eStageID] = bEnable;

	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	if (pcRenderM != nullptr)
	{
		//@todo ちょっと処理の負荷が心配なので要チェック
		if (bEnable)
		{
			//描画リストへの追加
			pcRenderM->AddRender(this, eStageID);
		}
		else
		{
			//描画リストへの削除
			pcRenderM->DelRender(this, eStageID);
		}
	}
}

//---------------------------------------------------------------------------
/*!
@brief	描画の有効、無効化取得
*/
//---------------------------------------------------------------------------
bool CRender::GetDrawEnable(const CRenderingManager::E_RENDER_STAGE eStageID) const
{
	return m_cDrawBit[eStageID];
}

//---------------------------------------------------------------------------
/*!
@brief	描画優先度設定
*/
//---------------------------------------------------------------------------
void CRender::SetRenderPriority(const E_RENDER_PRIORITY ePri)
{
	m_unDrawPriority = (u32)ePri;
}

//EOF
