#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SceneMain.h
	@brief		メインシーン
	@author		shiba
	@date		2015/07/13 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Scene/SceneBase.h"

class CSceneMain : public CSceneBase
{
public:
	CSceneMain();
	virtual ~CSceneMain();

	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();	//仮想関数はコンストラクタやデストラクタから呼び出さないよう注意

private:
	enum E_SCENE_STATE
	{
		eSCENE_STATE_BEGIN,
		eSCENE_STATE_MAIN,
		eSCENE_STATE_GAMEOVER,
		eSCENE_STATE_CLEAR,
		eSCENE_STATE_MAX,
	};

private:
	void OnState();
	void OffState();
	void ChangeState(const E_SCENE_STATE eState);

private:
	class CUnitPlayer* m_pcUnitPlayer;
	class CUnitEnemy* m_pcUnitEnemy;
	class CUnitEnemy* m_pcUnitEnemy2;
	class CUnitEnemy* m_pcUnitEnemy3;
	class CStageManager* m_pcStageM;

	int nPostEffectType;
	E_SCENE_STATE m_eMainState;
private:
	void Term();
};

//EOF