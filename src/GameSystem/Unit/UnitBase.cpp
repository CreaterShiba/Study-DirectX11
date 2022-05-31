/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UnitBase.cpp
	@brief		ユニット基底
	@author		shiba
	@date		2015/07/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "UnitBase.h"
#include "FrameWork/Collision/CollisionGeometry.h"
#include "FrameWork/Rendering/Light.h"
#include "FrameWork/Rendering/RenderingManager.h"
#include "FrameWork/Rendering/Model.h"

CUnitBase::CUnitBase() 
	: m_pcUnitModel(nullptr)
	, m_pCollision(nullptr)
	, m_nHP( 10 )
	, m_nLightID(-1)
	, m_fLightIntensity(1.0f)
{
}


CUnitBase::~CUnitBase()
{
	Term();
}

void CUnitBase::Term()
{
	SAFE_DELETE(m_pCollision);
	SAFE_DELETE_TASK(m_pcUnitModel);
}

void CUnitBase::TrailLightPos()
{
	if (m_nLightID >= 0)
	{
		CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
		NS_ASSERT(pcRenderM);
		CLight* pcLight = pcRenderM->GetLightData(m_nLightID);
		if (pcLight && m_pcUnitModel != nullptr)
		{
			pcLight->SetPos(m_pcUnitModel->GetPos());
		}
	}
}

void CUnitBase::SetLightIntensity()
{
	if (m_nLightID >= 0)
	{
		CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
		NS_ASSERT(pcRenderM);
		CLight* pcLight = pcRenderM->GetLightData(m_nLightID);
		if (pcLight)
		{
			pcLight->SetIntensity(m_fLightIntensity);
		}
	}
}

//EOF