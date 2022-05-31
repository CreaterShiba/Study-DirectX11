#pragma once

#include "UnitBase.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UnitPlayer.h
	@brief		プレイヤーユニット操作
	@author		shiba
	@date		2015/07/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class CUnitPlayer : public CUnitBase
{
public:
	CUnitPlayer();
	~CUnitPlayer();

	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();

	//@brief 移動処理行われた？
	const bool IsMoving() const
	{
		return m_bMoving;
	}

	void InvokeMove();
	void OnDead();
	void OnGoal();

	const bool IsDead()
	{
		return m_ePlayerState == ePLAYER_STATE_DEAD;
	}

	const bool IsGoal()
	{
		return m_ePlayerState == ePLAYER_STATE_GOAL;
	}

private:
	//ステート
	enum E_PLAYER_STATE
	{
		ePLAYER_STATE_WAIT,	//待機(入力は受け付けない)
		ePLAYER_STATE_STAY,	//待機
		ePLAYER_STATE_WALK,	//歩き
		ePLAYER_STATE_DEAD,	//しぼう
		ePLAYER_STATE_GOAL,	//ゴール
		ePLAYER_STATE_MAX,
	};

	//モーションメモ
	//0:Stay 1:Walk 2:しぼう 3:ダメージ 4:攻撃
	enum E_MOTION_ID
	{
		eMOTION_ID_STAY,
		eMOTION_ID_WALK,
		eMOTION_ID_DEAD,
		eMOTION_ID_DMG,
		eMOTION_ID_ATK,
		eMOTION_ID_MAX
	};

private:
	void Term();
	void UpdateState();
	void OnState();
	void OffState();
	void ChangeState( const E_PLAYER_STATE eState );
	NSVector2 GetMovingPos() const;

private:
	E_PLAYER_STATE m_ePlayerState;	//!< プレイヤーステート
	bool m_bMoving;

	f32 m_fJumpF;
	f32 m_fGoalRoateCount;
};

//EOF
