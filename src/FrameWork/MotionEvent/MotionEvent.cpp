/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		MotionEvent.cpp
	@brief		モーションイベント
	@author		shiba
	@date		2012/10/31 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "MotionEvent.h"

CMotionEvent::CMotionEvent()
{
	m_cMotionEventData.reserve(MOTION_EVENT_MAX);
	for (s32 i = 0; i < CMotionEvent::MOTION_EVENT_MAX; ++i)
	{
		m_cMotionEventData.push_back(CMotionEventData());
	}
}

CMotionEvent::~CMotionEvent()
{

}

//---------------------------------------------------------------------------
/*!
	@brief	データセット
*/
//---------------------------------------------------------------------------
void CMotionEvent::SetupMotionEventParam(
	const s32 nIndex, 
	const CMotionEventData::E_MOT_EVENT_TYPE eEventType, const f32 fTime,
	const s32 nParam1, const s32 nParam2)
{
	if (nIndex >= 0 && nIndex < MOTION_EVENT_MAX)
	{
		CMotionEventData& data = m_cMotionEventData[nIndex];
		data.m_eEventType = eEventType;
		data.m_fMotionEventTriggerTime = fTime;
		data.m_cEventParam.m_naEventParam[0] = nParam1;
		data.m_cEventParam.m_naEventParam[1] = nParam2;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		モーションイベント発行チェック。Motion側で発行済チェックは行うこと。
	@param[in]	fCheckStartTime イベントチェックタイム開始時点
	@param[in]	fCheckEndTime	イベントチェックタイム終了時点
	@param[out]	paEventParam	有効モーションイベント配列
*/
//---------------------------------------------------------------------------
void CMotionEvent::GetAvailableMotEvent(const f32 fCheckStartTime, const f32 fCheckEndTime, std::vector<CMotionEventData *>* paEventParam )
{
	if (paEventParam == nullptr || fCheckStartTime < 0.0f)
	{
		return;
	}

	std::vector<CMotionEventData*>& data = *paEventParam;
	data.clear();

	for (s32 i = 0; i < CMotionEvent::MOTION_EVENT_MAX; ++i)
	{
		CMotionEventData& cMotionEvent = m_cMotionEventData[i];
		if (cMotionEvent.m_eEventType != CMotionEventData::eMAX)
		{
			//イベントチェック
			if (fCheckStartTime <= cMotionEvent.m_fMotionEventTriggerTime && fCheckEndTime >= cMotionEvent.m_fMotionEventTriggerTime)
			{
				data.push_back(&m_cMotionEventData[i]);
			}
		}
		else
		{
			break;	//未設定のタイプがあったら終える。

		}
	}
}

//EOF
