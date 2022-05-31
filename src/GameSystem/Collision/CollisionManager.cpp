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
#include "CollisionManager.h"

#include "FrameWork/Utility/UtilSound.h"
#include "FrameWork/Rendering/Model.h"

#include "GameSystem/Unit/UnitBase.h"
#include "GameSystem/Unit/UnitPlayer.h"
#include "GameSystem/Unit/UnitEnemy.h"
#include "GameSystem/Stage/StageManager.h"
#include "GameSystem/Gimmick/GimmickGoal.h"
#include "GameSystem/Gimmick/GimmickSwitch.h"

NS_SINGLETON_INSTANCE_DEFINE(CCollisionManager);

CCollisionManager::CCollisionManager()
	: m_pcUnitPlayer(nullptr)
	, m_pcStageM(nullptr)
	, m_pcGimmickSwitch(nullptr)
	, m_pcGoalUnit(nullptr)
	, m_pcGoalTrigger(nullptr)
	, m_pcEnemyInvokeTrigger(nullptr)
	, m_pcDangerEnemyInvokeTrigger(nullptr)
	, m_pPassableArea(nullptr)
{

}

CCollisionManager::~CCollisionManager()
{
	Term();
}

bool CCollisionManager::Init()
{
	//-5,25
	CLASS_NEW(m_pcEnemyInvokeTrigger, CCollisionSphere());
	m_pcEnemyInvokeTrigger->SetScale(10.0f);
	m_pcEnemyInvokeTrigger->SetPos(NSVector3(-5.0f, 0.0f, 25.0f));

	//25,20
	CLASS_NEW(m_pcDangerEnemyInvokeTrigger, CCollisionSphere());
	m_pcDangerEnemyInvokeTrigger->SetScale(10.0f);
	m_pcDangerEnemyInvokeTrigger->SetPos(NSVector3(25.0f, 0.0f, 20.0f));


	return true;
}

void CCollisionManager::Destroy()
{
	Term();
}

void CCollisionManager::CheckAllCollision()
{
	CCollisionManagerSys::CheckAllCollision();

	//プレイヤーユニットのステージ範囲制限
	if (m_pcUnitPlayer != nullptr && m_pcStageM != nullptr)
	{
		if (m_pcUnitPlayer->IsMoving())
		{
			CModel* pcPlayerModel = m_pcUnitPlayer->GetModel();
			const NSVector3& vMoveBeforePos = m_pcUnitPlayer->GetBeforeMovingPos();
			CheckUnitRaytoStageAABB(
				pcPlayerModel,
				pcPlayerModel->GetPos() - vMoveBeforePos);
		}

//#if defined(_DEBUG)
//		NSVector3 vPos = m_pcUnitPlayer->GetModel()->GetPos();
//		NS_Report(_T("PlayerPos x:%f,y:%f,z:%f\n"), vPos.x, vPos.y, vPos.z);
//#endif
	}

	if (m_pcUnitPlayer != nullptr)
	{
		//エネミーとの当たり判定(ダメージ判定)
		for (s32 i = 0; i < (s32)m_pcaUnitEnemy.size(); ++i)
		{
			CUnitBase* pcUnitEnemy = m_pcaUnitEnemy[i];
			if (pcUnitEnemy != nullptr)
			{
				if (IsIntersect(pcUnitEnemy->GetCollision(), m_pcUnitPlayer->GetCollision()))
				{
					//エネミーと接触した。ゲームオーバー
					m_pcUnitPlayer->OnDead();
				}
			}
		}
	}

	if (m_pcGimmickSwitch != nullptr)
	{
		//スイッチチェック
		if (!m_pcGimmickSwitch->IsSwitchOn() && IsOpenSwitch())
		{
			CUtilSound::PlaySE(eSOUNDID_SWITCH);
			m_pcGimmickSwitch->SetSwitchOnOff(true);
		}

		//スイッチ起動中に移動可能壁にめりこまないように、その壁の中にいた場合はカウントを強制的に停止する
		if (m_pcGimmickSwitch->IsSwitchOn() && m_pcUnitPlayer != nullptr && m_pPassableArea != nullptr)
		{
			CModel* pcModel = m_pcUnitPlayer->GetModel();
			if (pcModel != nullptr)
			{
				if(IsContainPointInAABB(pcModel->GetPos(), *m_pPassableArea))
					m_pcGimmickSwitch->StopCount();
				else
					m_pcGimmickSwitch->ResumeCount();
			}
		}
	}

	//ゴールチェック
	if (IsGoalUnit())
	{
		m_pcUnitPlayer->OnGoal();
		m_pcGoalUnit->OnGoal();
	}

	//エネミー起動チェック
	if (m_pcUnitPlayer != nullptr && m_pcEnemyInvokeTrigger != nullptr)
	{
		if (IsIntersect(m_pcUnitPlayer->GetCollision(), m_pcEnemyInvokeTrigger))
		{
			s32 nEnemyID = 1;
			if (nEnemyID >= 0 && nEnemyID <(s32)m_pcaUnitEnemy.size())
			{
				if(m_pcaUnitEnemy[nEnemyID] != nullptr)
					m_pcaUnitEnemy[nEnemyID]->InvokeMove();
			}
			//NS_Report(_T("Invokeenemy2"));
			SAFE_DELETE(m_pcEnemyInvokeTrigger);
		}
	}
		
	//ラストエネミー起動チェック
	if (m_pcUnitPlayer != nullptr && m_pcDangerEnemyInvokeTrigger != nullptr)
	{
		if (IsIntersect(m_pcUnitPlayer->GetCollision(), m_pcDangerEnemyInvokeTrigger))
		{
			s32 nEnemyID = 2;
			if (nEnemyID >= 0 && nEnemyID < (s32)m_pcaUnitEnemy.size())
			{
				if (m_pcaUnitEnemy[nEnemyID] != nullptr)
					m_pcaUnitEnemy[nEnemyID]->InvokeMove();
			}
			//NS_Report(_T("Invokelastenemy"));
			SAFE_DELETE(m_pcDangerEnemyInvokeTrigger);
		}
	}
}

void CCollisionManager::SetUnitPlayer(CUnitPlayer* pUnitPlayer)
{
	m_pcUnitPlayer = pUnitPlayer;
}

void CCollisionManager::SetStageManager(CStageManager* pStageM)
{
	m_pcStageM = pStageM;
}

void CCollisionManager::SetGoalUnit(CUnitPlayer* pUnit)
{
	m_pcGoalUnit = pUnit;
}

void CCollisionManager::SetGimmickSwitch(CGimmickSwitch* pGimmick)
{
	m_pcGimmickSwitch = pGimmick;
}

void CCollisionManager::AddUnitEnemy(CUnitEnemy* pUnitEnemy)
{
	m_pcaUnitEnemy.push_back(pUnitEnemy);
}

void CCollisionManager::SetPassableAreaAABB(CCollisionAABB* pAABB)
{
	m_pPassableArea = pAABB;
}

void CCollisionManager::SetupGoalCollision()
{
	NS_ASSERT(m_pcGoalTrigger == false);

	CLASS_NEW(m_pcGoalTrigger, CCollisionSphere());
	m_pcGoalTrigger->SetScale(10.0f);
	NSVector3 vGoal = GlobalParam::vPlayerGoalPos;
	m_pcGoalTrigger->SetPos(vGoal);
}

//---------------------------------------------------------------------------
/*!
	@brief		ユニットとステージのコリジョンチェック
	@param[in]	pcUnit			対象ユニット
	@param[in]	vMoveVec		ユニットの移動ベクトル
	@return		ヒットしたか
*/
//---------------------------------------------------------------------------
bool CCollisionManager::CheckUnitRaytoStageAABB(class CUtilCoord* pcUnit, const NSVector3& vMoveVec)
{
	if (pcUnit == nullptr)
	{
		return false;
	}

	NSVector3 vZAt = pcUnit->GetZAt();							// レイ方向
	f32 fOffSet = 0.001f;
	NSVector3 vRayOffset = vZAt * fOffSet;						// レイオフセット。入れないとAABBに残りっぱなので
	//vRayOffset.SetZero();
	NSVector3 vRayStartPos = pcUnit->GetPos() + vRayOffset;		// レイ開始地点

	//ステージ総当り
	s32 nSize = m_pCollisionList.size();
	for (s32 i = 0; i < nSize; ++i)
	{
		f32 fT = 0.0f;					//レイ視点と衝突点との距離(CheckRayの出力)
		NSVector3 vHitNormal, vHitPos;	//衝突点法線、衝突点ワールド位置(CheckRayの出力)

		if (m_pCollisionList[i] != nullptr && m_pCollisionList[i]->IsCollisionEnable() &&
			m_pCollisionList[i]->GetColGeomType() == eCOL_GEOM_AABB && m_pCollisionList[i]->GetColObjectType() == eCOL_OBJECT_STAGE &&
			CheckRay(vRayStartPos, vZAt, *(static_cast<CCollisionAABB*>(m_pCollisionList[i])), fT, &vHitNormal, &vHitPos) && fT <= 0.0f)
		{
			//衝突してるので壁ズリ発生	 
			NSVector3 vCalcNormalPos;
			NSVector3 vCalcPos = UtilMath::CalcProjVec(&vCalcNormalPos, vMoveVec, vHitNormal);
			vCalcPos.y = 0.0f;

			pcUnit->SetPos(vHitPos + vCalcPos - vRayOffset);

			//壁ズリ後の座標が他のステージAABBに含まれていないかチェック。
			//@todo AABB同士のリストを作ったりすればより高速化できる
			for (s32 j = 0; j < nSize; ++j)
			{
				if (j != i && m_pCollisionList[j] != nullptr && m_pCollisionList[j]->IsCollisionEnable() &&
					m_pCollisionList[j]->GetColGeomType() == eCOL_GEOM_AABB && m_pCollisionList[j]->GetColObjectType() == eCOL_OBJECT_STAGE)
				{
					CCollisionAABB& cAABB = *(static_cast<CCollisionAABB*>(m_pCollisionList[j]));
					if (IsContainPointInAABB(pcUnit->GetPos(), cAABB))
					{
						vRayStartPos = pcUnit->GetPos();
						NSVector3 vOffsetHitpos = vHitPos - vRayOffset;	//オフセットを考慮したヒット位置
						vZAt = vRayStartPos - vOffsetHitpos;
						vZAt.Normalize();
						vRayOffset = vZAt * fOffSet;

						//現在の座標（壁に埋まっている）から壁へのヒット位置へのベクトルをレイとして、適正座標を出す
						if (CheckRay(vRayStartPos, vZAt, cAABB, fT, &vHitNormal, &vHitPos) && fT <= 0.0f)
						{
							pcUnit->SetPos(vHitPos - vRayOffset);
							break;
						}
					}
				}
			}

			break;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		ゴールチェック
	@param[in]	pUnitColSphere	ユニットコリジョン
	@return		ヒットしたか
*/
//---------------------------------------------------------------------------
bool CCollisionManager::IsGoalUnit()
{
	if (m_pcUnitPlayer == nullptr || m_pcGoalUnit == nullptr || m_pcGoalTrigger == nullptr)
	{
		return false;
	}
	return IsIntersect(m_pcUnitPlayer->GetCollision(), m_pcGoalTrigger);
}


//---------------------------------------------------------------------------
/*!
	@brief		プレイヤーとスイッチ開閉チェック
	@param[in]	pUnitColSphere	ユニットコリジョン
	@return		ヒットしたか
*/
//---------------------------------------------------------------------------
bool CCollisionManager::IsOpenSwitch()
{
	if (m_pcUnitPlayer == nullptr || m_pcGimmickSwitch == nullptr)
	{
		return false;
	}

	return IsIntersect(m_pcUnitPlayer->GetCollision(), m_pcGimmickSwitch->GetCollision());
}

void CCollisionManager::Term()
{
	m_pPassableArea = nullptr;

	SAFE_DELETE(m_pcDangerEnemyInvokeTrigger);
	SAFE_DELETE(m_pcEnemyInvokeTrigger);
	SAFE_DELETE(m_pcGoalTrigger);

	m_pcGimmickSwitch = nullptr;
	m_pcGoalUnit = nullptr;
	m_pcStageM = nullptr;
	m_pcUnitPlayer = nullptr;
	m_pcaUnitEnemy.clear();
}

//EOF
