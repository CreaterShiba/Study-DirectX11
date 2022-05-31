#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilSound.h
	@brief		サウンドユーティリティ
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Sound/SoundID.h"

class CUtilSound
{
public:
	static s32 PlaySE(const E_SOUND_ID eSoundID);
	static s32 PlayBGM(const E_SOUND_ID eSoundID);
};

//EOF