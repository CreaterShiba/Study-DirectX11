/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilInput.cpp
	@brief		入力ユーティリティ
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "UtilInput.h"
#include "FrameWork/Input/InputManager.h"

//---------------------------------------------------------------------------
/*!
	@brief		入力されているか？
	@param[in]	eButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CUtilInput::IsPush(const E_PAD_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT( pcInputM );

	return pcInputM->IsPush(eButton, eINPUT_DEVICE_TYPE_PAD, nPadID);
}

bool CUtilInput::IsPush(const E_KEYBOARD_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	return pcInputM->IsPush(eButton, eINPUT_DEVICE_TYPE_KEYBOARD, nPadID);
}

bool CUtilInput::IsPush(const E_MOUSE_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	return pcInputM->IsPush(eButton, eINPUT_DEVICE_TYPE_MOUSE, nPadID);
}

//---------------------------------------------------------------------------
/*!
	@brief		入力された瞬間？
	@param[in]	unButton	ボタン定義
	@param[in]	eType	入力形式
*/
//---------------------------------------------------------------------------
bool CUtilInput::IsTrigger(const E_PAD_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT( pcInputM );

	return pcInputM->IsTrigger(eButton, eINPUT_DEVICE_TYPE_PAD, nPadID);
}

bool CUtilInput::IsTrigger(const E_KEYBOARD_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	return pcInputM->IsTrigger(eButton, eINPUT_DEVICE_TYPE_KEYBOARD, nPadID);
}

bool CUtilInput::IsTrigger(const E_MOUSE_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	return pcInputM->IsTrigger(eButton, eINPUT_DEVICE_TYPE_MOUSE, nPadID);
}

//---------------------------------------------------------------------------
/*!
	@brief		ボタンを離した瞬間？
	@param[in]	unButton	ボタン定義
	@param[in]	eType	入力形式
*/
//---------------------------------------------------------------------------
bool CUtilInput::IsRelease(const E_PAD_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT( pcInputM );

	return pcInputM->IsRelease(eButton, eINPUT_DEVICE_TYPE_PAD, nPadID);
}

bool CUtilInput::IsRelease(const E_KEYBOARD_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	return pcInputM->IsRelease(eButton, eINPUT_DEVICE_TYPE_KEYBOARD, nPadID);
}

bool CUtilInput::IsRelease(const E_MOUSE_BUTTON eButton, const int nPadID /* = 0 */)
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	return pcInputM->IsRelease(eButton, eINPUT_DEVICE_TYPE_MOUSE, nPadID);
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスのカーソル座標を獲得
	@return NSVector2 マウス座標
*/
//---------------------------------------------------------------------------
const NSVector2& CUtilInput::GetMouseCursorPos()
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT( pcInputM );

	return pcInputM->GetMouseCursorPos();
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスの移動ベクトルを獲得
	@return NSVector2 移動ベクトル
*/
//---------------------------------------------------------------------------
const NSVector2& CUtilInput::GetMouseCursorDir()
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT( pcInputM );

	return pcInputM->GetMouseCursorDir();
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスの移動ベクトル(正規化)を獲得
	@return NSVector2 正規化向き
*/
//---------------------------------------------------------------------------
const NSVector2 CUtilInput::GetMouseCursorDirNom()
{
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT( pcInputM );

	return pcInputM->GetMouseCursorDirNom();
}

//---------------------------------------------------------------------------
/*!
	@brief	コントローラーの左スティックの向きの取得
	@return NSVector2 正規化向き
*/
//---------------------------------------------------------------------------
const NSVector2 CUtilInput::GetLeftStickDir( const int nPadID /* = 0 */ )
{
	NSVector2 dir;
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	dir = pcInputM->GetLeftStickDir(nPadID);

	return dir;
}


//---------------------------------------------------------------------------
/*!
	@brief	コントローラーの右スティックの向きの取得
	@return NSVector2 正規化向き
*/
//---------------------------------------------------------------------------
const NSVector2 CUtilInput::GetRightStickDir( const int nPadID /* = 0 */ )
{
	NSVector2 dir;
	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);

	dir = pcInputM->GetRightStickDir(nPadID);

	return dir;
}

//EOF
