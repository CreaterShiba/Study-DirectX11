#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SceneBase.h
	@brief		シーン
	@author		shiba
	@date		2012/9/29 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "SceneManager.h"

#include "FrameWork/Task/Task.h"

/*==========================================================================*/
/*!
	@class	CSceneBase
	@brief	シーンベース
*/
/*==========================================================================*/
class CSceneBase : public CTask
{
public:
	CSceneBase() 
	: m_eSceneType(CSceneManager::E_SCENE::eNONE)
	{}
	virtual ~CSceneBase(){}

	virtual void InitScene() {}
	virtual void EndScene() {}

protected:
	CSceneManager::E_SCENE m_eSceneType;

};

//EOF
