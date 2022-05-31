#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GameSystem.h
	@brief		ゲームシステム
	@author		shiba
	@date		2012/3/1 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#pragma comment ( lib, "winmm.lib" )

/*==========================================================================*/
/*!
	@class	CGameSystem
	@brief	ゲーム管理
*/
/*==========================================================================*/
class CGameSystem
{
public:
	CGameSystem();
	~CGameSystem();

	bool Init();		//初期化

	bool GameLoop();	//ゲームループ
	bool GameProc();	//ゲームプロシージャ
	bool CheckDevice();	//デバイス情報を取得
	bool GameDraw();
	
private:
	void Term();
	void Destory();		//破棄
private:
#if defined(_DEBUG)
	class CFPSCounter* m_DebugFps;
#endif
	class CDefferedSprite* m_DefferdSprite;	//デファード用
	class CSprite* m_GetBrightSprite;		//輝度抽出
	class CBlurSprite* m_BlurBrightSprite;	//輝度ブラー
	class CSprite* m_PostEffectSprite;		//ポストエフェクト用
};

//EOF