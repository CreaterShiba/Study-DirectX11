#pragma once

#include "StageData.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		StageManager.h
	@brief		ステージ管理
	@author		shiba
	@date		2015/07/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class CCollisionAABB;
class CStageManager
{
public:
	CStageManager();
	~CStageManager();

	bool Init();

private:
	//@brief 開閉エリア開け閉め
	void OpenPassableArea();
	void ClosePassableArea();

private:
	//ステージオブジェクト
	class CubeParam
	{
	public:
		CubeParam()
			: m_eTypeID(E_STAGE_TYPE_ID::eNONE)
			, m_nWidthIdx(0)
			, m_nHeightIdx(0)
			, m_bPassable(false)
			, m_nInstancingIndex(-1)
		{}

	public:
		E_STAGE_TYPE_ID m_eTypeID;	//ブロックタイプ

		s32 m_nWidthIdx;			//横インデックス
		s32 m_nHeightIdx;			//縦インデックス

		NSVector2 m_vPos;			//座標
		bool m_bPassable;			//通行可能エリア

		s32 m_nInstancingIndex;		//インスタンシングインデックス
	};

private:
	class CModel* m_pcStageFloor;						//!< 床モデル
	std::vector< class CModel* > m_paStageCubeModel;	//!< ステージモデル(インスタンス使用)

	CubeParam* m_paCubeParam;							//!< 各ブロックのパラメタ管理
	s32 m_nStageSize;									//!< ステージ大きさ

	std::vector< class CCollisionGeomBase* > m_paStageColData;	//!< ステージコリジョン

	s32 m_nPassableAreaColID;	//!< 移動可能エリアコリジョンID

	class CGimmickSwitch* m_pcGimmickSwitch;	//!< スイッチギミック
	class CUnitPlayer* m_pcGoalUnit;			//<! ゴールユニット
	//class CGimmickGoal* m_pcGimmickGoal;		//!< ゴールギミック
	
private: 
	void AddStageAABB(const NSVector3& vMin, const NSVector3& vMax, const NSVector3& vPos);
	void OnOrOffPassableCubeModel(const bool bOnOff);	//モデル上下にずらすだけ
	void Term();
};

//EOF

