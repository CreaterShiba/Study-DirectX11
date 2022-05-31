#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SceneDebug.h
	@brief		デバッグシーン
	@author		shiba
	@date		2012/10/7 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Scene/SceneBase.h"

class CSceneDebug : public CSceneBase
{
public:
	CSceneDebug();
	virtual ~CSceneDebug();

	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();	//仮想関数はコンストラクタやデストラクタから呼び出さないよう注意

private:

private:
	void Term();
};

//EOF
