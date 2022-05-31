#pragma once

#include "FrameWork/Task/Task.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UnitBase.h
	@brief		ユニット基底
	@author		shiba
	@date		2015/07/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class CUnitBase : public CTask
{
public:
	CUnitBase();
	virtual ~CUnitBase();

protected:
	class CModel* m_pcUnitModel;			//!< ユニットモデルデータ
	class CCollisionGeomBase* m_pCollision;	//!< コリジョンデータ

	s32 m_nHP;	//!< 耐久度
	s32 m_nLightID;	//!< ライトID

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
	void SetLightID(const s32 nID)
	{
		m_nLightID = nID;
		TrailLightPos();
	}

	//@brief 移動前座標取得
	const NSVector3& GetBeforeMovingPos() const
	{
		return m_vBeforeMovingPos;
	}

protected:
	void TrailLightPos();
	void SetLightIntensity();

protected:
	NSVector3 m_vBeforeMovingPos;	//!< 移動前座標
	f32 m_fLightIntensity;			//!< ライト強度
};

//EOF
