#pragma once

#include "MotionEventData.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		MotionEvent.h
	@brief		モーションイベント
	@author		shiba
	@date		2012/10/31 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
class CMotionEvent 
{
public:
	CMotionEvent();
	~CMotionEvent();

	//モーションイベントチェック
	void GetAvailableMotEvent(const f32 fCheckStartTime, const f32 fCheckEndTime, std::vector< CMotionEventData* >* paEventParam );

	//モーションデータセット
	void SetupMotionEventParam(
		const s32 nIndex,
		const CMotionEventData::E_MOT_EVENT_TYPE eEventType,
		const f32 fTime,
		const s32 nParam1, const s32 nParam2);
public:
	const static s32 MOTION_EVENT_MAX = 4;	//!< 1モーションあたりの最大イベント数

private:
	std::vector<CMotionEventData> m_cMotionEventData;	//!< イベントデータ

};

//EOF
