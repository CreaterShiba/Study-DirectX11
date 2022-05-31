/*==========================================================================*/
/*==========================================================================*/
/*!
@file		ntg.cpp
@brief		エントリーポイント
@author		shiba
@date		2012/2/28 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include <crtdbg.h>
#include "GameSystem/GameSystem.h"

//---------------------------------------------------------------------------
/*!
@brief		エントリーポイント
*/
//---------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	NS_UNUSED(hInstance);
	NS_UNUSED(nCmdShow);

	// メモリチェック
#if defined( WIN32 ) 
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//setlocale(LC_CTYPE, "jpn");

	CGameSystem cGameSystem;
	if (cGameSystem.Init())
	{
		//ゲーム処理
		cGameSystem.GameLoop();
	}
	else
	{
		//@todo 失敗処理入れておくこと
	}

	return 0;
}

//EOF
