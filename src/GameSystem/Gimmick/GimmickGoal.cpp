/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GimmickGoal.cpp
	@brief		ゴール
	@author		shiba
	@date		2015/07/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "GimmickGoal.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"
#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/TextureDataType.h"
#include "FrameWork/Rendering/ModelDataType.h"
#include "FrameWork/Collision/CollisionGeometry.h"

CGimmickGoal::CGimmickGoal()
{

}

CGimmickGoal::~CGimmickGoal()
{
	Term();
}

bool CGimmickGoal::Init()
{
	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	NS_ASSERT(pcAssetM);

	//ゴールモデル読み込み
	CLASS_NEW(m_pcUnitModel, CModel);
	if (m_pcUnitModel->LoadModel(_T("resource/graphics/model/switch.obj")))
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

void CGimmickGoal::Update(const f32 fDelta)
{

}

void CGimmickGoal::Term()
{
}

void CGimmickGoal::Destroy()
{
	Term();
}

//EOF
