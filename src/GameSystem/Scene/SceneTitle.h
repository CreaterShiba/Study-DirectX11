#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SceneTitle.h
	@brief		タイトルシーン
	@author		shiba
	@date		2015/07/13 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Scene/SceneBase.h"

class CSceneTitle : public CSceneBase
{
public:
	CSceneTitle();
	virtual ~CSceneTitle();

	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();	//仮想関数はコンストラクタやデストラクタから呼び出さないよう注意

private:
	class CSprite* m_TestSprite;

private: 
	void Term();
};

//EOF