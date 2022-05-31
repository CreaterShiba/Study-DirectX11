/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GimmickBase.cpp
	@brief		ギミック基底
	@author		shiba
	@date		2015/07/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "GimmickBase.h"
#include "FrameWork/Collision/CollisionGeometry.h"
#include "FrameWork/Rendering/RenderingManager.h"
#include "FrameWork/Rendering/Light.h"
#include "FrameWork/Rendering/Model.h"

CGimmickBase::CGimmickBase()
	: m_pcUnitModel(nullptr)
	, m_pCollision(nullptr)
{
}


CGimmickBase::~CGimmickBase()
{
	Term();
}

void CGimmickBase::Term()
{
	SAFE_DELETE(m_pCollision);
	SAFE_DELETE_TASK(m_pcUnitModel);
}

void CGimmickBase::SetupLightID(const s32 nLightID)
{
	if (m_pcUnitModel == nullptr)
	{
		return;
	}
	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	NS_ASSERT(pcRenderM);

	CLight* pcLight = pcRenderM->GetLightData(nLightID);
	if (pcLight != nullptr)
	{
		pcLight->SetPos(m_pcUnitModel->GetPos());
		m_nLightID = nLightID;
	}
}

//EOF