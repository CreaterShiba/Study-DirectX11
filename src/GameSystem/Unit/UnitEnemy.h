#pragma once

#include "UnitBase.h"
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UnitEnemy.h
	@brief		エネミーユニット操作
	@author		shiba
	@date		2015/07/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class CUnitEnemy : public CUnitBase
{
public:
	CUnitEnemy();
	~CUnitEnemy();

	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();

	//エネミータイプ
	enum  E_ENEMY_TYPE
	{
		eNORMAL,
		eDANGER,
		eMAX
	};

private:
	//ステート
	enum E_ENEMY_STATE
	{
		eENEMY_STATE_STAY,	//待機
		eENEMY_STATE_WALK,	//歩き
		eENEMY_STATE_MAX,
	};

	//モーションメモ
	enum E_MOTION_ID
	{
		eMOTION_ID_STAY,
		eMOTION_ID_WALK,
		eMOTION_ID_DEAD,
		eMOTION_ID_DMG,
		eMOTION_ID_ATK,
		eMOTION_ID_MAX
	};


public:
	void AddTrailPos(const NSVector2& vPos)
	{
		m_vaRootPos.push_back(vPos);
	}
	void SetTrailSpeed(const f32 fSpeed)
	{
		m_fTrailSpeed = fSpeed;
	}
	void SetupModel(const E_ENEMY_TYPE eEnemyType);
	void InvokeMove();

private:
	void Term();
	void UpdateState();
	void OnState();
	void OffState();
	void ChangeState(const E_ENEMY_STATE eState);
	void ResetTrailParam();

	E_ENEMY_STATE m_eEnemyState;

	std::vector<NSVector2> m_vaRootPos;	//!< 追跡経路
	s32 m_nTrailID;			//!< 追跡ID
	s32 m_nTrailBaseId;	
	f32 m_fTrailCounter;	//!< 追跡かうんた
	f32 m_fTrailSpeed;		//!< 追跡スピード
	bool m_bTrailDir;

	E_ENEMY_TYPE m_eEnemyType;

	f32 m_fLightCounter = 0.0f;
};

//EOF

