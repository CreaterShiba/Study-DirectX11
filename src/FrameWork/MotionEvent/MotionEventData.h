#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		MotionEventData.h.h
	@brief		モーションイベントデータ
	@author		shiba
	@date		2012/10/31 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

class CMotionEventParam
{
public:
	CMotionEventParam()
	{
		for (auto &param : m_naEventParam)
		{
			param = 0;
		}
	}
	const static s32 MOTION_EVENT_PARAM_SIZE = 2;
	s32 m_naEventParam[MOTION_EVENT_PARAM_SIZE];	//!< イベント用汎用パラメータ
	
};

class CMotionEventData
{
public:
	enum E_MOT_EVENT_TYPE
	{
		eSOUND_PLAY,	//SE再生
		eCOLLISION,		//コリジョン発生
		eEFFECT,		//エフェクト発生
		eMAX,
	};
public:
	CMotionEventData() :
		m_eEventType(eMAX),
		m_fMotionEventTriggerTime(0.0f)
	{}

	E_MOT_EVENT_TYPE m_eEventType;		//!< 自身のイベントタイプ
	f32 m_fMotionEventTriggerTime;		//!< トリガータイム
	CMotionEventParam m_cEventParam;	//!< イベントパラメータ
};
