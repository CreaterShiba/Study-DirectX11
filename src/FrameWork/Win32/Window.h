#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Window.h
	@brief		ウィンドウクラス
	@author		shiba
	@date		2012/2/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include <windows.h>

/*==========================================================================*/
/*!
	@class	CWindow
	@brief	ウィンドウクラス
*/
/*==========================================================================*/
class CWindow
{
	NS_SINGLETON( CWindow );
	CWindow();
	~CWindow();

public:
	//@brief 初期化
	bool Init();

	//@brief ウィンドウ作成
	bool CreateGameWindow(const int nSizeX, const int nSizeY, const TCHAR* strTitle);

	//!< ウィンドウハンドル
	HWND m_Hwnd;		
	bool m_bWindowEnd;

private:
	//@todo ここにはおかないけど、とりあえずね。
	s32 m_nWindowSizeX;
	s32 m_nWindowSizeY;
};

//EOF
