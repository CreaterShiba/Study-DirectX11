#include "UtilSound.h"
#include "FrameWork/Sound/SoundManager.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilSound.cpp
	@brief		サウンドユーティリティ
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

s32 CUtilSound::PlaySE(const E_SOUND_ID eSoundID)
{
	CSoundManager* pcSoundM = CSoundManager::GetInstance();
	NS_ASSERT(pcSoundM);

	return pcSoundM->PlaySE(eSoundID);
}

s32 CUtilSound::PlayBGM(const E_SOUND_ID eSoundID)
{
	CSoundManager* pcSoundM = CSoundManager::GetInstance();
	NS_ASSERT(pcSoundM);

	return pcSoundM->PlayBGM(eSoundID);
}