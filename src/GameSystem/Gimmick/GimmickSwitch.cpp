/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GimmickSwitch.cpp
	@brief		スイッチ
	@author		shiba
	@date		2015/07/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "GimmickSwitch.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"
#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/TextureDataType.h"
#include "FrameWork/Rendering/ModelDataType.h"
#include "FrameWork/Collision/CollisionGeometry.h"
#include "FrameWork/Utility/UtilSound.h"

CGimmickSwitch::CGimmickSwitch() 
	: m_bCountStop(false)
	, m_bSwitchOnOff(false)
	, m_fSwitchCounter( 0.0f )
{

}

CGimmickSwitch::~CGimmickSwitch()
{

}

bool CGimmickSwitch::Init()
{
	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	NS_ASSERT(pcAssetM);

	//ゴールモデル読み込み
	CLASS_NEW(m_pcUnitModel, CModel);
	if (m_pcUnitModel->LoadModel(_T("resource/Graphics/model/switch.obj")))
	{
		m_pcUnitModel->SetScale(NSVector3(170.0f, 300.0f, 170.0f));

		m_pcUnitModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_MRT_POS_NOM_DIF);
		m_pcUnitModel->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_WRITEZ);

		//テクスチャの生成、ID割当(適当に割り当て)
		TCHAR strTexName[256] = { 0 };
		char* texPath = "resource/Graphics/texture/Cha_SlimBlue.tga";
		mbstowcs(strTexName, texPath, strlen(texPath));

		SInitTextureParam cParam;
		wcscpy_s(cParam.strTexName, TEXTURE_FILE_PATH, strTexName);
		cParam.bTGA = true;

		//テクスチャ割当
		CAssetTexture* pcTexture = pcAssetM->LoadTexture(cParam);
		m_pcUnitModel->SetDiffuseTexID(0, pcTexture->GetAssetUID());
	}

	CLASS_NEW(m_pCollision, CCollisionSphere());
	if (m_pCollision != nullptr)
	{
		m_pCollision->SetTrailCoord(m_pcUnitModel);
		static_cast<CCollisionSphere*>(m_pCollision)->SetScale(5.0f);
	}
	return true;
}

void CGimmickSwitch::Update(const f32 fDelta)
{
	if (m_bSwitchOnOff && !m_bCountStop)
	{
		m_fSwitchCounter += 1.0f / FPS;
		if (SWITCH_COUNT_MAX <= m_fSwitchCounter)
		{
			CUtilSound::PlaySE(eSOUNDID_SWITCH);
			SetSwitchOnOff(false);

			//@todo ステージに通知。ギミック内部にあるので外側から監視できるように分離する
		}
	}
}

void CGimmickSwitch::Destroy()
{

}

void CGimmickSwitch::SetSwitchOnOff(const bool bOnOff)
{
	if (m_pcUnitModel == nullptr)
	{
		return;
	}
	
	NSVector3 vPos = m_pcUnitModel->GetPos();
	if (bOnOff)
	{
		m_pcUnitModel->SetPos(vPos.x, -5.5f, vPos.z);
		m_bSwitchOnOff = true;
		m_fSwitchCounter = 0.0f;

		if (m_onFunction != nullptr)
		{
			m_onFunction();
		}
	}
	else
	{
		m_bSwitchOnOff = false;
		m_pcUnitModel->SetPos(vPos.x, -4.0f, vPos.z);

		if (m_offFunction != nullptr)
		{
			m_offFunction();
		}
	}
}

//EOF
