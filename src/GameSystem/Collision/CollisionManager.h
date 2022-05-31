#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CollisionManager.h
	@brief		コリジョン管理(ゲーム部分)
	@author		shiba
	@date		2015/07/16 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Collision/CollisionManagerSys.h"

class CUnitBase;
class CUnitPlayer;
class CUnitEnemy;
class CStageManager;
class CGimmickGoal;
class CGimmickSwitch;

class CCollisionManager : public CCollisionManagerSys
{
public:
	NS_SINGLETON(CCollisionManager);
	CCollisionManager();
	~CCollisionManager();

public:
	virtual bool Init();
	virtual void Destroy();

	virtual void CheckAllCollision();

	//@todo コリジョン追加する形になってるので自動登録とコールバック式に変更
	void SetUnitPlayer(CUnitPlayer* pUnitPlayer);
	void SetStageManager(CStageManager* pStageM);

	void SetGoalUnit(CUnitPlayer* pUnit);
	void SetGimmickSwitch(CGimmickSwitch* pGimmick);

	void AddUnitEnemy(CUnitEnemy* pUnitEnemy);

	void SetPassableAreaAABB(CCollisionAABB* pAABB);

	void SetupGoalCollision();

private:
	//レイとステージAABBチェック
	bool CheckUnitRaytoStageAABB(class CUtilCoord* pcUnit, const NSVector3& vMoveVec);

	//ゴールとプレイヤーチェック
	bool IsGoalUnit();

	//プレイヤーとスイッチ開閉チェック
	bool IsOpenSwitch();

private:
	void Term();

private:
	CUnitPlayer* m_pcUnitPlayer;
	std::vector< CUnitEnemy* > m_pcaUnitEnemy;
	CStageManager* m_pcStageM;

	CGimmickSwitch* m_pcGimmickSwitch;
	CUnitPlayer* m_pcGoalUnit;
	CCollisionSphere* m_pcGoalTrigger;	//!< ゴールトリガー

	//エネミー起動トリガー
	CCollisionSphere* m_pcEnemyInvokeTrigger;
	CCollisionSphere* m_pcDangerEnemyInvokeTrigger;

	CCollisionAABB* m_pPassableArea;
};

//EOF
