/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDeviceKeyPad.cpp
	@brief		パッドクラス
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "InputDeviceKeyPad.h"
#include "InputManager.h"

#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   サンプルから引っ張ってきたデッドゾーン値
const f32 MaxAxisValue = 32768.0f;

// DirectInputのキーボードのキーコードマップ
u32 CInputDeviceKeyPad::m_aPadMap[ePAD_BUTTONMAX] =
{
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	0,0,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_START,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CInputDeviceKeyPad::CInputDeviceKeyPad() 
: m_nPadIndex(-1)
, m_nFirstRepeatCount(DEFAULT_REPEAT_FIRST_CNT)
, m_nNextRepeatCount(DEFAULT_REPEAT_NEXT_CNT)
{
	ZeroMemory(&m_cControllerState, sizeof(CONTROLLER_STATE));
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CInputDeviceKeyPad::~CInputDeviceKeyPad()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyPad::Init()
{
	m_eInputDeviceType = eINPUT_DEVICE_TYPE_PAD;

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyPad::Update()
{
	if (m_nPadIndex >= 0 && CInputManager::CONTROLLER_MAX > m_nPadIndex)
	{
		DWORD dwResult = XInputGetState(m_nPadIndex, &m_cControllerState.state);

		if (dwResult == ERROR_SUCCESS)
		{
			m_cControllerState.bConnected = true;

			XINPUT_STATE& xState = m_cControllerState.state;

			m_vLeftPadDir.SetZero();
			m_vRightPadDir.SetZero();

			//デッドゾーン内の数値なら0とする
			if ((xState.Gamepad.sThumbLX < INPUT_DEADZONE &&
				xState.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
				(xState.Gamepad.sThumbLY < INPUT_DEADZONE &&
					xState.Gamepad.sThumbLY > -INPUT_DEADZONE))
			{
				xState.Gamepad.sThumbLX = 0;
				xState.Gamepad.sThumbLY = 0;
			}

			if ((xState.Gamepad.sThumbRX < INPUT_DEADZONE &&
				xState.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
				(xState.Gamepad.sThumbRY < INPUT_DEADZONE &&
					xState.Gamepad.sThumbRY > -INPUT_DEADZONE))
			{
				xState.Gamepad.sThumbRX = 0;
				xState.Gamepad.sThumbRY = 0;
			}
			m_vLeftPadDir.x = xState.Gamepad.sThumbLX;
			m_vLeftPadDir.y = xState.Gamepad.sThumbLY;
			m_vLeftPadDir = m_vLeftPadDir / MaxAxisValue;

			m_vRightPadDir.x = xState.Gamepad.sThumbRX;
			m_vRightPadDir.y = xState.Gamepad.sThumbRY;
			m_vRightPadDir = m_vRightPadDir / MaxAxisValue;

			//入力パラメータの取得
			WORD wButtons = xState.Gamepad.wButtons;

			CPadData* pcPaddata = &m_PadData;
			if (pcPaddata)
			{
				for (int i = 0; i < ePAD_BUTTONMAX; i++)
				{
					pcPaddata->byOldButton[i] = pcPaddata->byButton[i];
					pcPaddata->byButton[i] = 0;
					pcPaddata->byTriggerButton[i] = 0;

					WORD wbitButton = wButtons;
					if (wbitButton & m_aPadMap[i])
					{
						pcPaddata->byButton[i] |= ePAD_KEY_ON;
					}

					pcPaddata->byTriggerButton[i] = (~pcPaddata->byOldButton[i] & pcPaddata->byButton[i]);
					pcPaddata->byReleaseButton[i] = (pcPaddata->byOldButton[i] & ~pcPaddata->byButton[i]);
					pcPaddata->byRepeatButton[i] = 0;

					if (wbitButton & m_aPadMap[i])
					{
						// 押されてる
						pcPaddata->nRepeatCount[i]--;
						if (pcPaddata->nRepeatCount[i] <= 0)
						{
							// リピート
							if (!(pcPaddata->byOldButton[i] & ePAD_KEY_ON) && m_nFirstRepeatCount > 0)
							{
								// 初回
								pcPaddata->nRepeatCount[i] = m_nFirstRepeatCount;
							}
							else
							{
								pcPaddata->nRepeatCount[i] = m_nNextRepeatCount;
							}
							pcPaddata->byRepeatButton[i] |= ePAD_KEY_ON;
						}
					}
					else
					{
						// 押されていない
						pcPaddata->nRepeatCount[i] = 0;
					}
				}
			}
		}
		else
		{
			m_cControllerState.bConnected = false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		入力されているか？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyPad::IsPush( const u32 unButton )
{
	return (m_PadData.byButton[unButton] & ePAD_KEY_ON);
}

//---------------------------------------------------------------------------
/*!
	@brief		入力された瞬間？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyPad::IsTrigger( const u32 unButton )
{
	return (m_PadData.byTriggerButton[unButton] & ePAD_KEY_ON);
}

//---------------------------------------------------------------------------
/*!
	@brief		ボタンを離した瞬間？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyPad::IsRelease( const u32 unButton )
{
	return (m_PadData.byReleaseButton[unButton] & ePAD_KEY_ON);
}

//---------------------------------------------------------------------------
/*!
	@brief		連打されているか？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyPad::IsRepeat( const u32 unButton )
{
	return (m_PadData.byRepeatButton[unButton] & ePAD_KEY_ON);
}

const NSVector2& CInputDeviceKeyPad::GetLeftStickDir() const
{
	return m_vLeftPadDir;
}

const NSVector2& CInputDeviceKeyPad::GetRightStickDir() const
{
	return m_vRightPadDir;
}


//EOF