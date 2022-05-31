#pragma once

#include "FrameWork/Task/Task.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GimmickBase.h
	@brief		ギミック基底
	@author		shiba
	@date		2015/07/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class CGimmickBase : public CTask
{
public:
	CGimmickBase();
	virtual ~CGimmickBase();

protected:
	class CModel* m_pcUnitModel;			//!< ユニットモデルデータ
	class CCollisionGeomBase* m_pCollision;	//!< コリジョンデータ

private:
	void Term();

public:
	CCollisionGeomBase* GetCollision() const
	{
		return m_pCollision;
	}
	CModel* GetModel() const
	{
		return m_pcUnitModel;
	}
	void SetupLightID(const s32 nLightID);

private:
	s32 m_nLightID;
};

//EOF
