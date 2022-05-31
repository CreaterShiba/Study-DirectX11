
/*==========================================================================*/
/*==========================================================================*/
/*!
@file		CollisionGeometry.h
@brief		コリジョンジオメトリ
@author		shiba
@date		2014/07/28 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "CollisionGeometry.h"
#include "CollisionManagerSys.h"
#include "FrameWork/Utility/UtilCoord.h"

CCollisionGeomBase::CCollisionGeomBase()
	: m_eColGeomType(E_COL_GEOM_TYPE::eCOL_GEOM_NONE)
	, m_eColObjectType(E_COL_OBJECT_TYPE::eCOL_OBJECT_NONE)
	, m_fScale(1.0f)
	, m_pTrailCoord(nullptr)
	, m_bColEnable(true)
#if defined(_DEBUG)
	, m_pDebugGeom(nullptr)
#endif
	, m_nParam(0)
{
	m_cWorldMat = DirectX::XMMatrixIdentity();
}

CCollisionGeomBase::~CCollisionGeomBase()
{
	m_pTrailCoord = nullptr;
}

//EOF
