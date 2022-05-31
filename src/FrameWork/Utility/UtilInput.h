#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilInput.h
	@brief		入力ユーティリティ
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Utility/UtilDefine.h"
#include "FrameWork/Input/InputDefine.h"

class CUtilInput
{
public:
	CUtilInput() {}
	~CUtilInput() {}

	//--------------------------------------------------------------------
	// インターフェイス
	//--------------------------------------------------------------------
	//@brief 押しているか
	static bool IsPush(const E_KEYBOARD_BUTTON eButton, const int nPadID = 0);
	static bool IsPush(const E_MOUSE_BUTTON eButton, const int nPadID = 0);
	static bool IsPush(const E_PAD_BUTTON eButton, const int nPadID = 0);

	//@brief 押した瞬間であるか
	static bool IsTrigger(const E_KEYBOARD_BUTTON eButton, const int nPadID = 0);
	static bool IsTrigger(const E_MOUSE_BUTTON eButton, const int nPadID = 0);
	static bool IsTrigger(const E_PAD_BUTTON eButton, const int nPadID = 0);

	//@brief 離した瞬間であるか
	static bool IsRelease(const E_KEYBOARD_BUTTON eButton, const int nPadID = 0);
	static bool IsRelease(const E_MOUSE_BUTTON eButton, const int nPadID = 0);
	static bool IsRelease(const E_PAD_BUTTON eButton, const int nPadID = 0);

	//@brief カーソル座標を取得
	static const NSVector2& GetMouseCursorPos(); 
	//@brief 移動ベクトル
	static const NSVector2& GetMouseCursorDir();
	//@brief 移動ベクトル(正規化ずみ)
	static const NSVector2 GetMouseCursorDirNom();

	//@brief スティック移動ベクトル(大きさも含む)
	static const NSVector2 GetLeftStickDir( const int nPadID  = 0 );
	static const NSVector2 GetRightStickDir( const int nPadID = 0 );

private:
	//@brief デバイスタイプ取得(コア)
	//E_INPUT_DEVICE_TYPE GetDeviceTypeCore() const;

};

//EOF
