#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SceneManager.h
	@brief		シーンマネージャ
	@author		shiba
	@date		2012/9/29 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

class CSceneManager
{
public:
	NS_SINGLETON(CSceneManager);
	CSceneManager();
	~CSceneManager();

	bool Init();

public:
	enum E_SCENE
	{
		eNONE,
#if defined(_DEBUG)
		eDEBUG,		//デバグシーン
#endif
		eTITLE,		//タイトル
		eMAIN,		//メイン
		eENDING,	//エンド
		eMAX
	};

	void LoadScene(const E_SCENE eScene);
	bool IsSceneLoadEnd();

private:
	E_SCENE m_eIgnitionScene;
	class CSceneBase* m_pActiveScene;

private:
	void Term();
};

//EOF
