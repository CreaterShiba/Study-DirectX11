#pragma once

#include <functional>
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GimmickSwitch.h
	@brief		スイッチ
	@author		shiba
	@date		2015/07/15 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "GimmickBase.h"

class CGimmickSwitch : public CGimmickBase
{
public:
	CGimmickSwitch();
	~CGimmickSwitch();

	virtual bool Init();
	virtual void Update(const f32 fDelta);
	virtual void Destroy();

public:
	void SetSwitchOnOff(const bool bOnOff);
	bool IsSwitchOn() const
	{
		return m_bSwitchOnOff;
	}

	void SetSwitchOnEventCallback(const std::function<void()>& func)
	{
		m_onFunction = func;
	}

	void SetSwitchOffEventCallback(const std::function<void()>& func)
	{
		m_offFunction = func;
	}

	void StopCount()
	{
		m_bCountStop = true;
	}
	void ResumeCount()
	{
		m_bCountStop = false;
	}
private:
	bool m_bCountStop;
	bool m_bSwitchOnOff;
	f32 m_fSwitchCounter;
	const f32 SWITCH_COUNT_MAX = 3.0f;

	std::function<void()> m_onFunction;		//!< スイッチON関数コール
	std::function<void()> m_offFunction;	//!< スイッチOFF関数コール
};

//EOF
